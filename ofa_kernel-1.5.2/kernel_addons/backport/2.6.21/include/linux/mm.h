#ifndef _BACKPORT_LINUX_MM_H_
#define _BACKPORT_LINUX_MM_H_

#include_next <linux/mm.h>

#if defined(__i386__)
#include <asm/highmem.h>
#endif

#define is_vmalloc_addr(x) ((unsigned long)(x) >= VMALLOC_START && (unsigned long)(x) < VMALLOC_END)

#endif
