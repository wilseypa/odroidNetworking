#ifndef _ARM_PMU_H
#define _ARM_PMU_H

#ifdef __KERNEL__ /* Kernel space ============================================= */

#include <linux/types.h>
#include <linux/cpufreq.h>
#include <linux/smp.h>
#define cycles_to_nanoseconds(x) ({ \
    unsigned long long nanosec; \
    nanosec = (1000000*x)/cpufreq_get(get_cpu()); \
    put_cpu(); \
    nanosec; \
})

#else /* User space =========================================================== */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PATH_MAX 128
#define PAGE_SIZE 4096

unsigned int cpufreq_get(unsigned int cpu)
{
    char filename[PATH_MAX];
    char value[PAGE_SIZE];
    unsigned int ret;
    int fd;
    int len;

    snprintf(filename, PATH_MAX, "/sys/devices/system/cpu/cpu%u/cpufreq/cpuinfo_cur_freq", cpu);

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("Cannot open file: %s, Error %d\n", filename, errno);
        return 0;
    }

    len = read(fd, value, sizeof(value));
    if (len <= 0) {
        printf("Error reading from file: %s\n", filename);
        goto done;
    }

    ret = atoi(value);

done:
    close(fd);
    return ret;
}

unsigned long long cycles_to_nanoseconds(unsigned long long cycles)
{
    unsigned long long freq = cpufreq_get(sched_getcpu());
    if (!freq)
        return 0;
    else
        return (1000*1000*cycles)/freq;
}

#endif

/* Used in both kernel and user space ========================================= */

static uint32_t last_time = 0;
static uint32_t r = 0;

static inline uint32_t rdtsc_now(void)
{
    last_time = r;
#if defined(__GNUC__) && defined(__ARM_ARCH_7A__)
    asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(r) );
    return r;
#else
#error Unsupported architecture/compiler!
#endif
}

static inline uint32_t rdtsc_last(void)
{
    return last_time;
}

#define cycles_to_seconds(x)      cycles_to_nanoseconds(x)/1000000000
#define cycles_to_milliseconds(x) cycles_to_nanoseconds(x)/1000000
#define cycles_to_microseconds(x) cycles_to_nanoseconds(x)/1000

#endif

