#ifndef __LINUX_IP_BACKPORT_TO_2_6_21__
#define __LINUX_IP_BACKPORT_TO_2_6_21__

#include_next <linux/ip.h>
#include <linux/skbuff.h>

static inline struct iphdr *ip_hdr(const struct sk_buff *skb)
{
	return (struct iphdr *)skb_network_header(skb);
}

#endif
