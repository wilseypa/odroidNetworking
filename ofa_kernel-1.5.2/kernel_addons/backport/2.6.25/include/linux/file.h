#ifndef BACKPORT_LINUX_FILE_H
#define BACKPORT_LINUX_FILE_H

#include_next <linux/file.h>
#include <linux/fs.h>

static inline void drop_file_write_access(struct file *filp)
{
	put_write_access(filp->f_path.dentry->d_inode);
}

#endif
