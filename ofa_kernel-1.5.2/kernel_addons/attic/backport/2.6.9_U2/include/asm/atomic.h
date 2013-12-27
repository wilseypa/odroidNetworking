#ifndef _ASM_ATOMIC_BACKPORT_H
#define _ASM_ATOMIC_BACKPORT_H

#include_next <asm/atomic.h>

# if defined(__x86_64__)

/**
 * atomic64_add_return - add and return
 * @i: integer value to add
 * @v: pointer to type atomic64_t
 *
 * Atomically adds @i to @v and returns @i + @v
 */
static __inline__ long atomic64_add_return(long i, atomic64_t *v)
{
	long __i = i;
	__asm__ __volatile__(
		LOCK "xaddq %0, %1;"
		:"=r"(i)
		:"m"(v->counter), "0"(i));
	return i + __i;
}

# endif /* __x86_64__ */

#define atomic_cmpxchg(v, old, new) ((int)cmpxchg(&((v)->counter), old, new))

#endif
