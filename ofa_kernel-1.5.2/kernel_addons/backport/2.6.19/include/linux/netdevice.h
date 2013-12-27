#ifndef BACKPORT_LINUX_NETDEVICE_TO_2_6_23
#define BACKPORT_LINUX_NETDEVICE_TO_2_6_23

#include_next <linux/netdevice.h>

#define dev_get_by_name(net, name) dev_get_by_name(name)

#define NETIF_F_LRO		32768   /* large receive offload */
#define NETIF_F_IPV6_CSUM	16	/* Can checksum TCP/UDP over IPV6 */

#define for_each_netdev(a, dev) for ((dev) = dev_base; (dev) != NULL; (dev) = (dev)->next)

#endif
