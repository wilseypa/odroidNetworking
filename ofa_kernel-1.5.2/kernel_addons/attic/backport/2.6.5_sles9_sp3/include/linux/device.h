#ifndef LINUX_DEVICE_BACKPORT_H
#define LINUX_DEVICE_BACKPORT_H

#include_next <linux/device.h>
#include <linux/slab.h>
#include <linux/err.h>

#define add_uevent_var add_hotplug_env_var
#define uevent hotplug

/**
 * The following 5 static functions were added in kernel 2.6.13.
 * They are added here as static functions, with one change --
 * in function create_class(), there is no owner field in
 * struct class in this kernel release, so the "owner" parameter
 * in the function is unused.
 * Also, in function class_device_create, struct class_device has
 * no devt field, so this also has been commented out.
 * Finally, in kernel 2.6.13, there is also a function
 * class_device_destroy().  This function is not used here, since
 * it depends entirely on the presence of the devt field in the
 * struct class_device.  Instead, class_device_unregister is
 * called directly.
 */

static void class_create_release(struct class *cls)
{
	kfree(cls);
}

static void class_device_create_release(struct class_device *class_dev)
{
	kfree(class_dev);
}

/**
 * class_create - create a struct class structure
 * @owner: pointer to the module that is to "own" this struct class
 * @name: pointer to a string for the name of this class.
 *
 * This is used to create a struct class pointer that can then be used
 * in calls to class_device_create().
 *
 * Note, the pointer created here is to be destroyed when finished by
 * making a call to class_destroy().
 */
static struct class *class_create(struct module *owner, char *name)
{
	struct class *cls;
	int retval;

	cls = kzalloc(sizeof(*cls), GFP_KERNEL);
	if (!cls) {
		retval = -ENOMEM;
		goto error;
	}

	cls->name = name;
	/* cls->owner = owner; */
	cls->class_release = class_create_release;
	cls->release = class_device_create_release;

	retval = class_register(cls);
	if (retval)
		goto error;

	return cls;

error:
	kfree(cls);
	return ERR_PTR(retval);
}

/**
 * class_destroy - destroys a struct class structure
 * @cs: pointer to the struct class that is to be destroyed
 *
 * Note, the pointer to be destroyed must have been created with a call
 * to class_create().
 */
static void class_destroy(struct class *cls)
{
	if ((cls == NULL) || (IS_ERR(cls)))
		return;

	class_unregister(cls);
}

/**
 * class_device_create - creates a class device and registers it with sysfs
 * @cs: pointer to the struct class that this device should be registered to.
 * @dev: the dev_t for the char device to be added.
 * @device: a pointer to a struct device that is assiociated with this class device.
 * @fmt: string for the class device's name
 *
 * This function can be used by char device classes.  A struct
 * class_device will be created in sysfs, registered to the specified
 * class.  A "dev" file will be created, showing the dev_t for the
 * device.  The pointer to the struct class_device will be returned from
 * the call.  Any further sysfs files that might be required can be
 * created using this pointer.
 *
 * Note: the struct class passed to this function must have previously
 * been created with a call to class_create().
 */
static struct class_device *class_device_create(struct class *cls, void *parent, dev_t devt,
					 struct device *device, char *fmt, ...)
{
	va_list args;
	struct class_device *class_dev = NULL;
	int retval = -ENODEV;

	if (cls == NULL || IS_ERR(cls))
		goto error;

	class_dev = kzalloc(sizeof(*class_dev), GFP_KERNEL);
	if (!class_dev) {
		retval = -ENOMEM;
		goto error;
	}

	/* class_dev->devt = devt; */
	class_dev->dev = device;
	class_dev->class = cls;

	va_start(args, fmt);
	vsnprintf(class_dev->class_id, BUS_ID_SIZE, fmt, args);
	va_end(args);
	retval = class_device_register(class_dev);
	if (retval)
		goto error;

	return class_dev;

error:
	kfree(class_dev);
	return ERR_PTR(retval);
}

#endif
