#ifndef NET_IP_BACKPORT_TO_2_6_9_H
#define NET_IP_BACKPORT_TO_2_6_9_H

#include_next <net/ip.h>

/*
 *	Map a multicast IP onto multicast MAC for type IP-over-InfiniBand.
 *	Leave P_Key as 0 to be filled in by driver.
 */

static inline void ip_ib_mc_map(u32 addr, char *buf)
{
	buf[0]  = 0;		/* Reserved */
	buf[1]  = 0xff;		/* Multicast QPN */
	buf[2]  = 0xff;
	buf[3]  = 0xff;
	addr    = ntohl(addr);
	buf[4]  = 0xff;
	buf[5]  = 0x12;		/* link local scope */
	buf[6]  = 0x40;		/* IPv4 signature */
	buf[7]  = 0x1b;
	buf[8]  = 0;		/* P_Key */
	buf[9]  = 0;
	buf[10] = 0;
	buf[11] = 0;
	buf[12] = 0;
	buf[13] = 0;
	buf[14] = 0;
	buf[15] = 0;
	buf[19] = addr & 0xff;
	addr  >>= 8;
	buf[18] = addr & 0xff;
	addr  >>= 8;
	buf[17] = addr & 0xff;
	addr  >>= 8;
	buf[16] = addr & 0x0f;
}

#define inet_get_local_port_range(a, b) { *(a) = sysctl_local_port_range[0]; *(b) = sysctl_local_port_range[1]; }

#endif
