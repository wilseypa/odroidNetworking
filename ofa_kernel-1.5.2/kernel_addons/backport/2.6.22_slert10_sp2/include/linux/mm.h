#ifndef _BACKPORT_LINUX_MM_H_
#define _BACKPORT_LINUX_MM_H_

#include_next <linux/mm.h>

#if defined(__i386__)
#include <asm/highmem.h>
#endif

#define VM_FAULT_LOCKED  0x0200		/* ->fault locked the returned page */
#define VM_CAN_NONLINEAR 0x08000000     /* Has ->fault & does nonlinear pages */

#define is_vmalloc_addr(x) ((unsigned long)(x) >= VMALLOC_START && (unsigned long)(x) < VMALLOC_END)

struct shrinker {
	shrinker_t		shrink;
	struct list_head	list;
	int			seeks;  /* seeks to recreate an obj */
	long			nr;     /* objs pending delete */
};

#endif
