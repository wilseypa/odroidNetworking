/* scatterlist compatibility for pre-2.6.11 kernels */

#ifndef _LINUX_SCATTERLIST_H
#define _LINUX_SCATTERLIST_H

#include <linux/mm.h>
#include <asm/scatterlist.h>

static inline void sg_set_buf(struct scatterlist *sg, const void *buf,
			      unsigned int buflen)
{
	sg->page = virt_to_page(buf);
	sg->offset = offset_in_page(buf);
	sg->length = buflen;
}

static inline void sg_init_one(struct scatterlist *sg,
                               u8 *buf, unsigned int buflen)
{
        memset(sg, 0, sizeof(*sg));

        sg->page = virt_to_page(buf);
        sg->offset = offset_in_page(buf);
        sg->length = buflen;
}

#define sg_page(a) (a)->page
#define sg_init_table(a, b)
#define sg_set_page(a, b) ((a)->page) = (b)

#define for_each_sg(sglist, sg, nr, __i)	\
	for (__i = 0, sg = (sglist); __i < (nr); __i++, sg++)

#endif /* _LINUX_SCATTERLIST_H */
