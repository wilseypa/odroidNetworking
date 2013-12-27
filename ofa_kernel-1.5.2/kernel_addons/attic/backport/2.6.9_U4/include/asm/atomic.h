#ifndef _ASM_ATOMIC_BACKPORT_H
#define _ASM_ATOMIC_BACKPORT_H

#include_next <asm/atomic.h>

#if defined(__powerpc64__)

#include <asm-powerpc/atomic64.h>

#elif defined(__ia64__)

#include <asm-ia64/atomic.h>

#elif defined (__i386__)

#include <asm-x86/atomic_32.h>

#elif defined(__x86_64__)

#include <asm-x86/atomic_64.h>

#endif
#endif /* _ASM_ATOMIC_BACKPORT_H */
