#ifndef BACKPORT_LINUX_SYSFS_H
#define BACKPORT_LINUX_SYSFS_H

#include_next <linux/sysfs.h>

#define __ATTR_RO(_name) { \
        .attr   = { .name = __stringify(_name), .mode = 0444, .owner = THIS_MODULE },   \
        .show   = _name##_show, \
}

#define __ATTR(_name,_mode,_show,_store) { \
        .attr = {.name = __stringify(_name), .mode = _mode, .owner = THIS_MODULE },     \
        .show   = _show,                                        \
        .store  = _store,                                       \
}

#endif
