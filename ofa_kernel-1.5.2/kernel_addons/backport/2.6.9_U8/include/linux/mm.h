#ifndef _BACKPORT_LINUX_MM_H_
#define _BACKPORT_LINUX_MM_H_

#include_next <linux/mm.h>

#if defined(__i386__)
#include <asm/atomic_kmap.h>
#include <asm/fixmap.h>
#endif

static inline int 
remap_pfn_range(struct vm_area_struct *vma, unsigned long addr,
		unsigned long pfn, unsigned long size, pgprot_t prot)
{
	return remap_page_range(vma, addr, pfn << PAGE_SHIFT, size, prot);
}

static inline int 
io_remap_pfn_range(struct vm_area_struct *vma, unsigned long addr,
		unsigned long pfn, unsigned long size, pgprot_t prot)
{
	return io_remap_page_range(vma, addr, pfn << PAGE_SHIFT, size, prot);
}

/*
	See commits
		41d78ba55037468e6c86c53e3076d1a74841de39
		d98c7a09843621f1b145ca5ae8ed03ff04085edb
	for an explanation.
*/

static inline int backport_set_page_dirty_lock(struct page *page)
{
	if (!PageCompound(page))
		return set_page_dirty_lock(page);

	return 0;
}

#define set_page_dirty_lock backport_set_page_dirty_lock

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
