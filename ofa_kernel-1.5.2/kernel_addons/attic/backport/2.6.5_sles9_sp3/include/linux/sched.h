#ifndef LINUX_SCHED_BACKPORT_H
#define LINUX_SCHED_BACKPORT_H

#include_next <linux/sched.h>

static inline unsigned long
wait_for_completion_timeout(struct completion *x, unsigned long timeout)
{
	might_sleep();

	spin_lock_irq(&x->wait.lock);
	if (!x->done) {
		DECLARE_WAITQUEUE(wait, current);

		wait.flags |= WQ_FLAG_EXCLUSIVE;
		__add_wait_queue_tail(&x->wait, &wait);
		do {
			__set_current_state(TASK_UNINTERRUPTIBLE);
			spin_unlock_irq(&x->wait.lock);
			timeout = schedule_timeout(timeout);
			spin_lock_irq(&x->wait.lock);
			if (!timeout) {
				__remove_wait_queue(&x->wait, &wait);
				goto out;
			}
		} while (!x->done);
		__remove_wait_queue(&x->wait, &wait);
	}
	x->done--;
out:
	spin_unlock_irq(&x->wait.lock);
	return timeout;
}

static inline int xxx_can_do_mlock(void)
{
	return 1;
}

#define can_do_mlock xxx_can_do_mlock

static inline struct mm_struct *xxx_mmgrab(struct mm_struct *mm)
{
	if (!atomic_read(&mm->mm_users))
		mm = NULL;
	else
		atomic_inc(&mm->mm_users);

	return mm;
}


static inline struct mm_struct * xxx_get_task_mm(struct task_struct * task)
{
	struct mm_struct * mm;

	task_lock(task);
	mm = task->mm;
	if (mm)
		mm = xxx_mmgrab(mm);
	task_unlock(task);

	return mm;
}

#define get_task_mm xxx_get_task_mm

#endif
