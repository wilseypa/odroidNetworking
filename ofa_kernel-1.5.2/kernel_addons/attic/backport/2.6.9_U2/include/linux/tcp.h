#ifndef __BACKPORT_LINUX_TCP_TO_2_6_21__
#define __BACKPORT_LINUX_TCP_TO_2_6_21__

#include_next <linux/tcp.h>

static inline struct tcphdr *tcp_hdr(const struct sk_buff *skb)
{
	return (struct tcphdr *)skb_transport_header(skb);
}

#endif
