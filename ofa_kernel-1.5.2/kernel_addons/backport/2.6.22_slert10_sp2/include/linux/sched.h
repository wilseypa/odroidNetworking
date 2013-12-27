#ifndef LINUX_SCHED_BACKPORT_H
#define LINUX_SCHED_BACKPORT_H

#include_next <linux/sched.h>

#define TASK_WAKEKILL	   128

#define TASK_KILLABLE	   (TASK_WAKEKILL | TASK_UNINTERRUPTIBLE)

#define schedule_timeout_killable(_arg) schedule_timeout_interruptible(_arg)

static inline int __fatal_signal_pending(struct task_struct *tsk)
{
	return sigismember(&tsk->pending.signal, SIGKILL);
}

static inline int fatal_signal_pending(struct task_struct *p)
{
	return signal_pending(p) && __fatal_signal_pending(p);
}

static inline pid_t task_pid_nr(struct task_struct *tsk)
{
	return tsk->pid;
}
#endif
