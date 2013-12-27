#ifndef LINUX_DEVICE_BACKPORT_H
#define LINUX_DEVICE_BACKPORT_H

#include_next <linux/device.h>

#define add_uevent_var add_hotplug_env_var
#define uevent hotplug

static inline const char *dev_name(const struct device *dev)
{
	return dev->bus_id;
}

#endif
