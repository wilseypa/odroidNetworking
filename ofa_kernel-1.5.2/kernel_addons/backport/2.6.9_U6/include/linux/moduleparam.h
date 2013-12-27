#ifndef LINUX_MODULEPARAM_H_BACKPORT_TO_2_6_9
#define LINUX_MODULEPARAM_H_BACKPORT_TO_2_6_9

#include_next <linux/moduleparam.h>

#undef module_param_array_named

/* Treat num as a pointer and pass the address of a dummy variable if num == NULL */
#define module_param_array_named(name, array, type, num, perm)		\
	static unsigned int __arr_backport_to_2_6_9_num##name;		\
	static struct kparam_array __param_arr_##name			\
	= { ARRAY_SIZE(array), num ? num :				\
		&__arr_backport_to_2_6_9_num##name,			\
		param_set_##type, param_get_##type,			\
		sizeof(array[0]), array };				\
	module_param_call(name, param_array_set, param_array_get, 	\
			  &__param_arr_##name, perm);			\

#endif /* LINUX_TIMER_H_BACKPORT */
