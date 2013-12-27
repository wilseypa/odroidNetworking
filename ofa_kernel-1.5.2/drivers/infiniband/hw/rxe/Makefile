
EXTRA_PRE_CFLAGS := -D__KERNEL__ $(BACKPORTS) $(DEBUG) \
	-I/usr/src/ofa_kernel/include

obj-m += ib_rxe.o ib_rxe_net.o ib_rxe_sample.o

ib_rxe-y := \
	rxe.o \
	rxe_comp.o \
	rxe_req.o \
	rxe_resp.o \
	rxe_recv.o \
	rxe_pool.o \
	rxe_queue.o \
	rxe_verbs.o \
	rxe_av.o \
	rxe_srq.o \
	rxe_qp.o \
	rxe_cq.o \
	rxe_mr.o \
	rxe_dma.o \
	rxe_opcode.o \
	rxe_mmap.o \
	rxe_arbiter.o \
	rxe_sb8.o \
	rxe_mcast.o \
	rxe_task.o

ib_rxe_net-y := \
	rxe_net.o \
	rxe_net_sysfs.o

ib_rxe_sample-y := \
	rxe_sample.o

# Generation of SB8 tables for Ethernet CRC32
hostprogs-y := gen_sb8tables
clean-files := sb8_tables.h

$(obj)/rxe_sb8.o: $(obj)/sb8_tables.h

quiet_cmd_sb8 = GEN $@
	cmd_sb8 = $< --flop --swip --flip --swap --poly=0xedb88320 > $@

$(obj)/sb8_tables.h: $(obj)/gen_sb8tables
	$(call cmd,sb8)
