#ifndef _DMA_MAPPING_BACKPORT_H
#define _DMA_MAPPING_BACKPORT_H

#include <linux/device.h>
#include <linux/mm.h>

#include_next <linux/dma-mapping.h>

static inline int valid_dma_direction(int dma_direction)
{
	return ((dma_direction == DMA_BIDIRECTIONAL) ||
		(dma_direction == DMA_TO_DEVICE) ||
		(dma_direction == DMA_FROM_DEVICE));
}

#endif /* _DMA_MAPPING_BACKPORT_H */
