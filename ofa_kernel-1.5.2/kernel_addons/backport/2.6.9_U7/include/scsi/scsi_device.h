#ifndef _SCSI_SCSI_DEVICE_H_BACKPORT
#define _SCSI_SCSI_DEVICE_H_BACKPORT

#include_next <scsi/scsi_device.h>

#include <linux/device.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <asm/atomic.h>

struct scsi_lun;

extern void int_to_scsilun(unsigned int, struct scsi_lun *);
extern void scsi_target_block(struct device *);
extern void scsi_target_unblock(struct device *);
extern void starget_for_each_device(struct scsi_target *, void *,
		     void (*fn)(struct scsi_device *, void *));
#endif /* _SCSI_SCSI_DEVICE_H_BACKPORT */
