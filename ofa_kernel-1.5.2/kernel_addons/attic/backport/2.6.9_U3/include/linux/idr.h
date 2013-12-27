#ifndef LINUX_IDR_BACKPORT_TO_2_6_13_H
#define LINUX_IDR_BACKPORT_TO_2_6_13_H

#include_next <linux/idr.h>

/* TODO: provide a real implementation */
static inline void idr_destroy(struct idr *idp) {}


#endif
