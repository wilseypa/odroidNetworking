#ifndef BACKPORT_ASM_TOPOLOGY_H
#define BACKPORT_ASM_TOPOLOGY_H

#include_next <asm/topology.h>

static inline const cpumask_t *cpumask_of_node(int node)
{
	node_to_cpumask_ptr(cm, node);
	return cm;
}

#endif /* BACKPORT_ASM_TOPOLOGY_H */
