#include <linux/bio.h>
#include <linux/blkdev.h>
#include <linux/completion.h>
#include <linux/kernel.h>
#include <linux/mempool.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/hardirq.h>

#include <scsi/scsi.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_dbg.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_driver.h>
#include <scsi/scsi_eh.h>
#include <scsi/scsi_host.h>

int scsi_is_target_device(const struct device *dev)
{
        char *str = dev->bus_id;

	if (strncmp(str, "target", 6) == 0) {
		return 1;
	}

        return 0;
}

/**
 * scsi_internal_device_block - internal function to put a device
 *                              temporarily into the SDEV_BLOCK state
 * @sdev:       device to block
 *
 * Block request made by scsi lld's to temporarily stop all
 * scsi commands on the specified device.  Called from interrupt
 * or normal process context.
 *
 * Returns zero if successful or error if not
 *
 * Notes:
 *      This routine transitions the device to the SDEV_BLOCK state
 *      (which must be a legal transition).  When the device is in this
 *      state, all commands are deferred until the scsi lld reenables
 *      the device with scsi_device_unblock or device_block_tmo fires.
 *      This routine assumes the host_lock is held on entry.
 **/
int
scsi_internal_device_block(struct scsi_device *sdev)
{
        request_queue_t *q = sdev->request_queue;
        unsigned long flags;
        int err = 0;

        err = scsi_device_set_state(sdev, SDEV_BLOCK);
        if (err)
		return err;
                
        /*
         * The device has transitioned to SDEV_BLOCK.  Stop the
         * block layer from calling the midlayer with this device's
         * request queue.
         */
        spin_lock_irqsave(q->queue_lock, flags);
        blk_stop_queue(q);
        spin_unlock_irqrestore(q->queue_lock, flags);

        return 0;
}
EXPORT_SYMBOL_GPL(scsi_internal_device_block);

/**
 * scsi_internal_device_unblock - resume a device after a block request
 * @sdev:       device to resume
 *
 * Called by scsi lld's or the midlayer to restart the device queue
 * for the previously suspended scsi device.  Called from interrupt or
 * normal process context.
 *
 * Returns zero if successful or error if not.
 *
 * Notes:
 *      This routine transitions the device to the SDEV_RUNNING state
 *      (which must be a legal transition) allowing the midlayer to
 *      goose the queue for this device.  This routine assumes the
 *      host_lock is held upon entry.
 **/
int
scsi_internal_device_unblock(struct scsi_device *sdev)
{
        request_queue_t *q = sdev->request_queue;
        int err;
        unsigned long flags;


        /*
         * Try to transition the scsi device to SDEV_RUNNING
         * and goose the device queue if successful.
         */
        err = scsi_device_set_state(sdev, SDEV_RUNNING);
        if (err)
		return err;
                
        spin_lock_irqsave(q->queue_lock, flags);
        blk_start_queue(q);
        spin_unlock_irqrestore(q->queue_lock, flags);

        return 0;
}
EXPORT_SYMBOL_GPL(scsi_internal_device_unblock);

static void
device_block(struct scsi_device *sdev, void *data)
{
        scsi_internal_device_block(sdev);
}

static int
target_block(struct device *dev, void *data)
{
        if (scsi_is_target_device(dev))
                starget_for_each_device(to_scsi_target(dev), NULL,
                                        device_block);

        return 0;
}

void
scsi_target_block(struct device *dev)
{
        if (scsi_is_target_device(dev))
                starget_for_each_device(to_scsi_target(dev), NULL,
                                        device_block);
        else
                device_for_each_child(dev, NULL, target_block);
}
EXPORT_SYMBOL_GPL(scsi_target_block);

static void
device_unblock(struct scsi_device *sdev, void *data)
{
        scsi_internal_device_unblock(sdev);
}

static int
target_unblock(struct device *dev, void *data)
{
        if (scsi_is_target_device(dev))
                starget_for_each_device(to_scsi_target(dev), NULL,
                                        device_unblock);
        return 0;
}

void
scsi_target_unblock(struct device *dev)
{
        if (scsi_is_target_device(dev))
                starget_for_each_device(to_scsi_target(dev), NULL,
                                        device_unblock);
        else
                device_for_each_child(dev, NULL, target_unblock);
}
EXPORT_SYMBOL_GPL(scsi_target_unblock);

MODULE_LICENSE("GPL");
