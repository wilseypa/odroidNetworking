/*
 * Copyright (c) 2005 Topspin Communications.  All rights reserved.
 * Copyright (c) 2005 Mellanox Technologies Ltd.  All rights reserved.
 * Copyright (c) 2009 HNR Consulting.  All rights reserved.
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
 *
 * $Id$
 */

#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "perftest_parameters.h"
#include "perftest_resources.h"
#include "multicast_resources.h"
#include "perftest_communication.h"

#ifdef _WIN32
#pragma warning( disable : 4242)
#pragma warning( disable : 4244)
#else
#define __cdecl
#endif

/****************************************************************************** 
 *
 ******************************************************************************/
static int set_mcast_group(struct pingpong_context *ctx,
						   struct perftest_parameters *user_parm,
						   struct mcast_parameters *mcg_params) {

	struct ibv_port_attr port_attr;

	if (ibv_query_gid(ctx->context,user_parm->ib_port,user_parm->gid_index,&mcg_params->port_gid)) {
			return 1;
	}
		
	if (ibv_query_pkey(ctx->context,user_parm->ib_port,DEF_PKEY_IDX,&mcg_params->pkey)) {
		return 1;
	}

	if (ibv_query_port(ctx->context,user_parm->ib_port,&port_attr)) {
		return 1;
	}
	mcg_params->sm_lid  = port_attr.sm_lid;
	mcg_params->sm_sl   = port_attr.sm_sl;
	mcg_params->ib_port = user_parm->ib_port;
	
	if (!strcmp(link_layer_str(user_parm->link_type),"IB")) {
		// Request for Mcast group create registery in SM.
		if (join_multicast_group(SUBN_ADM_METHOD_SET,mcg_params)) {
			fprintf(stderr,"Couldn't Register the Mcast group on the SM\n");
			return 1;
		}
	}
	return 0;
}

/****************************************************************************** 
 *
 ******************************************************************************/
static int send_set_up_connection(struct pingpong_context *ctx,
								  struct perftest_parameters *user_parm,
								  struct pingpong_dest *my_dest,
								  struct mcast_parameters *mcg_params,
								  struct perftest_comm *comm) {

	if (set_up_connection(ctx,user_parm,my_dest)) {
		fprintf(stderr," Unable to set up my IB connection parameters\n");
		return FAILURE;
	}

	if (user_parm->use_mcg && (user_parm->duplex || user_parm->machine == SERVER)) {

		int i = (user_parm->duplex) ? 1 : 0;
		mcg_params->user_mgid = user_parm->user_mgid;
		set_multicast_gid(mcg_params,ctx->qp[0]->qp_num,(int)user_parm->machine);
		if (set_mcast_group(ctx,user_parm,mcg_params)) {
			return 1;
		}
		
		while (i < user_parm->num_of_qps) {
			if (ibv_attach_mcast(ctx->qp[i],&mcg_params->mgid,mcg_params->mlid)) {
				fprintf(stderr, "Couldn't attach QP to MultiCast group");
				return 1;
			}
			i++;
		}

		mcg_params->mcast_state |= MCAST_IS_ATTACHED;
		my_dest->gid = mcg_params->mgid;
		my_dest->lid = mcg_params->mlid;
		my_dest->qpn = QPNUM_MCAST;
	}
	return 0;
}
/****************************************************************************** 
 *
 ******************************************************************************/
int __cdecl main(int argc, char *argv[]) {

	struct ibv_device		 	*ib_dev = NULL;
	struct pingpong_context  	ctx;
	struct pingpong_dest	 	*my_dest  = NULL;
	struct pingpong_dest		*rem_dest = NULL;
	struct perftest_parameters  user_param;
	struct perftest_comm		user_comm;
	struct mcast_parameters     mcg_params;
	int                      	ret_parser,i = 0;
	int                      	size_max_pow = 24;

        init_perfcounters(1,0);
    
	/* init default values to user's parameters */
	memset(&ctx, 0,sizeof(struct pingpong_context));
	memset(&user_param, 0 , sizeof(struct perftest_parameters));
	memset(&mcg_params, 0 , sizeof(struct mcast_parameters));
	memset(&user_comm, 0,sizeof(struct perftest_comm));
 
	user_param.verb    = SEND;
	user_param.tst     = BW;
	user_param.version = VERSION;

	// Configure the parameters values according to user arguments or defalut values.
	ret_parser = parser(&user_param,argv,argc);
	if (ret_parser) {
		if (ret_parser != VERSION_EXIT && ret_parser != HELP_EXIT)
			fprintf(stderr," Parser function exited with Error\n");
		return 1;
	}

	// Finding the IB device selected (or defalut if no selected).
	ib_dev = ctx_find_dev(user_param.ib_devname);
	if (!ib_dev) {
		fprintf(stderr," Unable to find the Infiniband/RoCE deivce\n");
		return 1;
	}

	mcg_params.ib_devname = ibv_get_device_name(ib_dev);

	// Getting the relevant context from the device
	ctx.context = ibv_open_device(ib_dev);
	if (!ctx.context) {
		fprintf(stderr, " Couldn't get context for the device\n");
		return 1;
	}

	// See if MTU and link type are valid and supported.
	if (check_link_and_mtu(ctx.context,&user_param)) {
		fprintf(stderr, " Couldn't get context for the device\n");
		return FAILURE;
	}

	// Print basic test information.
	ctx_print_test_info(&user_param);

	ALLOCATE(my_dest , struct pingpong_dest , user_param.num_of_qps);
	memset(my_dest, 0, sizeof(struct pingpong_dest)*user_param.num_of_qps);
	ALLOCATE(rem_dest , struct pingpong_dest , user_param.num_of_qps);
	memset(rem_dest, 0, sizeof(struct pingpong_dest)*user_param.num_of_qps);

	// Allocating arrays needed for the test.
	alloc_ctx(&ctx,&user_param);

	// copy the rellevant user parameters to the comm struct + creating rdma_cm resources.
	if (create_comm_struct(&user_comm,&user_param)) { 
		fprintf(stderr," Unable to create RDMA_CM resources\n");
		return 1;
	}

	// Create (if nessacery) the rdma_cm ids and channel.
	if (user_param.work_rdma_cm == ON) {

	    if (create_rdma_resources(&ctx,&user_param)) {
			fprintf(stderr," Unable to create the rdma_resources\n");
			return FAILURE;
	    }
		
  	    if (user_param.machine == CLIENT) {

			if (rdma_client_connect(&ctx,&user_param)) {
				fprintf(stderr,"Unable to perform rdma_client function\n");
				return FAILURE;
			}
		
		} else {

			if (rdma_server_connect(&ctx,&user_param)) {
				fprintf(stderr,"Unable to perform rdma_client function\n");
				return FAILURE;
			}
		}
					
	} else {

		 // create all the basic IB resources (data buffer, PD, MR, CQ and events channel)
	    if (ctx_init(&ctx,&user_param)) {
			fprintf(stderr, " Couldn't create IB resources\n");
			return FAILURE;
	    }
	}

	// Set up the Connection.
	if (send_set_up_connection(&ctx,&user_param,my_dest,&mcg_params,&user_comm)) {
		fprintf(stderr," Unable to set up socket connection\n");
		return 1;
	}

	for (i=0; i < user_param.num_of_qps; i++)
		ctx_print_pingpong_data(&my_dest[i],&user_comm);

	// Init the connection and print the local data.
	if (establish_connection(&user_comm)) {
		fprintf(stderr," Unable to init the socket connection\n");
		return 1;
	}

	user_comm.rdma_params->side = REMOTE;
	
	for (i=0; i < user_param.num_of_qps; i++) {

		// shaking hands and gather the other side info.
		if (ctx_hand_shake(&user_comm,&my_dest[i],&rem_dest[i])) {
			fprintf(stderr,"Failed to exchange date between server and clients\n");
			return 1;
		}

		ctx_print_pingpong_data(&rem_dest[i],&user_comm);
	}

	// Joining the Send side port the Mcast gid
	if (user_param.use_mcg && (user_param.machine == CLIENT || user_param.duplex)) {
		memcpy(mcg_params.mgid.raw, rem_dest[0].gid.raw, 16);
		if (set_mcast_group(&ctx,&user_param,&mcg_params)) {
			fprintf(stderr," Unable to Join Sender to Mcast gid\n");
			return 1;
		}
	}

	if (user_param.work_rdma_cm == OFF) {

		// Prepare IB resources for rtr/rts.
		if (ctx_connect(&ctx,rem_dest,&user_param,my_dest)) {
			fprintf(stderr," Unable to Connect the HCA's through the link\n");
			return 1;
		}
	}

	// shaking hands and gather the other side info.
	if (ctx_hand_shake(&user_comm,&my_dest[0],&rem_dest[0])) {
		fprintf(stderr,"Failed to exchange date between server and clients\n");
		return 1;    
	}

    if (user_param.use_event) {

		if (ibv_req_notify_cq(ctx.send_cq, 0)) {
			fprintf(stderr, " Couldn't request CQ notification\n");
            return 1;
        }

		if (ibv_req_notify_cq(ctx.recv_cq, 0)) {
			fprintf(stderr, " Couldn't request CQ notification\n");
            return 1;
        }
    }

	printf(RESULT_LINE);
	printf(RESULT_FMT);


	if (user_param.test_method == RUN_ALL) {

		if (user_param.connection_type == UD) 
		   size_max_pow =  (int)UD_MSG_2_EXP(MTU_SIZE(user_param.curr_mtu)) + 1;

		for (i = 1; i < size_max_pow ; ++i) {

			user_param.size = (uint64_t)1 << i;

			if (user_param.machine == CLIENT || user_param.duplex)
				ctx_set_send_wqes(&ctx,&user_param,rem_dest);

			if (user_param.machine == SERVER || user_param.duplex) {
				if (ctx_set_recv_wqes(&ctx,&user_param)) {
					fprintf(stderr," Failed to post receive recv_wqes\n");
					return 1;
				}
			}

			if (ctx_hand_shake(&user_comm,&my_dest[0],&rem_dest[0])) {
				fprintf(stderr,"Failed to exchange date between server and clients\n");
				return 1;
			}

			if (user_param.duplex) {

				if(run_iter_bi(&ctx,&user_param))
					return 17;

			} else if (user_param.machine == CLIENT) {

				if(run_iter_bw(&ctx,&user_param)) {
					return 17;
				}

			} else	{		  				

				if(run_iter_bw_server(&ctx,&user_param)) {
					return 17;
				}
			}

			print_report_bw(&user_param);

			if (ctx_hand_shake(&user_comm,&my_dest[0],&rem_dest[0])) {
				fprintf(stderr,"Failed to exchange date between server and clients\n");
				return 1;
			}
		}

	} else if (user_param.test_method == RUN_REGULAR) { 

		if (user_param.machine == CLIENT || user_param.duplex)
			ctx_set_send_wqes(&ctx,&user_param,rem_dest);

		if (user_param.machine == SERVER || user_param.duplex) {
			if (ctx_set_recv_wqes(&ctx,&user_param)) {
				fprintf(stderr," Failed to post receive recv_wqes\n");
				return 1;
			}
		}

		if (ctx_hand_shake(&user_comm,&my_dest[0],&rem_dest[0])) {
			fprintf(stderr,"Failed to exchange date between server and clients\n");
			return 1;
		}

		if (user_param.duplex) {

			if(run_iter_bi(&ctx,&user_param))
				return 17;

		} else if (user_param.machine == CLIENT) {

			if(run_iter_bw(&ctx,&user_param)) {
				return 17;
			}

		} else if(run_iter_bw_server(&ctx,&user_param)) {

			return 17;
		}

		print_report_bw(&user_param);

	} else if (user_param.test_method == RUN_INFINITELY) { 

		if (user_param.machine == CLIENT)
			ctx_set_send_wqes(&ctx,&user_param,rem_dest);

		else if (user_param.machine == SERVER) {

			if (ctx_set_recv_wqes(&ctx,&user_param)) {
				fprintf(stderr," Failed to post receive recv_wqes\n");
				return 1;
			}
		}

		if (ctx_hand_shake(&user_comm,&my_dest[0],&rem_dest[0])) {
			fprintf(stderr,"Failed to exchange date between server and clients\n");
			return 1;
		}

		if (user_param.machine == CLIENT) {

			if(run_iter_bw_infinitely(&ctx,&user_param)) { 
				fprintf(stderr," Error occured while running infinitely! aborting ...\n");
				return 1;
			}

		} else if (user_param.machine == SERVER) {

			if(run_iter_bw_infinitely_server(&ctx,&user_param)) {
				fprintf(stderr," Error occured while running infinitely on server! aborting ...\n");
				return 1;
			}
		}
	}
		
	if (ctx_close_connection(&user_comm,&my_dest[0],&rem_dest[0])) {
		fprintf(stderr," Failed to close connection between server and client\n");
		return 1;
	}

	printf(RESULT_LINE);
	return 0; 
}

