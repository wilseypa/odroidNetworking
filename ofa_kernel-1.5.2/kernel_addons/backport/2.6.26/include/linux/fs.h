#ifndef __BACKPORT_LINUX_FS_H_TO_2_6_26__
#define __BACKPORT_LINUX_FS_H_TO_2_6_26__

#include_next <linux/fs.h>

#define FILE_LOCK_DEFERRED 1

#define MAY_ACCESS 16
#define MAY_OPEN 32

static inline loff_t 
generic_file_llseek_unlocked(struct file *file, loff_t offset, int origin)
{
	loff_t retval;
	struct inode *inode = file->f_mapping->host;

	switch (origin) {
		case SEEK_END:
			offset += inode->i_size;
			break;
		case SEEK_CUR:
			offset += file->f_pos;
	}
	retval = -EINVAL;
	if (offset>=0 && offset<=inode->i_sb->s_maxbytes) {
		/* Special lock needed here? */
		if (offset != file->f_pos) {
			file->f_pos = offset;
			file->f_version = 0;
		}
		retval = offset;
	}
	return retval;
}
#endif /* __BACKPORT_LINUX_FS_H_TO_2_6_26__ */
