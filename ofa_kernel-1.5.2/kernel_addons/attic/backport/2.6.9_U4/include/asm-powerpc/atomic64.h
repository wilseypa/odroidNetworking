#ifndef __ATOMIC64_INC_BACKPORT_2_6_9__
#define __ATOMIC64_INC_BACKPORT_2_6_9__

typedef struct { volatile long counter; } atomic64_t;

#define ATOMIC64_INIT(i)        { (i) }

#define atomic_cmpxchg(v, o, n) (cmpxchg(&((v)->counter), (o), (n)))
#define atomic64_read(v)        ((v)->counter)
#define atomic64_set(v,i)       (((v)->counter) = (i))

static __inline__ void atomic64_add(long a, atomic64_t *v)
{
  long t;

  __asm__ __volatile__(
"1:	ldarx   %0,0,%3         # atomic64_add\n\
	add     %0,%2,%0\n\
	stdcx.  %0,0,%3 \n\
	bne-    1b"
: "=&r" (t), "=m" (v->counter)
: "r" (a), "r" (&v->counter), "m" (v->counter)
: "cc");
}

static __inline__ long atomic64_add_return(long a, atomic64_t *v)
{
  long t;

  __asm__ __volatile__(
#ifdef CONFIG_SMP
"	lwsync\n"
#endif
"1:	ldarx   %0,0,%2         # atomic64_add_return\n\
	add     %0,%1,%0\n\
	stdcx.  %0,0,%2 \n\
	bne-    1b"
#ifdef CONFIG_SMP
"	isync\n"
#endif
	: "=&r" (t)
	: "r" (a), "r" (&v->counter)
	: "cc", "memory");

  return t;
}

static __inline__ void atomic64_inc(atomic64_t *v)
{
  long t;

  __asm__ __volatile__(
"1:	ldarx   %0,0,%2         # atomic64_inc\n\
	addic   %0,%0,1\n\
	stdcx.  %0,0,%2 \n\
	bne-    1b"
: "=&r" (t), "=m" (v->counter)
: "r" (&v->counter), "m" (v->counter)
: "cc");
}

static __inline__ void atomic64_dec(atomic64_t *v)
{
	long t;

	__asm__ __volatile__(
"1:	ldarx	%0,0,%2		# atomic64_dec\n\
	addic	%0,%0,-1\n\
	stdcx.	%0,0,%2\n\
	bne-	1b"
	: "=&r" (t), "+m" (v->counter)
	: "r" (&v->counter)
	: "cc");
}

#include <asm-generic/atomic.h>

#endif
