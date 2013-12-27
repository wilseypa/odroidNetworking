EXTRA_CFLAGS += $(OPENIB_KERNEL_EXTRA_CFLAGS) \
		$(KERNEL_MEMTRACK_CFLAGS) \
		$(KERNEL_NFS_FS_CFLAGS) \
		-I$(CWD)/include \
		-I$(CWD)/drivers/infiniband/ulp/ipoib \
		-I$(CWD)/drivers/infiniband/debug \
		-I$(CWD)/drivers/net/cxgb3 \
		-I$(CWD)/net/rds \
		-I$(CWD)/drivers/net/mlx4 \
		-I$(CWD)/drivers/infiniband/hw/mlx4

obj-$(CONFIG_INFINIBAND)        += drivers/infiniband/
obj-$(CONFIG_CHELSIO_T3)        += drivers/net/cxgb3/
obj-$(CONFIG_MLX4_CORE)         += drivers/net/mlx4/
obj-$(CONFIG_RDS)               += net/rds/
obj-$(CONFIG_MEMTRACK)          += drivers/infiniband/debug/
obj-$(CONFIG_SUNRPC_XPRT_RDMA)  += net/sunrpc/
obj-$(CONFIG_SUNRPC_XPRT_RDMA)  += net/sunrpc/auth_gss/
obj-$(CONFIG_SUNRPC_XPRT_RDMA)  += net/sunrpc/xprtrdma/
obj-$(CONFIG_SUNRPC_XPRT_RDMA)  += fs/nfs/
obj-$(CONFIG_SUNRPC_XPRT_RDMA)  += fs/lockd/
obj-$(CONFIG_SUNRPC_XPRT_RDMA)  += fs/exportfs/
obj-$(CONFIG_SUNRPC_XPRT_RDMA)  += fs/nfs_common/
obj-$(CONFIG_SUNRPC_XPRT_RDMA)  += fs/nfsd/
