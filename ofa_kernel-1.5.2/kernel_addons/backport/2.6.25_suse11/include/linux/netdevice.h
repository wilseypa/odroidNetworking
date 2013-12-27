#ifndef __BACKPORT_LINUX_NETDEVICE_H_TO_2_6_25__
#define __BACKPORT_LINUX_NETDEVICE_H_TO_2_6_25__

#include_next <linux/netdevice.h>

static inline
struct net *dev_net(const struct net_device *dev)
{
#ifdef CONFIG_NET_NS
        return dev->nd_net;
#else
        return &init_net;
#endif
}

#endif /* __BACKPORT_LINUX_NETDEVICE_H_TO_2_6_25__ */
