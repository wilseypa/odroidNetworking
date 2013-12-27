#ifndef BACKPORT_LINUX_FCNTL_H
#define BACKPORT_LINUX_FCNTL_H

#include_next <linux/fcntl.h>

#define F_CANCELLK      (F_LINUX_SPECIFIC_BASE+5)

#endif
