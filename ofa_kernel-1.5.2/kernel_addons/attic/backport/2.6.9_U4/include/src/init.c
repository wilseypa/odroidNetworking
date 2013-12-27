#include <linux/init.h>

/**
 *	driver_init - initialize driver model.
 *
 *	Call the driver model init functions to initialize their
 *	subsystems. Called early from init/main.c.
 */

extern int attribute_container_init(void);

void __init driver_init(void)
{
	attribute_container_init();
}
