#ifndef _LINUX_INETDEVICE_BACKPORT_TO_2_6_17
#define _LINUX_INETDEVICE_BACKPORT_TO_2_6_17

#include_next <linux/inetdevice.h>
#include <linux/rtnetlink.h>

static inline struct net_device *xxx_ip_dev_find(u32 addr)
{
	struct net_device *dev;
	u32 ip;

	read_lock(&dev_base_lock);
	for (dev = dev_base; dev; dev = dev->next) {
		ip = inet_select_addr(dev, 0, RT_SCOPE_HOST);
		if (ip == addr) {
			dev_hold(dev);
			break;
		}
	}
	read_unlock(&dev_base_lock);

	return dev;
}

#define ip_dev_find(net, addr) xxx_ip_dev_find(addr)

#endif

