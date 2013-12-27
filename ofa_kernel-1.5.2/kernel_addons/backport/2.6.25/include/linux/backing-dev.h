#ifndef BACKPORT_LINUX_BACK_DEV_H
#define BACKPORT_LINUX_BACK_DEV_H

#include_next <linux/backing-dev.h>

static inline int bdi_register(struct backing_dev_info *bdi, struct device *parent,
				const char *fmt, ...)
{
	return 0;
}

static inline int bdi_register_dev(struct backing_dev_info *bdi, dev_t dev)
{
	return bdi_register(bdi, NULL, "%u:%u", MAJOR(dev), MINOR(dev));
}

static inline void bdi_unregister(struct backing_dev_info *bdi)
{
	return;
}

#endif
