#ifndef BACKPORT_LINUX_FS_H
#define BACKPORT_LINUX_FS_H

#include_next <linux/fs.h>

#define i_private u.generic_ip
#define i_mutex i_sem

static inline void inc_nlink(struct inode *inode)
{
        inode->i_nlink++;
}

#endif
