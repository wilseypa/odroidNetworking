#include "rxe_timestamp.h"

spinlock_t ts_lock;
struct ts *ts_list;
struct timespec ts_start;

char *get_file(char *path)
{
	char *p;
	static char *q;

	q = NULL;

	for (p = path; *p; p++)
		if (*p == '/')
			q = p + 1;

	return q ? q : path;
}

void timestamp(struct ts *t, int num, char *file, char *func, int line)
{
	unsigned long flags;
	struct timespec ts;

	spin_lock_irqsave(&ts_lock, flags);

	if (!t->init) {
		snprintf(t->file, sizeof(t->file), "%s", get_file(file));
		snprintf(t->func, sizeof(t->func), "%s", func);
		t->line = line;
		t->next = ts_list;
		ts_list = t;
		t->init = 1;
	}

	getnstimeofday(&ts);
	t->num[t->i] = num;
	t->cpu[t->i] = smp_processor_id();
	t->ts[t->i++] = ts;
	t->i = t->i % TS_SIZE;

	spin_unlock_irqrestore(&ts_lock, flags);
}

void ts_init()
{
	getnstimeofday(&ts_start);
	spin_lock_init(&ts_lock);
}

void ts_dump_list()
{
	struct ts *t;
	int i;

	printk(KERN_INFO "timestamps\n");

	for (t = ts_list; t; t = t->next) {
		for (i = 0; i < TS_SIZE; i++) {
			printk(KERN_INFO "%s(%s:%d) %d %d %ld.%09ld\n",
				t->func, t->file, t->line,
				t->num[i], t->cpu[i], t->ts[i].tv_sec,
				t->ts[i].tv_nsec);
		}
	}
}
