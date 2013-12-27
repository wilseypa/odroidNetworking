#ifndef __BACKPORT_ASM_IA64_ATOMIC_H
#define __BACKPORT_ASM_IA64_ATOMIC_H

#define atomic64_read(v)	((v)->counter)

#define atomic64_add_return(i,v)					\
({									\
	long __ia64_aar_i = (i);					\
	(__builtin_constant_p(i)					\
	 && (   (__ia64_aar_i ==  1) || (__ia64_aar_i ==   4)		\
	     || (__ia64_aar_i ==  8) || (__ia64_aar_i ==  16)		\
	     || (__ia64_aar_i == -1) || (__ia64_aar_i ==  -4)		\
	     || (__ia64_aar_i == -8) || (__ia64_aar_i == -16)))		\
		? ia64_fetch_and_add(__ia64_aar_i, &(v)->counter)	\
		: ia64_atomic64_add(__ia64_aar_i, v);			\
})

#define atomic_cmpxchg(v, old, new) ((int)cmpxchg(&((v)->counter), old, new))

#include <asm-generic/atomic.h>
#endif /* __BACKPORT_ASM_IA64_ATOMIC_H */
