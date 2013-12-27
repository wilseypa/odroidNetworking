#ifndef LINUX_TYPES_BACKPORT_H
#define LINUX_TYPES_BACKPORT_H

#include_next <linux/types.h>

#define BITS_PER_BYTE 8
#define __bitwise__

typedef __u16	__sum16;
typedef __u32	__wsum;

#endif
#ifndef LINUX_TYPES_BAcKPORT_TO_2_6_18_H
#define LINUX_TYPES_BAcKPORT_TO_2_6_18_H

#include_next <linux/types.h>

typedef _Bool bool;

#endif
