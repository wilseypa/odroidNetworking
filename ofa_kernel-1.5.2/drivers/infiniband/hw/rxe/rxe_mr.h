/*
 * Copyright (c) 2009,2010 Mellanox Technologies Ltd. All rights reserved.
 * Copyright (c) 2009,2010 System Fabric Works, Inc. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *	- Redistributions of source code must retain the above
 *	  copyright notice, this list of conditions and the following
 *	  disclaimer.
 *
 *	- Redistributions in binary form must reproduce the above
 *	  copyright notice, this list of conditions and the following
 *	  disclaimer in the documentation and/or other materials
 *	  provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef RXE_MR_H
#define RXE_MR_H

enum copy_direction {
	direction_in,
	direction_out,
};

int rxe_mem_init_dma(struct rxe_dev *rxe, struct rxe_pd *pd,
		     int access, struct rxe_mem *mem);

int rxe_mem_init_phys(struct rxe_dev *rxe, struct rxe_pd *pd,
		      int access, u64 iova, struct ib_phys_buf *buf,
		      int num_buf, struct rxe_mem *mem);

int rxe_mem_init_user(struct rxe_dev *rxe, struct rxe_pd *pd, u64 start,
		      u64 length, u64 iova, int access, struct ib_udata *udata,
		      struct rxe_mem *mr);

int rxe_mem_init_fast(struct rxe_dev *rxe, struct rxe_pd *pd,
		      int max_pages, struct rxe_mem *mem);

int rxe_mem_init_mw(struct rxe_dev *rxe, struct rxe_pd *pd,
		    struct rxe_mem *mw);

int rxe_mem_init_fmr(struct rxe_dev *rxe, struct rxe_pd *pd, int access,
		     struct ib_fmr_attr *attr, struct rxe_mem *fmr);

int rxe_mem_copy(struct rxe_mem *mem, u64 iova, void *addr,
		 int length, enum copy_direction dir, uint32_t *crcp);

int copy_data(struct rxe_dev *rxe, struct rxe_pd *pd, int access,
	      struct rxe_dma_info *dma, void *addr, int length,
	      enum copy_direction dir, __be32 *crcp);

void *iova_to_vaddr(struct rxe_mem *mem, u64 iova, int length);

enum lookup_type {
	lookup_local,
	lookup_remote,
};

struct rxe_mem *lookup_mem(struct rxe_pd *pd, int access, u32 key,
			   enum lookup_type type);

int mem_check_range(struct rxe_mem *mem, u64 iova, size_t length);

int rxe_mem_map_pages(struct rxe_dev *rxe, struct rxe_mem *mem,
		      u64 *page, int num_pages, u64 iova);

void rxe_mem_cleanup(void *arg);

int advance_dma_data(struct rxe_dma_info *dma, unsigned int length);

#endif /* RXE_MR_H */
