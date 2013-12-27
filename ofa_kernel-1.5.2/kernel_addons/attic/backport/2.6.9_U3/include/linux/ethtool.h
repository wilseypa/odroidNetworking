#ifndef BACKPORT_LINUX_ETHTOOL_TO_2_6_13
#define BACKPORT_LINUX_ETHTOOL_TO_2_6_13

#include_next <linux/ethtool.h>
#include <linux/netdevice.h>

#define ADVERTISED_Pause		(1 << 13)
#define ADVERTISED_Asym_Pause		(1 << 14)

static inline int ethtool_op_set_tx_hw_csum(struct net_device *dev, u32 data)
{
	if (data)
		dev->features |= NETIF_F_HW_CSUM;
	else
		dev->features &= ~NETIF_F_HW_CSUM;
	return 0;
}

#endif
