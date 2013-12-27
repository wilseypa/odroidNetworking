#ifndef LINUX_PCI_IDS_BACKPORT_TO_2_6_5_H
#define LINUX_PCI_IDS_BACKPORT_TO_2_6_5_H

#include_next <linux/pci_ids.h>

/* according to linux/pci_ids.h kernel 2.6.16 */
#define PCI_VENDOR_ID_MELLANOX          0x15b3
#define PCI_DEVICE_ID_MELLANOX_TAVOR    0x5a44
#define PCI_DEVICE_ID_MELLANOX_ARBEL_COMPAT 0x6278
#define PCI_DEVICE_ID_MELLANOX_ARBEL    0x6282
#define PCI_DEVICE_ID_MELLANOX_SINAI_OLD 0x5e8c
#define PCI_DEVICE_ID_MELLANOX_SINAI    0x6274
#define PCI_VENDOR_ID_TOPSPIN           0x1867

#endif
