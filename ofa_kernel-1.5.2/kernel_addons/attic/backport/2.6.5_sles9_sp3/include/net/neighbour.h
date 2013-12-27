#ifndef BACKPORT_LINUX_NEIGHBOUR_TO_SLES9SP3
#define BACKPORT_LINUX_NEIGHBOUR_TO_SLES9SP3

#include <net/arp.h>
#include_next <net/neighbour.h>

#define neigh_cleanup neigh_destructor

#endif
