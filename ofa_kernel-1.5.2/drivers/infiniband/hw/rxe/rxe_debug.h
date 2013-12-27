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
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
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

#ifndef RXE_DEBUG_H
#define RXE_DEBUG_H

enum {
	RXE_DEBUG_TRACE		= (1 << 0),
	RXE_DEBUG_DEBUG		= (1 << 1),
	RXE_DEBUG_WARN		= (1 << 2),
	RXE_DEBUG_ERROR		= (1 << 3),
	RXE_DEBUG_DUMP		= (1 << 4),
};

#ifdef CONFIG_INFINIBAND_RXE_DEBUG

extern int rxe_ind;
char *rxe_indent(int ind);

#define rxe_enter(obj, format, arg...)					\
	do {								\
		struct rxe_dev *_rxe = (struct rxe_dev *)(obj);		\
		if (rxe_debug_flags & RXE_DEBUG_TRACE) {		\
			pr_info("%4s:enter %s: " format,		\
				_rxe ? _rxe->ib_dev.name : "",		\
				__func__, ## arg);	\
		}							\
		__acquire(rxe_log_func);				\
	} while (0)

#define rxe_exit(obj, format, arg...)					\
	do {								\
		struct rxe_dev *_rxe = (struct rxe_dev *)(obj);		\
		if (rxe_debug_flags & RXE_DEBUG_TRACE) {		\
			pr_info("%4s:exit  %s(%d): " format,	\
				_rxe ? _rxe->ib_dev.name : "",		\
				__func__,		\
					__LINE__, ## arg);		\
		}							\
		__release(rxe_log_func);				\
	} while (0)

#define rxe_debug(obj, format, arg...)					\
	do {								\
		struct rxe_dev *_rxe = (struct rxe_dev *)(obj);		\
		if (rxe_debug_flags & RXE_DEBUG_DEBUG) {		\
			pr_info("%4s:debug %s(%d): " format,	\
				_rxe ? _rxe->ib_dev.name : "",		\
				__func__,		\
					__LINE__, ## arg);		\
		}							\
	} while (0)

#define rxe_warn(obj, format, arg...)					\
	do {								\
		struct rxe_dev *_rxe = (struct rxe_dev *)(obj);		\
		if (rxe_debug_flags & RXE_DEBUG_WARN)			\
			pr_info("%4s:warn  %s: " format,		\
				_rxe ? _rxe->ib_dev.name : "",		\
				__func__, ## arg);	\
	} while (0)

#define rxe_error(obj, format, arg...)					\
	do {								\
		struct rxe_dev *_rxe = (struct rxe_dev *)(obj);		\
		if (rxe_debug_flags & RXE_DEBUG_ERROR)			\
			pr_info("%4s:error %s: " format,		\
				_rxe ? _rxe->ib_dev.name : "",		\
				__func__, ## arg);	\
	} while (0)

#else	/* !CONFIG_INFINIBAND_RXE_DEBUG */

#define rxe_enter(obj, format, arg...) do {} while (0)
#define rxe_exit(obj, format, arg...)  do {} while (0)
#define rxe_debug(obj, format, arg...) do {} while (0)
#define rxe_warn(obj, format, arg...)					\
	do {								\
		printk(KERN_WARNING "rxe: " format, ##arg);		\
	} while (0)
#define rxe_error(obj, format, arg...)					\
	do {								\
		printk(KERN_ERR "rxe: " format, ##arg);		\
	} while (0)

#endif	/* CONFIG_INFINIBAND_RXE_DEBUG */

#endif /* RXE_DEBUG_H */
