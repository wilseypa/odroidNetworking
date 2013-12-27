#ifndef SCSI_SCSI_HOST_BACKPORT_TO_2_6_26_H
#define SCSI_SCSI_HOST_BACKPORT_TO_2_6_26_H

#include_next <scsi/scsi_host.h>

static inline void *shost_priv(struct Scsi_Host *shost)
{
        return (void *)shost->hostdata;
}

#endif
