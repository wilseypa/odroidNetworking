#ifndef __BACKPORT_LINUX_NOTIFIER_H_TO_2_6_26__
#define __BACKPORT_LINUX_NOTIFIER_H_TO_2_6_26__

#include_next <linux/notifier.h>

#define NETDEV_BONDING_FAILOVER NETDEV_CHANGE
#define NOTIFY_DONE		0x0000		/* Don't care */

#endif /* __BACKPORT_LINUX_NOTIFIER_H_TO_2_6_26__ */
