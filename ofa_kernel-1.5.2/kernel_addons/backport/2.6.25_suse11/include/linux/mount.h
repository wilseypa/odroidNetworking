#ifndef BACKPORT_LINUX_MOUNT_H
#define BACKPORT_LINUX_MOUNT_H

#include_next <linux/mount.h>
#include <linux/fs.h>

static inline int __mnt_is_readonly(struct vfsmount *mnt)
{
	if (mnt->mnt_sb->s_flags & MS_RDONLY)
		return 1;
	return 0;
}

extern int mnt_want_write(struct vfsmount *mnt);
extern void mnt_drop_write(struct vfsmount *mnt);
extern int init_mnt_writers(void);

#endif
