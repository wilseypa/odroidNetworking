#ifndef LINUX_STDDEF_H_BACKPORT
#define LINUX_STDDEF_H_BACKPORT

#include_next <linux/stddef.h>

#ifdef __KERNEL__

enum {
	false	= 0,
	true	= 1
};

#endif /* __KERNELL__ */

#endif
