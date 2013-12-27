#ifndef ASM_HVCALL_BACKPORT_2618_H
#define ASM_HVCALL_BACKPORT_2618_H

#include_next <asm/hvcall.h>

#ifdef __KERNEL__

#define PLPAR_HCALL9_BUFSIZE 9

inline static long plpar_hcall9(unsigned long opcode,
                                unsigned long *retbuf,
                                unsigned long arg1,     /* <R4  */
                                unsigned long arg2,     /* <R5  */
                                unsigned long arg3,     /* <R6  */
                                unsigned long arg4,     /* <R7  */
                                unsigned long arg5,     /* <R8  */
                                unsigned long arg6,     /* <R9  */
                                unsigned long arg7,     /* <R10 */
                                unsigned long arg8,     /* <R11 */
                                unsigned long arg9      /* <R12 */
    )
{
       return plpar_hcall_9arg_9ret(opcode,
                                    arg1, arg2, arg3, arg4, arg5,
                                    arg6, arg7, arg8, arg9,
                                    retbuf, &retbuf[1], &retbuf[2],
                                    &retbuf[3], &retbuf[4], &retbuf[5],
                                    &retbuf[6], &retbuf[7], &retbuf[8]);
}

#endif /* __KERNEL__ */
#endif
