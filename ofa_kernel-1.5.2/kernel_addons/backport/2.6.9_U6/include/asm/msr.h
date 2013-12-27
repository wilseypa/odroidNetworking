#ifndef _ASM_MSR_BACKPORT_H
#define _ASM_MSR_BACKPORT_H

#include_next <asm/msr.h>
#if defined(__x86_64__)
#if !defined(wrmsr_safe)
#define wrmsr_safe(msr,a,b) ({ int ret__;                       \
        asm volatile("2: wrmsr ; xorl %0,%0\n"                  \
                     "1:\n\t"                                   \
                     ".section .fixup,\"ax\"\n\t"               \
                     "3:  movl %4,%0 ; jmp 1b\n\t"              \
                     ".previous\n\t"                            \
                     ".section __ex_table,\"a\"\n"              \
                     "   .align 8\n\t"                          \
                     "   .quad  2b,3b\n\t"                      \
                     ".previous"                                \
                     : "=a" (ret__)                             \
                     : "c" (msr), "0" (a), "d" (b), "i" (-EFAULT)); \
        ret__; })
#endif

#if !defined(rdmsr_safe)
#define rdmsr_safe(msr,a,b) \
        ({ int ret__;                                            \
         asm volatile ("1:       rdmsr\n"                        \
                       "2:\n"                                    \
                       ".section .fixup,\"ax\"\n"                \
                       "3:       movl %4,%0\n"                   \
                       " jmp 2b\n"                               \
                       ".previous\n"                             \
                       ".section __ex_table,\"a\"\n"             \
                       " .align 8\n"                             \
                       " .quad 1b,3b\n"                          \
                       ".previous":"=&bDS" (ret__), "=a"(*(a)), "=d"(*(b))\
                       :"c"(msr), "i"(-EIO), "0"(0));            \
         ret__; })             

#endif

#elif defined(__i386__)
#if !defined(wrmsr_safe)
#define wrmsr_safe(msr,a,b) ({ int ret__;                                               \
        asm volatile("2: wrmsr ; xorl %0,%0\n"                                          \
                     "1:\n\t"                                                           \
                     ".section .fixup,\"ax\"\n\t"                                       \
                     "3:  movl %4,%0 ; jmp 1b\n\t"                                      \
                     ".previous\n\t"                                                    \
                     ".section __ex_table,\"a\"\n"                                      \
                     "   .align 4\n\t"                                                  \
                     "   .long  2b,3b\n\t"                                              \
                     ".previous"                                                        \
                     : "=a" (ret__)                                                     \
                     : "c" (msr), "0" (a), "d" (b), "i" (-EFAULT));\
        ret__; })
#endif

#if !defined(rdmsr_safe)
#define rdmsr_safe(msr,a,b) ({ int ret__;                                               \
        asm volatile("2: rdmsr ; xorl %0,%0\n"                                          \
                     "1:\n\t"                                                           \
                     ".section .fixup,\"ax\"\n\t"                                       \
                     "3:  movl %4,%0 ; jmp 1b\n\t"                                      \
                     ".previous\n\t"                                                    \
                     ".section __ex_table,\"a\"\n"                                      \
                     "   .align 4\n\t"                                                  \
                     "   .long  2b,3b\n\t"                                              \
                     ".previous"                                                        \
                     : "=r" (ret__), "=a" (*(a)), "=d" (*(b))                           \
                     : "c" (msr), "i" (-EFAULT));\
        ret__; })
#endif

#else
#if !defined(wrmsr_safe)
#define wrmsr_safe(msr,a,b)  (-1)
#endif

#if !defined(rdmsr_safe)
#define rdmsr_safe(msr,a,b)  (-1) 
#endif

#endif
#endif
