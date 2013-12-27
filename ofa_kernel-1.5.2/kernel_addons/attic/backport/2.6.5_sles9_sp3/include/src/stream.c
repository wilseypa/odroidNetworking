/*
 *     SUCS NET3:
 *
 *     Generic stream handling routines. These are generic for most
 *     protocols. Even IP. Tonight 8-).
 *     This is used because TCP, LLC (others too) layer all have mostly
 *     identical sendmsg() and recvmsg() code.
 *     So we (will) share it here.
 *
 *     Authors:        Arnaldo Carvalho de Melo <acme@conectiva.com.br>
 *                     (from old tcp.c code)
 *                     Alan Cox <alan@redhat.com> (Borrowed comments 8-))
 */
#include <linux/module.h>
#include <linux/net.h>
#include <linux/signal.h>
#include <linux/tcp.h>
#include <linux/wait.h>
#include <net/sock.h>

/* Added for sock.c */
/**
 * sk_wait_data - wait for data to arrive at sk_receive_queue
 * sk - sock to wait on
 * timeo - for how long
 *
 * Now socket state including sk->sk_err is changed only under lock,
 * hence we may omit checks after joining wait queue.
 * We check receive queue before schedule() only as optimization;
 * it is very likely that release_sock() added new data.
 */
int sk_wait_data(struct sock *sk, long *timeo)
{
	int rc;
	DEFINE_WAIT(wait);

	prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);
	set_bit(SOCK_ASYNC_WAITDATA, &sk->sk_socket->flags);
	rc = sk_wait_event(sk, timeo, !skb_queue_empty(&sk->sk_receive_queue));
	clear_bit(SOCK_ASYNC_WAITDATA, &sk->sk_socket->flags);
	finish_wait(sk->sk_sleep, &wait);
	return rc;
}

EXPORT_SYMBOL(sk_wait_data);

/*
 *	Get a socket option on an socket.
 *
 *	FIX: POSIX 1003.1g is very ambiguous here. It states that
 *	asynchronous errors should be reported by getsockopt. We assume
 *	this means if you specify SO_ERROR (otherwise whats the point of it).
 */
int sock_common_getsockopt(struct socket *sock, int level, int optname,
			   char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;

	return sk->sk_prot->getsockopt(sk, level, optname, optval, optlen);
}

EXPORT_SYMBOL(sock_common_getsockopt);

int sock_common_recvmsg(struct kiocb *iocb, struct socket *sock,
			struct msghdr *msg, size_t size, int flags)
{
	struct sock *sk = sock->sk;
	int addr_len = 0;
	int err;

	err = sk->sk_prot->recvmsg(iocb, sk, msg, size, flags & MSG_DONTWAIT,
				   flags & ~MSG_DONTWAIT, &addr_len);
	if (err >= 0)
		msg->msg_namelen = addr_len;
	return err;
}

EXPORT_SYMBOL(sock_common_recvmsg);

/*
 *	Set socket options on an inet socket.
 */
int sock_common_setsockopt(struct socket *sock, int level, int optname,
			   char __user *optval, int optlen)
{
	struct sock *sk = sock->sk;

	return sk->sk_prot->setsockopt(sk, level, optname, optval, optlen);
}

EXPORT_SYMBOL(sock_common_setsockopt);

void sk_common_release(struct sock *sk)
{
	if (sk->sk_prot->destroy)
		sk->sk_prot->destroy(sk);

	/*
	 * Observation: when sock_common_release is called, processes have
	 * no access to socket. But net still has.
	 * Step one, detach it from networking:
	 *
	 * A. Remove from hash tables.
	 */

	sk->sk_prot->unhash(sk);

	/*
	 * In this point socket cannot receive new packets, but it is possible
	 * that some packets are in flight because some CPU runs receiver and
	 * did hash table lookup before we unhashed socket. They will achieve
	 * receive queue and will be purged by socket destructor.
	 *
	 * Also we still have packets pending on receive queue and probably,
	 * our own packets waiting in device queues. sock_destroy will drain
	 * receive queue, but transmitted packets will delay socket destruction
	 * until the last reference will be released.
	 */

	sock_orphan(sk);
	sock_put(sk);
}

EXPORT_SYMBOL(sk_common_release);

/*****************************/
/**
 * sk_stream_write_space - stream socket write_space callback.
 * sk - socket
 *
 * FIXME: write proper description
 */
void sk_stream_write_space(struct sock *sk)
{
	struct socket *sock = sk->sk_socket;

	if (sk_stream_wspace(sk) >= sk_stream_min_wspace(sk) && sock) {
		clear_bit(SOCK_NOSPACE, &sock->flags);

		if (sk->sk_sleep && waitqueue_active(sk->sk_sleep))
			wake_up_interruptible(sk->sk_sleep);
		if (sock->fasync_list && !(sk->sk_shutdown & SEND_SHUTDOWN))
			sock_wake_async(sock, 2, POLL_OUT);
	}
}

EXPORT_SYMBOL(sk_stream_write_space);

/**
 * sk_stream_wait_connect - Wait for a socket to get into the connected state
 * @sk - sock to wait on
 * @timeo_p - for how long to wait
 *
 * Must be called with the socket locked.
 */
int sk_stream_wait_connect(struct sock *sk, long *timeo_p)
{
	struct task_struct *tsk = current;
	DEFINE_WAIT(wait);

	while (1) {
		if (sk->sk_err)
			return sock_error(sk);
		if ((1 << sk->sk_state) & ~(TCPF_SYN_SENT | TCPF_SYN_RECV))
			return -EPIPE;
		if (!*timeo_p)
			return -EAGAIN;
		if (signal_pending(tsk))
			return sock_intr_errno(*timeo_p);

		prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);
#if 0
		sk->sk_write_pending++;
#endif
		if (sk_wait_event(sk, timeo_p,
				  !((1 << sk->sk_state) &
				    ~(TCPF_ESTABLISHED | TCPF_CLOSE_WAIT))))
			break;
		finish_wait(sk->sk_sleep, &wait);
#if 0
		sk->sk_write_pending--;
#endif
	}
	return 0;
}

EXPORT_SYMBOL(sk_stream_wait_connect);

/**
 * sk_stream_closing - Return 1 if we still have things to send in our buffers.
 * @sk - socket to verify
 */
static inline int sk_stream_closing(struct sock *sk)
{
	return (1 << sk->sk_state) &
	       (TCPF_FIN_WAIT1 | TCPF_CLOSING | TCPF_LAST_ACK);
}

void sk_stream_wait_close(struct sock *sk, long timeout)
{
	if (timeout) {
		DEFINE_WAIT(wait);

		do {
			prepare_to_wait(sk->sk_sleep, &wait,
					TASK_INTERRUPTIBLE);
			if (sk_wait_event(sk, &timeout, !sk_stream_closing(sk)))
				break;
		} while (!signal_pending(current) && timeout);

		finish_wait(sk->sk_sleep, &wait);
	}
}

EXPORT_SYMBOL(sk_stream_wait_close);

/**
 * sk_stream_wait_memory - Wait for more memory for a socket
 * @sk - socket to wait for memory
 * @timeo_p - for how long
 */
int sk_stream_wait_memory(struct sock *sk, long *timeo_p)
{
	int err = 0;
	long vm_wait = 0;
	long current_timeo = *timeo_p;
	DEFINE_WAIT(wait);

	if (sk_stream_memory_free(sk))
		current_timeo = vm_wait = (net_random() % (HZ / 5)) + 2;

	while (1) {
		set_bit(SOCK_ASYNC_NOSPACE, &sk->sk_socket->flags);

		prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);

		if (sk->sk_err || (sk->sk_shutdown & SEND_SHUTDOWN))
			goto do_error;
		if (!*timeo_p)
			goto do_nonblock;
		if (signal_pending(current))
			goto do_interrupted;
		clear_bit(SOCK_ASYNC_NOSPACE, &sk->sk_socket->flags);
		if (sk_stream_memory_free(sk) && !vm_wait)
			break;

		set_bit(SOCK_NOSPACE, &sk->sk_socket->flags);
#if 0
		sk->sk_write_pending++;
#endif
		sk_wait_event(sk, &current_timeo, sk_stream_memory_free(sk) &&
						  vm_wait);
#if 0
		sk->sk_write_pending--;
#endif

		if (vm_wait) {
			vm_wait -= current_timeo;
			current_timeo = *timeo_p;
			if (current_timeo != MAX_SCHEDULE_TIMEOUT &&
			    (current_timeo -= vm_wait) < 0)
				current_timeo = 0;
			vm_wait = 0;
		}
		*timeo_p = current_timeo;
	}
out:
	finish_wait(sk->sk_sleep, &wait);
	return err;

do_error:
	err = -EPIPE;
	goto out;
do_nonblock:
	err = -EAGAIN;
	goto out;
do_interrupted:
	err = sock_intr_errno(*timeo_p);
	goto out;
}

EXPORT_SYMBOL(sk_stream_wait_memory);

void sk_stream_rfree(struct sk_buff *skb)
{
	struct sock *sk = skb->sk;

	atomic_sub(skb->truesize, &sk->sk_rmem_alloc);
	sk->sk_forward_alloc += skb->truesize;
}

EXPORT_SYMBOL(sk_stream_rfree);

int sk_stream_error(struct sock *sk, int flags, int err)
{
	if (err == -EPIPE)
		err = sock_error(sk) ? : -EPIPE;
	if (err == -EPIPE && !(flags & MSG_NOSIGNAL))
		send_sig(SIGPIPE, current, 0);
	return err;
}

EXPORT_SYMBOL(sk_stream_error);

void __sk_stream_mem_reclaim(struct sock *sk)
{
	if (sk->sk_forward_alloc >= SK_STREAM_MEM_QUANTUM) {
		atomic_sub(sk->sk_forward_alloc / SK_STREAM_MEM_QUANTUM,
			   ((struct proto_sles9 *)sk->sk_prot)->memory_allocated);
		sk->sk_forward_alloc &= SK_STREAM_MEM_QUANTUM - 1;
		if (*((struct proto_sles9 *)sk->sk_prot)->memory_pressure &&
		    (atomic_read(((struct proto_sles9 *)sk->sk_prot)->memory_allocated) <
		     ((struct proto_sles9 *)sk->sk_prot)->sysctl_mem[0]))
			*((struct proto_sles9 *)sk->sk_prot)->memory_pressure = 0;
	}
}

EXPORT_SYMBOL(__sk_stream_mem_reclaim);

int sk_stream_mem_schedule(struct sock *sk, int size, int kind)
{
	int amt = sk_stream_pages(size);

	sk->sk_forward_alloc += amt * SK_STREAM_MEM_QUANTUM;
	atomic_add(amt, ((struct proto_sles9 *)sk->sk_prot)->memory_allocated);

	/* Under limit. */
	if (atomic_read(((struct proto_sles9 *)sk->sk_prot)->memory_allocated) <
			((struct proto_sles9 *)sk->sk_prot)->sysctl_mem[0]) {
		if (*((struct proto_sles9 *)sk->sk_prot)->memory_pressure)
			*((struct proto_sles9 *)sk->sk_prot)->memory_pressure = 0;
		return 1;
	}

	/* Over hard limit. */
	if (atomic_read(((struct proto_sles9 *)sk->sk_prot)->memory_allocated) >
			((struct proto_sles9 *)sk->sk_prot)->sysctl_mem[2]) {
		((struct proto_sles9 *)sk->sk_prot)->enter_memory_pressure();
		goto suppress_allocation;
	}

	/* Under pressure. */
	if (atomic_read(((struct proto_sles9 *)sk->sk_prot)->memory_allocated) >
			((struct proto_sles9 *)sk->sk_prot)->sysctl_mem[1])
		((struct proto_sles9 *)sk->sk_prot)->enter_memory_pressure();

	if (kind) {
		if (atomic_read(&sk->sk_rmem_alloc) < ((struct proto_sles9 *)sk->sk_prot)->sysctl_rmem[0])
			return 1;
	} else if (sk->sk_wmem_queued < ((struct proto_sles9 *)sk->sk_prot)->sysctl_wmem[0])
		return 1;

	if (!*((struct proto_sles9 *)sk->sk_prot)->memory_pressure
#if 0
	    ||
	    ((struct proto_sles9 *)sk->sk_prot)->sysctl_mem[2] > atomic_read(sk->sk_prot->sockets_allocated) *
				sk_stream_pages(sk->sk_wmem_queued +
						atomic_read(&sk->sk_rmem_alloc) +
						sk->sk_forward_alloc)
#endif
	    )
		return 1;

suppress_allocation:

	if (!kind) {
		sk_stream_moderate_sndbuf(sk);

		/* Fail only if socket is _under_ its sndbuf.
		 * In this case we cannot block, so that we have to fail.
		 */
		if (sk->sk_wmem_queued + size >= sk->sk_sndbuf)
			return 1;
	}

	/* Alas. Undo changes. */
	sk->sk_forward_alloc -= amt * SK_STREAM_MEM_QUANTUM;
	atomic_sub(amt, ((struct proto_sles9 *)sk->sk_prot)->memory_allocated);
	return 0;
}

EXPORT_SYMBOL(sk_stream_mem_schedule);

void sk_stream_kill_queues(struct sock *sk)
{
	/* First the read buffer. */
	__skb_queue_purge(&sk->sk_receive_queue);

	/* Next, the error queue. */
	__skb_queue_purge(&sk->sk_error_queue);

	/* Next, the write queue. */
	BUG_TRAP(skb_queue_empty(&sk->sk_write_queue));

	/* Account for returned memory. */
	sk_stream_mem_reclaim(sk);

	BUG_TRAP(!sk->sk_wmem_queued);
	BUG_TRAP(!sk->sk_forward_alloc);

	/* It is _impossible_ for the backlog to contain anything
	 * when we get here.  All user references to this socket
	 * have gone away, only the net layer knows can touch it.
	 */
}

EXPORT_SYMBOL(sk_stream_kill_queues);

