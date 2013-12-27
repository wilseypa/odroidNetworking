#ifndef __BACKPORT_SKBUFF_H_TO_2_6_26__
#define __BACKPORT_SKBUFF_H_TO_2_6_26__
#include_next <linux/skbuff.h>

static inline void skb_record_rx_queue(struct sk_buff *skb, u16 rx_queue)
{
#ifdef CONFIG_NETDEVICES_MULTIQUEUE
	skb->queue_mapping = rx_queue + 1;
#endif
}
#endif

