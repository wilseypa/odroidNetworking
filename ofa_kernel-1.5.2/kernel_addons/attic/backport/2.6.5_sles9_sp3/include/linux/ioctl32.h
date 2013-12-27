#ifndef LINUX_IOCTL32_BACKPORT_H
#define LINUX_IOCTL32_BACKPORT_H

#include_next <linux/ioctl32.h>

#ifndef CONFIG_COMPAT
#define register_ioctl32_conversion(cmd, handler)       ({ 0; })
#define unregister_ioctl32_conversion(cmd)              ({ 0; })
#endif

#endif
