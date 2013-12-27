#ifndef LINUX_SCHED_BACKPORT_H
#define LINUX_SCHED_BACKPORT_H

#include_next <linux/sched.h>

#define TASK_WAKEKILL	   128

#define TASK_KILLABLE	   (TASK_WAKEKILL | TASK_UNINTERRUPTIBLE)

#define schedule_timeout_killable(_arg) schedule_timeout_interruptible(_arg)

static inline pid_t task_pid_nr(struct task_struct *tsk)
{
	return tsk->pid;
}

static inline int
backport_set_cpus_allowed_ptr(struct task_struct *p, const cpumask_t new_mask)
{
	return set_cpus_allowed(p, new_mask);
}
#define set_cpus_allowed_ptr backport_set_cpus_allowed_ptr

#endif
