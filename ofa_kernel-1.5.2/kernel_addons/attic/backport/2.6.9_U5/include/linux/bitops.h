#include_next <linux/bitops.h>
#ifndef LINUX_BITOPS_BACKPORT_2_6_15
#define LINUX_BITOPS_BACKPORT_2_6_15

static inline int fls64(__u64 x)
{
	__u32 h = x >> 32;
	if (h)
		return fls(h) + 32;
	return fls(x);
}

#endif
#ifndef LINUX_BITOPS_BACKPORT_2_6_16
#define LINUX_BITOPS_BACKPORT_2_6_16

static inline unsigned fls_long(unsigned long l)
{
	if (sizeof(l) == 4)
		return fls(l);
	return fls64(l);
}

#endif
