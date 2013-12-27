#ifndef __BACKPORT_NET_IP_H_TO_2_6_23__
#define __BACKPORT_NET_IP_H_TO_2_6_23__

#include_next<net/ip.h>
#define inet_get_local_port_range(a, b) { *(a) = sysctl_local_port_range[0]; *(b) = sysctl_local_port_range[1]; }

#endif
