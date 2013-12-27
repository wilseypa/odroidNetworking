#ifndef _BACKPORT_LINUX_MM_H_
#define _BACKPORT_LINUX_MM_H_

#include_next <linux/mm.h>

#if defined(__i386__)
#include <asm/highmem.h>
#endif

/*
 * Determine if an address is within the vmalloc range
 *
 * On nommu, vmalloc/vfree wrap through kmalloc/kfree directly, so there
 * is no special casing required.
 */
static inline int is_vmalloc_addr(const void *x)
{
#ifdef CONFIG_MMU
	unsigned long addr = (unsigned long)x;

	return addr >= VMALLOC_START && addr < VMALLOC_END;
#else
	return 0;
#endif
}

#endif
