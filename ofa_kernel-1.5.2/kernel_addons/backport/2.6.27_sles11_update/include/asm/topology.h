#ifndef BACKPORT_ASM_TOPOLOGY_H
#define BACKPORT_ASM_TOPOLOGY_H

#include_next <asm/topology.h>

static inline const struct cpumask *cpumask_of_node(int node)
{
		node_to_cpumask_ptr(mask, node);
		return (struct cpumask *)mask;
}

#endif /* BACKPORT_ASM_TOPOLOGY_H */
