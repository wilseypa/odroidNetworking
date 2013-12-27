#ifndef BACKPORT_LINUX_MII_TO_SLES9SP3
#define BACKPORT_LINUX_MII_TO_SLES9SP3

#include_next <linux/mii.h>

#define BMCR_SPEED1000              0x0040  /* MSB of Speed (1000)         */
#define ADVERTISE_PAUSE_CAP     0x0400  /* Try for pause               */
#define ADVERTISE_PAUSE_ASYM    0x0800  /* Try for asymetric pause     */
#define MII_CTRL1000        0x09        /* 1000BASE-T control          */
#define ADVERTISE_1000FULL      0x0200  /* Advertise 1000BASE-T full duplex */
#define ADVERTISE_1000HALF      0x0100  /* Advertise 1000BASE-T half duplex */

#endif
