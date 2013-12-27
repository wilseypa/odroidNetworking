#ifndef BACKPORT_LINUX_FREEZER_H
#define BACKPORT_LINUX_FREEZER_H

#include_next <linux/freezer.h>

static inline void set_freezable(void) {}

#endif
