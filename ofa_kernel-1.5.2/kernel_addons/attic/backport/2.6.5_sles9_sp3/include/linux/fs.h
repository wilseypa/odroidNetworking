#ifndef BACKPORT_LINUX_FS_H
#define BACKPORT_LINUX_FS_H

#include_next <linux/fs.h>
#include <asm/uaccess.h>

#define i_private u.generic_ip
#define i_mutex i_sem

static inline ssize_t simple_read_from_buffer(void __user *to, size_t count,
					      loff_t *ppos, const void *from,
					      size_t available)
{
	loff_t pos = *ppos;
	if (pos < 0)
		return -EINVAL;
	if (pos >= available)
		return 0;
	if (count > available - pos)
		count = available - pos;
	if (copy_to_user(to, from + pos, count))
		return -EFAULT;
	*ppos = pos + count;
	return count;
}

static inline void inc_nlink(struct inode *inode)
{
        inode->i_nlink++;
}

#endif
