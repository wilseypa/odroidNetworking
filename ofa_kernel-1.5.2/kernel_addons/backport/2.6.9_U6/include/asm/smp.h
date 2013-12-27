#ifndef __BACKPORT_2_6_9_SMP_H__
#define __BACKPORT_2_6_9_SMP_H__
#include_next <asm/smp.h>

#define raw_smp_processor_id()  (get_paca()->paca_index)

#endif
