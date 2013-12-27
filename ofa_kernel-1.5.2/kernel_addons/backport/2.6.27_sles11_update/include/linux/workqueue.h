#ifndef __BACKPORT_WORKQUEUE_H_TO_2_6_29__
#define __BACKPORT_WORKQUEUE_H_TO_2_6_29__
#include_next <linux/workqueue.h>

static inline struct delayed_work *to_delayed_work(struct work_struct *work)
{
	return container_of(work, struct delayed_work, work);
}

#endif

