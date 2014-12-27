#include "xhci.h"

#include <linux/moduleparam.h>
#include <linux/kthread.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/device.h>

int xhci_handle_event(struct xhci_hcd *xhci);
int last_trb(struct xhci_hcd *xhci, struct xhci_ring *ring,
             struct xhci_segment *seg, union xhci_trb *trb);

static bool poll = 0;
static unsigned int pollcpu = 3;

void switch_to_interrupt_mode(struct usb_hcd *hcd)
{
    struct xhci_hcd *xhci = hcd_to_xhci(hcd);
    u32 temp;

    /* Set the HCD state before we enable the irqs */
    temp = xhci_readl(xhci, &xhci->op_regs->command);
    temp |= (CMD_EIE);
    xhci_dbg(xhci, " Enable interrupts, cmd = 0x%x.\n", temp);
    xhci_writel(xhci, temp, &xhci->op_regs->command);

    temp = xhci_readl(xhci, &xhci->ir_set->irq_pending);
    xhci_dbg(xhci, " Enabling event ring interrupter %p by writing 0x%x",
        xhci->ir_set, (unsigned int) ER_IRQ_ENABLE(temp));
    xhci_writel(xhci, ER_IRQ_ENABLE(temp), &xhci->ir_set->irq_pending);
    xhci_print_ir_set(xhci, 0);
}

static void xhci_handle_event_tasklet(unsigned long data)
{
    struct xhci_hcd *xhci = (struct xhci_hcd*)data;
    u64 temp_64;
    union xhci_trb *event_ring_deq;
    dma_addr_t deq;
    unsigned long flags;

    spin_lock_irqsave(&xhci->lock, flags);

    event_ring_deq = xhci->event_ring->dequeue;

    xhci_handle_event(xhci);

    temp_64 = xhci_read_64(xhci, &xhci->ir_set->erst_dequeue);
    /* If necessary, update the HW's version of the event ring deq ptr. */
    if (event_ring_deq != xhci->event_ring->dequeue) {

        deq = xhci_trb_virt_to_dma(xhci->event_ring->deq_seg, xhci->event_ring->dequeue);
        if (deq == 0)
            xhci_warn(xhci, "WARN something wrong with SW event ring dequeue ptr.\n");

        /* Update HC event ring dequeue pointer */
        temp_64 &= ERST_PTR_MASK;
        temp_64 |= ((u64) deq & (u64) ~ERST_PTR_MASK);
        xhci_write_64(xhci, temp_64, &xhci->ir_set->erst_dequeue);
    }

    spin_unlock_irqrestore(&xhci->lock, flags);
}

int xhci_poll(void *data)
{
    struct usb_hcd  *hcd = (struct usb_hcd*)data;
    struct xhci_hcd *xhci = hcd_to_xhci(hcd);
    union xhci_trb *event_ring_deq;
    unsigned long flags;
    struct xhci_ring* ring = xhci->event_ring;
    struct xhci_segment* seg;

    struct tasklet_struct *tsklt =
        (struct tasklet_struct*)kmalloc(sizeof(struct tasklet_struct), GFP_ATOMIC);
    if (!tsklt) {
        switch_to_interrupt_mode(hcd);
        return 1;
    }

    tasklet_init(tsklt, xhci_handle_event_tasklet, (unsigned long)xhci);

    event_ring_deq = xhci->event_ring->dequeue;
    while(poll) {
        spin_lock_irqsave(&xhci->lock, flags);

        if ((le32_to_cpu(event_ring_deq->event_cmd.flags) & TRB_CYCLE) !=
                xhci->event_ring->cycle_state) {
            spin_unlock_irqrestore(&xhci->lock, flags);
            continue;
        }

        do {    /* This updates event_ring_deq to the next trb so maybe we get events quicker
                and so we dont keep checking the same cycle bit over and over. */
            if (last_trb(xhci, ring, ring->deq_seg, event_ring_deq)) {
                seg = ring->deq_seg->next;
                event_ring_deq = seg->trbs;
            } else {
                event_ring_deq++;
            }

        } while (last_trb(xhci, ring, ring->deq_seg, event_ring_deq));

        tasklet_schedule(tsklt);

        spin_unlock_irqrestore(&xhci->lock, flags);
    }

    kfree(tsklt);
    switch_to_interrupt_mode(hcd);
    return 0;
}

void switch_to_polling_mode(struct usb_hcd *hcd)
{
    struct xhci_hcd *xhci = hcd_to_xhci(hcd);
    u32 temp;
    u64 temp_64;
    struct task_struct* poll_task;

    xhci_dbg(xhci, " //Disabling event ring interrupts\n");
    temp = xhci_readl(xhci, &xhci->ir_set->irq_pending);
    xhci_writel(xhci, ER_IRQ_DISABLE(temp), &xhci->ir_set->irq_pending);
    xhci_print_ir_set(xhci, 0);

    /* Set the HCD state before we enable the irqs */
    temp = xhci_readl(xhci, &xhci->op_regs->command);
    temp &= (~CMD_EIE);
    xhci_dbg(xhci, " Disable interrupts, cmd = 0x%x.\n", temp);
    xhci_writel(xhci, temp, &xhci->op_regs->command);

    temp = xhci_readl(xhci, &xhci->op_regs->status);
    temp |= STS_EINT;
    xhci_writel(xhci, temp, &xhci->op_regs->status);

    temp_64 = xhci_read_64(xhci, &xhci->ir_set->erst_dequeue);
    xhci_write_64(xhci, temp_64 | ERST_EHB, &xhci->ir_set->erst_dequeue);

    poll_task = kthread_create(xhci_poll, hcd, "xhci polling task");
    kthread_bind(poll_task, pollcpu);
    wake_up_process(poll_task);
}

static ssize_t xhci_store_poll_status(struct device *dev, struct device_attribute *attr,
    const char *buf, size_t count)
{
    struct usb_hcd *hcd;
    unsigned int should_poll;
    int ret = -EINVAL;

    hcd = dev_get_drvdata(dev->parent);
    if (!hcd) {
        pr_warn("Invalid hcd\n");
        goto exit;
    }

    if (sscanf(buf, "%u", &should_poll) > 0)
        ret = strlen(buf);
    else
        goto exit;

    poll = (should_poll > 0) ? true : false;

    if (poll) {
        switch_to_polling_mode(hcd);
    } else {
        switch_to_interrupt_mode(hcd);
    }

exit:
    return ret;
}

static ssize_t xhci_show_poll_status(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%u\n", poll);
}

DEVICE_ATTR(poll, S_IRUGO | S_IWUGO, xhci_show_poll_status, xhci_store_poll_status);

static ssize_t xhci_store_pollcpu(struct device *dev, struct device_attribute *attr,
    const char *buf, size_t count)
{
    if (sscanf(buf, "%u", &pollcpu) > 0) {
        return strlen(buf);
    }

    return -EINVAL;
}

static ssize_t xhci_show_pollcpu(struct device *dev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%u\n", pollcpu);
}

DEVICE_ATTR(pollcpu, S_IRUGO | S_IWUGO, xhci_show_pollcpu, xhci_store_pollcpu);

