#ifndef LINUX_JIFFIES_BACKPORT_H
#define LINUX_JIFFIES_BACKPORT_H

#include <linux/time.h>
#include <linux/timex.h>

#include_next <linux/jiffies.h>

static inline unsigned long usecs_to_jiffies(const unsigned int u)
{
	return (u * HZ + USEC_PER_SEC - 1) / USEC_PER_SEC;
}

#endif /* LINUX_JIFFIES_BACKPORT_H */
