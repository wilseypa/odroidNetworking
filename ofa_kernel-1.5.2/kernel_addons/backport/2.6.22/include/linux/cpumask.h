#ifndef BACKPORT_LINUX_CPUMASK_H
#define BACKPORT_LINUX_CPUMASK_H

#include_next <linux/cpumask.h>

#define cpumask_of(cpu)			(cpumask_of_cpu(cpu))
#define set_cpus_allowed_ptr(a, b)	(set_cpus_allowed(a, b))
#define cpumask_of_node(node)		(node_to_cpumask(node))

#endif /* BACKPORT_LINUX_CPUMASK_H */
