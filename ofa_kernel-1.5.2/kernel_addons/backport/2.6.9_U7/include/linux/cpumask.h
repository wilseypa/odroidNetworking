#ifndef LINUX_CPUMASK_BACKPORT_H
#define LINUX_CPUMASK_BACKPORT_H

#include_next <linux/cpumask.h>

#define for_each_possible_cpu(cpu)  for_each_cpu(cpu)

#endif /* LINUX_CPUMASK_BACKPORT_H */

