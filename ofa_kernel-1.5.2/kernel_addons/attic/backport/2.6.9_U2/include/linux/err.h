#ifndef BACKPORT_LINUX_ERR_H
#define BACKPORT_LINUX_ERR_H

#include_next <linux/err.h>

#define IS_ERR_VALUE(x) unlikely((x) > (unsigned long)-1000L)

#endif /* BACKPORT_LINUX_ERR_H */
