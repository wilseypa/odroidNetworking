#ifndef BACKPORT_LINUX_LOG2_TO_2_6_20_H
#define BACKPORT_LINUX_LOG2_TO_2_6_20_H

#include_next <linux/log2.h>

/*
 *  Determine whether some value is a power of two, where zero is
 * *not* considered a power of two.
 */

static inline __attribute__((const))
bool is_power_of_2(unsigned long n)
{
	return (n != 0 && ((n & (n - 1)) == 0));
}

/*
 * round down to nearest power of two
 */
static inline __attribute__((const))
unsigned long __rounddown_pow_of_two(unsigned long n)
{
	return 1UL << (fls_long(n) - 1);
}


/**
 * rounddown_pow_of_two - round the given value down to nearest power of two
 * @n - parameter
 *
 * round the given value down to the nearest power of two
 * - the result is undefined when n == 0
 * - this can be used to initialise global variables from constant data
 */
#define rounddown_pow_of_two(n)                 \
(                                               \
	__builtin_constant_p(n) ? (             \
		(n == 1) ? 0 :                  \
		(1UL << ilog2(n))) :            \
	__rounddown_pow_of_two(n)               \
)

#endif /* _LINUX_BACKPORT_LOG2_H */
