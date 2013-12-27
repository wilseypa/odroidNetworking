#ifndef __BACKPORT_SKBUFF_H_TO_2_6_29__
#define __BACKPORT_SKBUFF_H_TO_2_6_29__
#include_next <linux/skbuff.h>

static inline void skb_record_rx_queue(struct sk_buff *skb, u16 rx_queue)
{
	skb->queue_mapping = rx_queue + 1;
}
#endif

