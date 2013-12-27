#ifndef BACKPORT_LINUX_NET_H
#define BACKPORT_LINUX_NET_H

#include_next <linux/net.h>
#include <linux/random.h>

static inline
int kernel_getsockname(struct socket *sock, struct sockaddr *addr,
			 int *addrlen)
{
	return sock->ops->getname(sock, addr, addrlen, 0);
}

static inline
int kernel_getpeername(struct socket *sock, struct sockaddr *addr,
			 int *addrlen)
{
	return sock->ops->getname(sock, addr, addrlen, 1);
}

#endif
