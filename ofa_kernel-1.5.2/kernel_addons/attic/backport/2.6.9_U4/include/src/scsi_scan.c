#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/spinlock.h>

#include <scsi/scsi.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_driver.h>
#include <scsi/scsi_devinfo.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_transport.h>
#include <scsi/scsi_eh.h>

/**
 * int_to_scsilun: reverts an int into a scsi_lun
 * @int:        integer to be reverted
 * @scsilun:    struct scsi_lun to be set.
 *
 * Description:
 *     Reverts the functionality of the scsilun_to_int, which packed
 *     an 8-byte lun value into an int. This routine unpacks the int
 *     back into the lun value.
 *     Note: the scsilun_to_int() routine does not truly handle all
 *     8bytes of the lun value. This functions restores only as much
 *     as was set by the routine.
 *
 * Notes:
 *     Given an integer : 0x0b030a04,  this function returns a
 *     scsi_lun of : struct scsi_lun of: 0a 04 0b 03 00 00 00 00
 *
 **/
void int_to_scsilun(unsigned int lun, struct scsi_lun *scsilun)
{
        int i;

        memset(scsilun->scsi_lun, 0, sizeof(scsilun->scsi_lun));

        for (i = 0; i < sizeof(lun); i += 2) {
                scsilun->scsi_lun[i] = (lun >> 8) & 0xFF;
                scsilun->scsi_lun[i+1] = lun & 0xFF;
                lun = lun >> 16;
        }
}
EXPORT_SYMBOL(int_to_scsilun);
