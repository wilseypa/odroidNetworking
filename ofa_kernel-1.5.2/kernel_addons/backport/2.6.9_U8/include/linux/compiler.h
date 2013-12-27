#ifndef BACKPORT_LINUX_COMPILER_H
#define BACKPORT_LINUX_COMPILER_H

#include_next <linux/compiler.h>

#define __nocast

#endif
#ifndef BACKPORT_LINUX_COMPILER_TO_2_6_22_H
#define BACKPORT_LINUX_COMPILER_TO_2_6_22_H

#include_next <linux/compiler.h>

#define uninitialized_var(x) x = x

#endif
