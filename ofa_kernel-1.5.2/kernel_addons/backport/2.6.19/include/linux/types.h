#ifndef BACKPORT_LINUX_TYPES_TO_2_6_19
#define BACKPORT_LINUX_TYPES_TO_2_6_19

#include_next <linux/types.h>

typedef __u16	__sum16;
typedef __u32	__wsum;

#endif
