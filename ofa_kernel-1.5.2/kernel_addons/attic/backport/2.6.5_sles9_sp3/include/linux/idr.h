
#ifndef __IB_IDR_H_BACKPORT_
#define __IB_IDR_H_BACKPORT_

#include <linux/slab.h>
#include_next <linux/idr.h>

#define idr_get_new_above_int(idp, ptr, starting_id) ib_idr_get_new_above_int(idp, ptr, starting_id)
#define idr_get_new_above(idp, ptr, starting_id, id) ib_idr_get_new_above(idp, ptr, starting_id, id)

int ib_idr_get_new_above_int(struct idr *idp, void *ptr, int starting_id);
int ib_idr_get_new_above(struct idr *idp, void *ptr, int starting_id, int *id);

static inline void idr_destroy(struct idr *idp) {}

#define idr_get_new( idp, ptr, id) \
({ \
        int rv; \
 \
        rv = ib_idr_get_new_above_int(idp, ptr, 0); \
        if (rv < 0) { \
                if (rv == -1) \
                        rv = -EAGAIN; \
                else /* Will be -3 */ \
                        rv = -ENOSPC; \
        } \
        else { \
               *(id) = rv; \
               rv = 0; \
       } \
        rv; \
})


#endif /* __IDR_H_BACKPORT_ */
