#ifndef _JIFFIES_BACKPORT_H
#define _JIFFIES_BACKPORT_H

#include_next <linux/jiffies.h>

#define time_in_range_open(a,b,c) \
	(time_after_eq(a,b) && \
	 time_before(a,c))

#endif /* _JIFFIES_BACKPORT_H */
