#ifndef BACKPORT_LINUX_WORKQUEUE_TO_2_6_23
#define BACKPORT_LINUX_WORKQUEUE_TO_2_6_23

#include_next <linux/workqueue.h>

#define INIT_DELAYED_WORK_DEFERRABLE(_work, _func) INIT_DELAYED_WORK(_work, _func)
#define cancel_delayed_work_sync cancel_delayed_work

#endif
