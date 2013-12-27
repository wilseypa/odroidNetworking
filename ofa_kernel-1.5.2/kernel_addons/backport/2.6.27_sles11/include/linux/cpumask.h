#ifndef BACKPORT_LINUX_CPUMASK_H
#define BACKPORT_LINUX_CPUMASK_H

#include_next <linux/cpumask.h>

#define cpumask_of(cpu)		(get_cpu_mask(cpu))

#endif /* BACKPORT_LINUX_CPUMASK_H */
