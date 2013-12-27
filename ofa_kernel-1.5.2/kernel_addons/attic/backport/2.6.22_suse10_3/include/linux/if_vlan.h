#ifndef __BACKPORT_LINUX_IF_VLAN_H_TO_2_6_24__
#define __BACKPORT_LINUX_IF_VLAN_H_TO_2_6_24__

#include_next <linux/if_vlan.h>

#define vlan_dev_info(x) VLAN_DEV_INFO(x)

static inline u16 vlan_dev_vlan_id(const struct net_device *dev)
{
	return vlan_dev_info(dev)->vlan_id;
}

#endif
