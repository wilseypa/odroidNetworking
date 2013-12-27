#ifndef __BACKPORT_LINUX_IF_VLAN_H_TO_2_6_20__
#define __BACKPORT_LINUX_IF_VLAN_H_TO_2_6_20__

#include_next <linux/if_vlan.h>

static inline struct net_device *vlan_group_get_device(struct vlan_group *vg, int vlan_id)
{
	return vg->vlan_devices[vlan_id];
}

static inline void vlan_group_set_device(struct vlan_group *vg, int vlan_id,
					 struct net_device *dev)
{
	vg->vlan_devices[vlan_id] = dev;
}

#endif
