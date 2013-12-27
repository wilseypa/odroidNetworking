#ifndef _BACKPORT_NET_IF_NET6_H_
#define _BACKPORT_NET_IF_NET6_H_

#include_next <net/if_inet6.h>

static inline void ipv6_ib_mc_map(struct in6_addr *addr, char *buf)
{
	buf[0]	= 0;	/* Reserved */
	buf[1]	= 0xff;	/* Multicast QPN */
	buf[2]	= 0xff;
	buf[3]	= 0xff;
	buf[4]	= 0xff;
	buf[5]	= 0x12;	/* link local scope */
	buf[6]	= 0x60;	/* IPv6 signature */
	buf[7]	= 0x1b;
	buf[8]	= 0;	/* P_Key */
	buf[9]	= 0;
	memcpy(buf + 10, addr->s6_addr + 6, 10);
}
#endif
