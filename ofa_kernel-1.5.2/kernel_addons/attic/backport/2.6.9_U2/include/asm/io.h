#ifndef _ASM_IO_BACKPORT_H
#define _ASM_IO_BACKPORT_H

#include_next <asm/io.h>

#ifndef mmiowb
#define mmiowb()
#endif

#endif /* _ASM_IO_BACKPORT_H */
