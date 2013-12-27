#ifndef BACKPORT_KERNEL_H_2_6_22
#define BACKPORT_KERNEL_H_2_6_22

#include_next <linux/kernel.h>

#define upper_32_bits(n) ((u32)(((n) >> 16) >> 16))

#endif
#ifndef BACKPORT_KERNEL_H_2_6_19
#define BACKPORT_KERNEL_H_2_6_19

#include <linux/log2.h>

#endif
