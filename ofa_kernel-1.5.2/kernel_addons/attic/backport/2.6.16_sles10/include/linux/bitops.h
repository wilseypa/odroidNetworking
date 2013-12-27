#include_next <linux/bitops.h>
#ifndef LINUX_BITOPS_BACKPORT_2_6_16
#define LINUX_BITOPS_BACKPORT_2_6_16

static inline unsigned fls_long(unsigned long l)
{
	if (sizeof(l) == 4)
		return fls(l);
	return fls64(l);
}

#endif
