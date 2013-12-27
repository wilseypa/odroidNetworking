#ifndef BACKPORT_LINUX_FS_H
#define BACKPORT_LINUX_FS_H

#include_next <linux/fs.h>

static inline void inc_nlink(struct inode *inode)
{
        inode->i_nlink++;
}

#endif
