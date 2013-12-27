#ifndef BACKPORT_LINUX_TYPES_H
#define BACKPORT_LINUX_TYPES_H

#include_next <linux/types.h>

typedef unsigned __bitwise__ fmode_t;

#endif
