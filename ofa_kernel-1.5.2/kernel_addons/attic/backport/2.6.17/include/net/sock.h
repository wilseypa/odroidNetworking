#ifndef _NET_SOCK_SLES_BACKPORT_H
#define _NET_SOCK_SLES_BACKPORT_H

#include_next <net/sock.h>

#define sk_eat_skb(a, b, c) sk_eat_skb(a, b)

#endif
