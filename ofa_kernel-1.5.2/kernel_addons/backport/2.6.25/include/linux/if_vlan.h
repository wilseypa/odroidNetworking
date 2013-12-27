#ifndef __BACKPORT_LINUX_IF_VLAN_H_TO_2_6_25__
#define __BACKPORT_LINUX_IF_VLAN_H_TO_2_6_25__

#include_next <linux/if_vlan.h>

#define VLAN_DEV_INFO(x) ((struct vlan_dev_info *)(x->priv))
#define vlan_dev_real_dev(netdev) (VLAN_DEV_INFO(netdev)->real_dev)
#define vlan_dev_vlan_id(netdev) (VLAN_DEV_INFO(netdev)->vlan_id)

#endif /* __BACKPORT_LINUX_IF_VLAN_H_TO_2_6_25__ */
