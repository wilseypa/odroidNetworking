#ifndef BACKPORT_LINUX_FS_H
#define BACKPORT_LINUX_FS_H

#include_next <linux/fs.h>
#include <linux/mount.h>

#define FILE_LOCK_DEFERRED -EINPROGRESS
#define FS_RENAME_DOES_D_MOVE   32768   /* FS will handle d_move()
					 * during rename() internally.
					 */


#define ATTR_KILL_PRIV  (1 << 14)

#define vfs_setlease(a, b, c) setlease(a, b, c)

static inline int __mandatory_lock(struct inode *ino)
{
	return (ino->i_mode & (S_ISGID | S_IXGRP)) == S_ISGID;
}

#define mandatory_lock(_args) MANDATORY_LOCK(_args)

static inline int backport_vfs_symlink(struct inode *dir, struct dentry *dentry, const char *oldname)
{
	return vfs_symlink(dir, dentry, oldname, 0);
}

#define vfs_symlink(_dir, _dentry, _oldname) backport_vfs_symlink(_dir, _dentry, _oldname)

#ifdef CONFIG_DEBUG_WRITECOUNT
static inline void file_take_write(struct file *f)
{
	WARN_ON(f->f_mnt_write_state != 0);
	f->f_mnt_write_state = FILE_MNT_WRITE_TAKEN;
}
#else
static inline void file_take_write(struct file *filp) {}
#endif

static inline int inode_permission(struct inode *inode, int flags)
{
	return permission(inode, flags, NULL);
}

static inline int __mnt_is_readonly(struct vfsmount *mnt)
{
	if (mnt->mnt_sb->s_flags & MS_RDONLY)
		return 1;
	return 0;
}

static inline void drop_nlink(struct inode *inode)
{
	inode->i_nlink--;
}

static inline void clear_nlink(struct inode *inode)
{
	inode->i_nlink = 0;
}

static inline int vfs_test_lock(struct file *filp, struct file_lock *fl)
{
	if (filp->f_op && filp->f_op->lock)
		return filp->f_op->lock(filp, F_GETLK, fl);
	posix_test_lock(filp, fl, fl);
	return 0;
}

static inline int vfs_lock_file(struct file *filp, unsigned int cmd, struct file_lock *fl, struct file_lock *conf)
{
	if (filp->f_op && filp->f_op->lock)
		return filp->f_op->lock(filp, cmd, fl);
	else
		return  posix_lock_file_conf(filp, fl, conf);
}

static inline int vfs_cancel_lock(struct file *filp, struct file_lock *fl)
{
	if (filp->f_op && filp->f_op->lock)
		return filp->f_op->lock(filp, F_CANCELLK, fl);
	return 0;
}

#endif
