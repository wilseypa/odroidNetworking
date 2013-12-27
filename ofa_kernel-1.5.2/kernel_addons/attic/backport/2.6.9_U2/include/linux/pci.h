#ifndef LINUX_PCI_BACKPORT_TO_2_6_9_H
#define LINUX_PCI_BACKPORT_TO_2_6_9_H

#include_next <linux/pci.h>

#define PCI_EXP_LNKCTL          16      /* Link Control */
#define PCI_EXP_LNKSTA          18      /* Link Status */
#define  PCI_CAP_ID_HT          0x08    /* HyperTransport */

/*
 * pci_find_next_capability - Find next capability after current position
 * @dev: PCI device to query
 * @pos: Position to search from
 * @cap: capability code
 */
static inline int pci_find_next_capability(struct pci_dev *dev, u8 pos, int cap)
{
	u8 id;
	int ttl = 48;
	int ret;

	while (ttl--) {
		pci_read_config_byte(dev, pos + PCI_CAP_LIST_NEXT, &pos);
		pos &= ~3;
		if (pos < 0x40)
			break;
		pci_read_config_byte(dev, pos + PCI_CAP_LIST_ID, &id);
		if (id == 0xff)
			break;
		if (id == cap) {
			ret = pos;
			goto bail;
		}
	}

	ret = 0;

bail:
	return ret;
}

#endif
#ifndef __BACKPORT_LINUX_PCI_TO_2_6_19__
#define __BACKPORT_LINUX_PCI_TO_2_6_19__

#include_next <linux/pci.h>

/**
 * PCI_VDEVICE - macro used to describe a specific pci device in short form
 * @vend: the vendor name
 * @dev: the 16 bit PCI Device ID
 *
 * This macro is used to create a struct pci_device_id that matches a
 * specific PCI device.  The subvendor, and subdevice fields will be set
 * to PCI_ANY_ID. The macro allows the next field to follow as the device
 * private data.
 */

#define PCI_VDEVICE(vendor, device)            \
	PCI_VENDOR_ID_##vendor, (device),       \
	PCI_ANY_ID, PCI_ANY_ID, 0, 0

static inline struct pci_dev *pci_get_class(unsigned int class,
					    struct pci_dev *from)
{
	struct pci_dev *dev;

	dev = pci_find_class(class, from);
	if (!dev)
		return NULL;

	return pci_dev_get(dev);
}

#endif
