#ifndef BACKPORT_LINUX_SPINLOCK_H
#define BACKPORT_LINUX_SPINLOCK_H

#include_next <linux/spinlock.h>
#define DEFINE_SPINLOCK(x) spinlock_t x = SPIN_LOCK_UNLOCKED
#define DEFINE_RWLOCK(x)	rwlock_t x = RW_LOCK_UNLOCKED

#define spin_trylock_irqsave(lock, flags) \
({ \
        local_irq_save(flags); \
        spin_trylock(lock) ? \
        1 : ({local_irq_restore(flags); 0;}); \
})

#define spin_lock_nested(lock, subclass) spin_lock(lock)

#define spin_trylock_irq(lock) \
({ \
	local_irq_disable(); \
	spin_trylock(lock) ? \
	1 : ({ local_irq_enable(); 0;  }); \
})

#define assert_spin_locked(lock)  do { (void)(lock); } while(0)

#endif
