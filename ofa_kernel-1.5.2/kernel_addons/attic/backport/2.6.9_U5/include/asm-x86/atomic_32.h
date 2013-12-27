#ifndef __BACKPORT_ARCH_I386_ATOMIC__
#define __BACLPORT_ARCH_I386_ATOMIC__

#define atomic_cmpxchg(v, old, new) (cmpxchg(&((v)->counter), (old), (new)))

#include <asm-generic/atomic.h>
#endif
