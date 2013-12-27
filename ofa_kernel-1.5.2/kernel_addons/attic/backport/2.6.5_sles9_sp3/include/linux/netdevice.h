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

#undef SET_ETHTOOL_OPS
#define SET_ETHTOOL_OPS(netdev, ops) \
	(netdev)->ethtool_ops = (struct ethtool_ops *)(ops)

#define NETDEV_TX_OK 0          /* driver took care of packet */
#define NETDEV_TX_BUSY 1        /* driver tx path was busy*/
#define NETDEV_TX_LOCKED -1     /* driver tx lock was already taken */

#endif
