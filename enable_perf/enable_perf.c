/**
 * No copyright whatsoever!
 * Public domain.
 *
 * */

/*Performance counters individual enable*/
#define PMCNTEN_1 1/*bit for first counter in PMCNTEN*/
#define PMCNTEN_2 2/*bit for second counter in PMCNTEN*/
#define PMCNTEN_3 4/*bit for third counter in PMCNTEN*/
#define PMCNTEN_4 8/*bit for fourth counter in PMCNTEN*/
#define PMCNTEN_CYCLE_COUNTER (1<<31) /*bit for cycle counter in PMCNTEN*/

#define PMINTENCLR_ALL 0xFFFFFFFF /*bits to clear all interrupts*/
#define PMCR_E (1) /*bit to enable all counters*/

/* DBGAUTHSTATUS:Authentication Status register */
#define DAS_SNI (1<<7) /*Secure non-invasive debug features implemented*/
#define DAS_SNE (1<<6) /*Secure non-invasive debug features enabled: (DBGEN OR NIDEN) AND (SPIDEN OR SPNIDEN)*/
#define DAS_SI (1<<5) /*Secure invasive debug features implemented*/
#define DAS_SE (1<<4) /*Secure invasive debug features enabled: DBGEN AND SPIDEN*/
#define DAS_NSNI (1<<3) /*Non-secure non-invasive debug features implemented*/
#define DAS_NSNE (1<<2) /*Non-secure non-invasive debug features enabled: DBGEN OR NIDEN*/
#define DAS_NSI (1<<1)  /*Non-secure invasive debug features implemented*/
#define DAS_NSE (1)  /*Non-secure invasive debug features enabled : DBGEN*/

/*Performance monitor counter selection*/
#define PMN0 0
#define PMN1 1
#define PMN2 2
#define PMN3 3

/*events could be counted*/
#define  L1I_CACHE 0x14 /*Level 1 instruction cache access*/

#define MEASURE_PMU_TWICE 0 /*read the PMU counters twice*/
#define DISABLE_PMU_ONEXIT 0 /*disable PMU before exiting*/

#include <linux/init.h>
#include <linux/module.h>
#if MEASURE_PMU_TWICE==1
#include <linux/delay.h>
#endif

/***
 * On current CPU:
 * 		investigates the debug authentication status
 * 		starts performance monitor counting
 */
static void read_pmu_first(void* data){
	/* DBGAUTHSTATUS Authentication Status register Register*/
	uint32_t dbgauthstatus;
	/* PMCR: Performance Monitor Control Register */
	uint32_t pmcr;
	/* PMCNTENSET: Performance Monitors Count Enable Set register */
	uint32_t pmcntenset;
	/* PMINTENCLR: Performance Monitors Interrupt Enable Clear register*/
	uint32_t pminten;
	/* PMCCNTR: cycle counter register*/
	uint32_t pmccntr;
	/* PMSELR: Performance Monitors Event Counter Selection Register*/
	uint32_t pmselr;
	/* PMXEVTYPER: Performance Monitors Event Type Select Register*/
	uint32_t pmxevtyper;
	uint32_t first_counter;
	unsigned int this_cpu = smp_processor_id();

	/* Read DBGAUTHSTATUS
	 * CRn:c7, opc2:6, CRm:c14
	 * MRC p14, 0, <Rt>, c7, c14, 6; Read */
	asm volatile("mrc p14, 0, %0, c7, c14, 6" :"=r"(dbgauthstatus));

	/*Read PMCR, Performance Monitor Control Register*/
	asm volatile("mrc p15, 0, %0, c9, c12, 0" :"=r"(pmcr));

	/* enable the Performance monitor unit */
	asm volatile("mcr p15, 0, %0, c9, c12, 0"::"r"(PMCR_E));
	/* disable interrupt generation on any counter*/
	pminten = PMINTENCLR_ALL;
	asm volatile("mcr p15, 0, %0, c9, c14, 2"::"r"(pminten));

	/*enable first two counters and cycle counter */
	pmcntenset = PMCNTEN_CYCLE_COUNTER | PMCNTEN_1 | PMCNTEN_2;
	asm volatile("mcr p15, 0, %0, c9, c12, 1"::"r"(pmcntenset));
	asm volatile("isb");

	/*read cycle counter*/
	asm volatile("mrc p15, 0, %0, c9, c13, 0" :"=r"(pmccntr) );

	/*start count Level 1 instruction cache access */
	/*	select the first counter, PMSELR */
	pmselr = PMN0;
	asm volatile("mcr p15, 0, %0, c9, c12, 5"::"r"(pmselr));
	asm volatile("isb");
	/*	specify the event the counter should count, PMXEVTYPER*/
	pmxevtyper = L1I_CACHE;
	asm volatile("mcr p15, 0, %0, c9, c13, 1"::"r"(pmxevtyper));

	/*initial read as it is already selected*/
	/*read PMXEVCNTR: */
	asm volatile("mrc p15, 0, %0, c9, c13, 2":"=r"(first_counter));

	/*report:*/
	printk(KERN_INFO "DEBUG Investigation report for CPU %d:\n"
			"\t DBGAUTHSTATUS:0x%08X\n"
				"\t\tSecure non-invasive debug: %simplemented, %senabled\n"
				"\t\tSecure invasive debug: %simplemented, %senabled\n"
				"\t\tNon-secure non-invasive debug: %simplemented, %senabled\n"
				"\t\tNon-secure invasive debug: %simplemented, %senabled\n"
			"\t PMCR: 0x%08X, Cortex %d\n"
			"\t Initial cycle counter read: %d\n"
			"\t Initial first counter read: %d\n"
			,this_cpu,
				dbgauthstatus,
					(dbgauthstatus & DAS_SNI)?"":"not ",
					(dbgauthstatus & DAS_SNE)?"":"not ",
					(dbgauthstatus & DAS_SI)?"":"not ",
					(dbgauthstatus & DAS_SE)?"":"not ",
					(dbgauthstatus & DAS_NSNI)?"":"not ",
					(dbgauthstatus & DAS_NSNE)?"":"not ",
					(dbgauthstatus & DAS_NSI)?"":"not ",
					(dbgauthstatus & DAS_NSE)?"":"not ",
			pmcr,(pmcr>>16)&0xFF,
			pmccntr,
			first_counter
			);
}

#if MEASURE_PMU_TWICE==1
/***
 * On current CPU:
 * 		reads the performance monitor counters
 */
static void read_pmu_again(void* data){
	/* PMCR: Performance Monitor Control Register */
	uint32_t pmcr;
	/* PMCCNTR: cycle counter register*/
	uint32_t pmccntr;
	/*PMSELR: Performance Monitors Event Counter Selection Register*/
	uint32_t pmselr;

	uint32_t first_counter;
	unsigned int this_cpu = smp_processor_id();

	/*Read PMCR, Performance Monitor Control Register*/
	asm volatile("mrc p15, 0, %0, c9, c12, 0" :"=r"(pmcr));
	/*read cycle counter*/
	asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(pmccntr) );
	/*	select the first counter, PMSELR */
	pmselr = PMN0;
	asm volatile("mcr p15, 0, %0, c9, c12, 5"::"r"(pmselr));
	asm volatile("isb");
	/*read selected PMXEVCNTR: */
	asm volatile("mrc p15, 0, %0, c9, c13, 2":"=r"(first_counter));

	/*report:*/
	printk(KERN_INFO "DEBUG Investigation report for counters on CPU %d:\n"
			"\t PMCR: 0x%08X, Cortex %d\n"
			"\t second read of cycle counter: %d\n"
			"\t second read of first counter: %d\n"
			,this_cpu,
			pmcr,(pmcr>>16)&0xFF,
			pmccntr,
			first_counter
			);
}
#endif

#if DISABLE_PMU_ONEXIT==1
/***
 * On current CPU:
 * 		disables the performance monitor unit
 */
static void disable_pmu(void* data){
	/* PMCR: Performance Monitor Control Register */
	uint32_t pmcr;
	/*Read PMCR, Performance Monitor Control Register*/
	asm volatile("mrc p15, 0, %0, c9, c12, 0" :"=r"(pmcr));
	/*clear the enable bit*/
	pmcr = pmcr ^ PMCR_E;
	/* disable the Performance monitor unit */
	asm volatile("mcr p15, 0, %0, c9, c12, 0"::"r"(pmcr));
}
#endif

static int dbginvestigate_init(void){
    printk(KERN_ALERT "Module dbginvestigate inserted\n");
    on_each_cpu(read_pmu_first, NULL, 1);
#if MEASURE_PMU_TWICE==1
    /*read the counters again*/
    msleep(15);
    on_each_cpu(read_pmu_again, NULL, 1);
#endif
    return 0;
}

static void dbginvestigate_exit(void){
#if DISABLE_PMU_ONEXIT==1
    /*disable the enabled units */
    on_each_cpu(disable_pmu, NULL, 1);
#endif
    printk(KERN_ALERT "Module dbginvestigate removed\n");
}

MODULE_LICENSE("Dual BSD/GPL");
module_init(dbginvestigate_init);
module_exit(dbginvestigate_exit);
