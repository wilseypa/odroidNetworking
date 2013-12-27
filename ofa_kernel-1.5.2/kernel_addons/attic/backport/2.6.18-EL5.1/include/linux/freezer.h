#ifndef BACKPORT_LINUX_FREEZER_H
#define BACKPORT_LINUX_FREEZER_H
/*
static inline int frozen(struct task_struct *p) { return 0; }
static inline int freezing(struct task_struct *p) { return 0; }
static inline void freeze(struct task_struct *p) { BUG(); }
static inline int thaw_process(struct task_struct *p) { return 1; }
static inline void frozen_process(struct task_struct *p) { BUG(); }

static inline void refrigerator(void) {}
static inline int freeze_processes(void) { BUG(); return 0; }
static inline void thaw_processes(void) {}

static inline int try_to_freeze(void) { return 0; }
*/

static inline void set_freezable(void) {}

#endif
