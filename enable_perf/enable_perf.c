/* This module enables access to Cortex-A9 performance counters from userspace */

#undef __KERNEL__
#define __KERNEL__

#undef MODULE
#define MODULE

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

static int __init enable_perf_init(void)
{
  /* enable user-mode access to the performance counter */
  asm volatile("mcr p15, 0, %0, c9, c14, 0" :: "r"(1));
  /* enable all counters */
  asm volatile("mcr p15, 0, %0, c9, c12, 1" :: "r"(0x8000000f));

  return 0;
}

static void __exit enable_perf_exit(void) {}

module_init(enable_perf_init);
module_exit(enable_perf_exit);
