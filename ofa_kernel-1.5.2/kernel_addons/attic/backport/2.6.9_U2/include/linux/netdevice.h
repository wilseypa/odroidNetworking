#ifndef _LINUX_NETDEVICE_BACKPORT_TO_2_6_16
#define _LINUX_NETDEVICE_BACKPORT_TO_2_6_16

#include_next <linux/netdevice.h>

static inline void netif_tx_lock(struct net_device *dev)
{
	spin_lock(&dev->xmit_lock);
	dev->xmit_lock_owner = smp_processor_id();
}

static inline void netif_tx_unlock(struct net_device *dev)
{
	dev->xmit_lock_owner = -1;
	spin_unlock(&dev->xmit_lock);
}

static inline int __netif_rx_schedule_prep(struct net_device *dev)
{
        return !test_and_set_bit(__LINK_STATE_RX_SCHED, &dev->state);
}

static inline int skb_checksum_help_to_2_6_18(struct sk_buff *skb)
{
        return skb_checksum_help(&skb, 0);
}

#define skb_checksum_help skb_checksum_help_to_2_6_18

#undef SET_ETHTOOL_OPS
#define SET_ETHTOOL_OPS(netdev, ops) \
	(netdev)->ethtool_ops = (struct ethtool_ops *)(ops)

#endif
