#ifndef BACKPORT_LINUX_SPINLOCK_H
#define BACKPORT_LINUX_SPINLOCK_H

#include_next <linux/spinlock.h>
#define DEFINE_SPINLOCK(x) spinlock_t x = SPIN_LOCK_UNLOCKED

#ifdef DEFINE_RWLOCK
#undef DEFINE_RWLOCK
#endif

#define DEFINE_RWLOCK(x)	rwlock_t x = RW_LOCK_UNLOCKED

#define spin_lock_nested(lock, subclass) spin_lock(lock)

#define assert_spin_locked(lock)  do { (void)(lock); } while(0)

#endif
