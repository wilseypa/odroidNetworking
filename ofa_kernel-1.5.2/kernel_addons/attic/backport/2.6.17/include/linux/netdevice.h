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

static inline void netif_tx_lock_bh(struct net_device *dev)
{
	spin_lock_bh(&dev->xmit_lock);
	dev->xmit_lock_owner = smp_processor_id();
}

static inline void netif_tx_unlock_bh(struct net_device *dev)
{
	dev->xmit_lock_owner = -1;
	spin_unlock_bh(&dev->xmit_lock);
}

static inline int skb_checksum_help_to_2_6_18(struct sk_buff *skb)
{
        return skb_checksum_help(skb, 0);
}

#define skb_checksum_help skb_checksum_help_to_2_6_18

#undef SET_ETHTOOL_OPS
#define SET_ETHTOOL_OPS(netdev, ops) \
	(netdev)->ethtool_ops = (struct ethtool_ops *)(ops)

#define NETIF_F_TSO6		0
#define NETIF_F_LRO		32768   /* large receive offload */
#define NETIF_F_IPV6_CSUM	16	/* Can checksum TCP/UDP over IPV6 */

#define dev_get_by_name(net, name) dev_get_by_name(name)
#endif
