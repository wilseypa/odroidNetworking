#ifndef BACKPORT_LINUX_WORKQUEUE_TO_2_6_23
#define BACKPORT_LINUX_WORKQUEUE_TO_2_6_23

#include_next <linux/workqueue.h>

void msleep(unsigned int msecs);

static inline int 
backport_cancel_delayed_work_sync(struct delayed_work *work)
{
	cancel_delayed_work(work);
	while (delayed_work_pending(work))
		msleep(1);
	return 0;
}

#define cancel_delayed_work_sync backport_cancel_delayed_work_sync

static inline struct delayed_work *to_delayed_work(struct work_struct *work)
{
	return container_of(work, struct delayed_work, work);
}

#endif
