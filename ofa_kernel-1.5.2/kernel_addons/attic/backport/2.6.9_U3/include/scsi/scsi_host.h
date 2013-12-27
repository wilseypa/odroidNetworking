#ifndef _SCSI_SCSI_HOST_H_BACKPORT
#define _SCSI_SCSI_HOST_H_BACKPORT

#include_next <scsi/scsi_host.h>

#define scsi_queue_work(shost, work) schedule_work(work)

#endif
