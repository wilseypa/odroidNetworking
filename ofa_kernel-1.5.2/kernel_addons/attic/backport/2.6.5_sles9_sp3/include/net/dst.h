#ifndef BACKPORT_NET_DST_2_6_11
#define BACKPORT_NET_DST_2_6_11

#include <linux/skbuff.h>
#include_next <net/dst.h>

static inline u32 dst_mtu(struct dst_entry *dst)
{
	u32 mtu = dst_metric(dst, RTAX_MTU);
	/*
	 * Alexey put it here, so ask him about it :)
	 */
	barrier();
	return mtu;
}

#endif
