#ifndef BACKPORT_KERNEL_H_2_6_18
#define BACKPORT_KERNEL_H_2_6_18

#include_next <linux/kernel.h>
#include <linux/log2.h>

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#define NIP6_FMT "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x"
#define NIPQUAD_FMT "%u.%u.%u.%u"

#endif
#ifndef BACKPORT_KERNEL_H_2_6_17
#define BACKPORT_KERNEL_H_2_6_17

#define roundup(x, y) ((((x) + ((y) - 1)) / (y)) * (y))

#endif
