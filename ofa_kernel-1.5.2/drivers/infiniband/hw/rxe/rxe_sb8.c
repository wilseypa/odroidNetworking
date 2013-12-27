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

#include "rxe.h"

#include "rxe_loc.h"
#include "sb8_tables.h"

/* TODO we need big endian versions of sb8 */

/* slice by 8 algorithm for crc32 */
static __be32 sb8_le(__be32 last_crc, const void *data, const size_t length)
{
	const u8	*p8;
	int		init_bytes;
	size_t		qwords;
	int		end_bytes;
	u32		*p32;
	size_t		i;
	__be32		crc;
	u32		q;
	u8		i1, i2, i3, i4;

	crc = last_crc;

	if (length == 0 || rxe_crc_disable)
		return crc;

	p8		= data;
	init_bytes	= (-(long)p8) & 7;
	if (init_bytes > length)
		init_bytes = length;
	qwords		= (length - init_bytes) >> 3;
	end_bytes	= (length - init_bytes) & 7;
	p32		= (u32 *)(p8 + init_bytes);

	for (i = 0; i < init_bytes; i++) {
		i1 = crc ^ *p8++;
		crc = t32[i1] ^ (crc >> 8);
	}

	for (i = 0; i < qwords; i++) {

		q = *p32++ ^ crc;

		i1 = q;
		i2 = q >> 8;
		i3 = q >> 16;
		i4 = q >> 24;

		crc = t88[i1] ^ t80[i2] ^ t72[i3] ^ t64[i4];

		q = *p32++;

		i1 = q;
		i2 = q >> 8;
		i3 = q >> 16;
		i4 = q >> 24;

		crc ^= t56[i1] ^ t48[i2] ^ t40[i3] ^ t32[i4];
	}

	p8 = (u8 *)p32;

	for (i = 0; i < end_bytes; i++) {
		i1 = crc ^ *p8++;
		crc = t32[i1] ^ (crc >> 8);
	}

	return crc;
}

/* slice by 8 with a copy */
__be32 sb8_le_copy(void *dest, const void *data,
		   const size_t length, __be32 last_crc)
{
	const u8	*p8;
	u8		*d8;
	int		init_bytes;
	size_t		qwords;
	int		end_bytes;
	u64		*p64;
	u64		*d64;
	size_t		i;
	__be32		crc;
	u32		q;
	u64		t;
	u8		i1, i2, i3, i4;

	crc = last_crc;

	if (rxe_crc_disable) {
		memcpy(dest, data, length);
		return crc;
	}

	p8		= data;
	d8		= dest;
	init_bytes	= (-(long)p8) & 7;
	if (init_bytes > length)
		init_bytes = length;
	qwords		= (length - init_bytes) >> 3;
	end_bytes	= (length - init_bytes) & 7;
	p64		= (u64 *)(p8 + init_bytes);
	d64		= (u64 *)(d8 + init_bytes);

	for (i = 0; i < init_bytes; i++) {
		t = *p8++;
		*d8++ = t;
		i1 = crc ^ t;
		crc = t32[i1] ^ (crc >> 8);
	}

	for (i = 0; i < qwords; i++) {
		t = *p64++;
		*d64++ = t;
		q = (u32)t ^ crc;

		i1 = q;
		i2 = q >> 8;
		i3 = q >> 16;
		i4 = q >> 24;

		crc = t88[i1] ^ t80[i2] ^ t72[i3] ^ t64[i4];

		t >>= 32;
		q = (u32)t;

		i1 = q;
		i2 = q >> 8;
		i3 = q >> 16;
		i4 = q >> 24;

		crc ^= t56[i1] ^ t48[i2] ^ t40[i3] ^ t32[i4];
	}

	p8 = (u8 *)p64;
	d8 = (u8 *)d64;

	for (i = 0; i < end_bytes; i++) {
		t = *p8++;
		*d8++ = t;
		i1 = crc ^ t;
		crc = t32[i1] ^ (crc >> 8);
	}

	return crc;
}

/* Compute a partial ICRC for all the IB transport headers. */
__be32 rxe_sb8_ib_headers(struct rxe_pkt_info *pkt)
{
	u32 crc;
	unsigned int length;
	unsigned int grh_offset;
	unsigned int bth_offset;
	u8 tmp[RXE_LRH_BYTES + RXE_GRH_BYTES + RXE_BTH_BYTES];

	/* This seed is the result of computing a CRC with a seed of
	 * 0xfffffff and 8 bytes of 0xff representing a masked LRH. */
	crc = 0xdebb20e3;

	length = RXE_BTH_BYTES;
	grh_offset = 0;
	bth_offset = 0;

	if (pkt->mask & RXE_LRH_MASK) {
		length += RXE_LRH_BYTES;
		grh_offset += RXE_LRH_BYTES;
		bth_offset += RXE_LRH_BYTES;
	}
	if (pkt->mask & RXE_GRH_MASK) {
		length += RXE_GRH_BYTES;
		bth_offset += RXE_GRH_BYTES;
	}

	memcpy(tmp, pkt->hdr, length);

	if (pkt->mask & RXE_GRH_MASK) {
		tmp[grh_offset + 0] |= 0x0f;	/* GRH: tclass */
		tmp[grh_offset + 1] = 0xff;
		tmp[grh_offset + 2] = 0xff;
		tmp[grh_offset + 3] = 0xff;
		tmp[grh_offset + 7] = 0xff;
	}

	tmp[bth_offset + 4] = 0xff;		/* BTH: resv8a */

	crc = sb8_le(crc, tmp + grh_offset, length - grh_offset);

	/* And finish to compute the CRC on the remainder of the headers. */
	crc = sb8_le(crc, pkt->hdr + length,
		       rxe_opcode[pkt->opcode].length - RXE_BTH_BYTES);

	return crc;
}

/* Compute the ICRC for a packet (incoming or outgoing). */
__be32 rxe_sb8(struct rxe_pkt_info *pkt)
{
	u32 crc;
	int size;

	crc = rxe_sb8_ib_headers(pkt);

	/* And finish to compute the CRC on the remainder. */
	size = pkt->paylen - rxe_opcode[pkt->opcode].length - RXE_ICRC_SIZE;
	crc = sb8_le(crc, payload_addr(pkt), size);
	crc = ~crc;

	return crc;
}
