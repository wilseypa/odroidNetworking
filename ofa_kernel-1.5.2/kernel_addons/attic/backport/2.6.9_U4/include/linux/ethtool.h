#ifndef BACKPORT_LINUX_ETHTOOL_TO_2_6_13
#define BACKPORT_LINUX_ETHTOOL_TO_2_6_13

#include_next <linux/ethtool.h>

#define ADVERTISED_Pause		(1 << 13)
#define ADVERTISED_Asym_Pause		(1 << 14)

#endif
