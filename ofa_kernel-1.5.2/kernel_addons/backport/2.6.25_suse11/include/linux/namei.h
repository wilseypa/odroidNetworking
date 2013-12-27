#ifndef BACKPORT_LINUX_NAMEI_H
#define BACKPORT_LINUX_NAMEI_H

#include_next <linux/namei.h>

static inline int kern_path(const char *name, unsigned int flags, struct path *path)
{
	struct nameidata nd;
	int rc = path_lookup(name, flags, &nd);
	if (!rc)
		*path = nd.path;
	return rc;
}

#endif /* BACKPORT_LINUX_NAMEI_H */
