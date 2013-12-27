#ifndef _ASM_BITOPS_IA64_BACKPORT_H
#define _ASM_BITOPS_IA64_BACKPORT_H

#include_next <asm/bitops.h>
#if defined(__ia64__)

/* fls was fixed on IA64 in 2.6.12 */

/*
 * Find the last (most significant) bit set.  Returns 0 for x==0 and
 * bits are numbered from 1..32 (e.g., fls(9) == 4).
 */
static inline int
fls_new (int t)
{
        unsigned long x = t & 0xffffffffu;

        if (!x)
                return 0;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return ia64_popcnt(x);
}

#define fls fls_new

#endif
#endif
