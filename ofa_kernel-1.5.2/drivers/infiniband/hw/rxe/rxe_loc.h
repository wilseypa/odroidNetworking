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

#ifndef RXE_LOC_H
#define RXE_LOC_H

/* local declarations shared between rxe files */

extern struct ib_dma_mapping_ops rxe_dma_mapping_ops;

void rxe_release(struct kref *kref);

void arbiter_skb_queue(struct rxe_dev *rxe,
		       struct rxe_qp *qp, struct sk_buff *skb);

int rxe_arbiter(void *arg);
int rxe_completer(void *arg);
int rxe_requester(void *arg);
int rxe_responder(void *arg);

__be32 rxe_sb8_ib_headers(struct rxe_pkt_info *pkt);
__be32 rxe_sb8(struct rxe_pkt_info *pkt);
__be32 sb8_le_copy(void *dest, const void *data,
		   const size_t length, __be32 last_crc);

static inline __be32 sb8_copy(void *dest, const void *src,
			      const size_t length, __be32 crc)
{
#ifdef __LITTLE_ENDIAN
	return sb8_le_copy(dest, src, length, crc);
#else
	/* TODO this is wrong */
	return sb8_le_copy(dest, src, length, crc);
#endif
}

void rxe_resp_queue_pkt(struct rxe_dev *rxe,
			struct rxe_qp *qp, struct sk_buff *skb);

void rxe_comp_queue_pkt(struct rxe_dev *rxe,
			struct rxe_qp *qp, struct sk_buff *skb);

static inline unsigned wr_opcode_mask(int opcode, struct rxe_qp *qp)
{
	return rxe_wr_opcode_info[opcode].mask[qp->ibqp.qp_type];
}

#endif /* RXE_LOC_H */
