#ifndef LINUX_MUTEX_BACKPORT_H
#define LINUX_MUTEX_BACKPORT_H

/* Since 2.6.18 linux/mutex.h pulls in linux/lockdep.h */
#include_next <linux/mutex.h>
#include <linux/lockdep.h>

#define mutex_lock_nested(a, b) mutex_lock(a)

#endif
