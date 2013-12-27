#ifndef LINUX_CACHE_BACKPORT_H
#define LINUX_CACHE_BACKPORT_H

#include_next <linux/cache.h>

/* read_mostly (introduced in 2.6.13) */
#if !defined(__read_mostly)
#if defined(CONFIG_X86) || defined(CONFIG_SPARC64)
#define __read_mostly __attribute__((__section__(".data.read_mostly")))
#else
#define __read_mostly
#endif
#endif
#endif
