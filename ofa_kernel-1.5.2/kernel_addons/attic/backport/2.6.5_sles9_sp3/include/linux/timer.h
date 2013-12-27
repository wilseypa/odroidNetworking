#ifndef LINUX_TIMER_H_BACKPORT
#define LINUX_TIMER_H_BACKPORT

#include <linux/jiffies.h>
#include_next <linux/timer.h>

static inline void setup_timer(struct timer_list * timer,
                               void (*function)(unsigned long),
                                unsigned long data)
{
        timer->function = function;
        timer->data = data;
        init_timer(timer);
}

static inline unsigned long round_jiffies_relative(unsigned long j)
{
	return jiffies + j;
}

static inline unsigned long round_jiffies(unsigned long j)
{
	return j;
}

#endif /* LINUX_TIMER_H_BACKPORT */
