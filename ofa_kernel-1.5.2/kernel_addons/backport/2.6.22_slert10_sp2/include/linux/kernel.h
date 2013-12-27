#ifndef BACKPORT_KERNEL_H_2_6_22
#define BACKPORT_KERNEL_H_2_6_22

#include_next <linux/kernel.h>

#include <asm/errno.h>
#include <asm/string.h>

#define USHORT_MAX	((u16)(~0U))

static inline int strict_strtoul(const char *cp, unsigned int base, unsigned long *res)
{
	char *tail;
	unsigned long val;
	size_t len;

	*res = 0;
	len = strlen(cp);
	if (len == 0)
		return -EINVAL;

	val = simple_strtoul(cp, &tail, base);
	if ((*tail == '\0') ||
		((len == (size_t)(tail - cp) + 1) && (*tail == '\n'))) {
		*res = val;
		return 0;
	}

	return -EINVAL;
}


#endif
