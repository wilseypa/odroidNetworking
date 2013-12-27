#ifndef _CPUMASK_BACKPORT_H
#define _CPUMASK_BACKPORT_H

#include_next <linux/cpumask.h>

#define for_each_possible_cpu(cpu)  for_each_cpu(cpu)

#ifdef CONFIG_SMP
# if NR_CPUS > BITS_PER_LONG
#  define cpus_full(map) bitmap_full(map.mask, NR_CPUS)
# else
#  define cpus_full(map) (map == CPU_MASK_ALL)
# endif
#else
# define cpus_full(map) (map == 1UL)
#endif

#endif /* _CPUMASK_BACKPORT_H */
