#ifndef __ATOMIC_64_INC_BACKPORT_2_6_9__
#define __ATOMIC_64_INC_BACKPORT_2_6_9__

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

#define atomic_cmpxchg(v, old, new) ((int)cmpxchg(&((v)->counter), old, new))

#include <asm-generic/atomic.h>
#endif
