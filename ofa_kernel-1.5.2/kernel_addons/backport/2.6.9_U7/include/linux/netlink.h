#ifndef BACKPORT_LINUX_NETLINK_H
#define BACKPORT_LINUX_NETLINK_H

#include_next <linux/netlink.h>

#define __nlmsg_put(skb, daemon_pid, seq, type, len, flags) \
       __nlmsg_put(skb, daemon_pid, 0, 0, len)

#define netlink_kernel_create(net, uint, groups, input, mutex, mod) \
       netlink_kernel_create(uint, input)

#define NETLINK_ISCSI           8

static inline struct nlmsghdr *nlmsg_hdr(const struct sk_buff *skb)
{
	return (struct nlmsghdr *)skb->data;
}

#endif
