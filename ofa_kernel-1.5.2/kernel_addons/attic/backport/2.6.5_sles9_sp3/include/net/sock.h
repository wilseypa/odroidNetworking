#ifndef NET_SOCK_H_BACKPORT
#define NET_SOCK_H_BACKPORT

#include_next <net/sock.h>

struct proto_sles9 {
	struct proto	proto_orig;
	/* Memory pressure */
	void                    (*enter_memory_pressure)(void);
	atomic_t                *memory_allocated;      /* Current allocated memory. */
	atomic_t                *sockets_allocated;     /* Current number of sockets. */
	/*
	 * Pressure flag: try to collapse.
	 * Technical note: it is used by multiple contexts non atomically.
	 * All the sk_stream_mem_schedule() is of this nature: accounting
	 * is strict, actions are advisory and have some latency.
	 */
	int                     *memory_pressure;
	int                     *sysctl_mem;
	int                     *sysctl_wmem;
	int                     *sysctl_rmem;
	int                     max_header;

	kmem_cache_t            *slab;
	unsigned int            obj_size;

	atomic_t                *orphan_count;

	struct request_sock_ops *rsk_prot;
	struct timewait_sock_ops *twsk_prot;

	struct module           *owner;
};

extern int sock_common_getsockopt(struct socket *sock, int level, int optname,
				  char __user *optval, int __user *optlen);
extern int sock_common_recvmsg(struct kiocb *iocb, struct socket *sock,
			       struct msghdr *msg, size_t size, int flags);
extern int sock_common_setsockopt(struct socket *sock, int level, int optname,
				  char __user *optval, int optlen);

extern void sk_common_release(struct sock *sk);
static inline
int proto_register(struct proto_sles9 *prot, int alloc_slab)
{
	int rc = -ENOBUFS;

	if (alloc_slab) {
		prot->slab = kmem_cache_create(prot->proto_orig.name, prot->obj_size, 0,
					       SLAB_HWCACHE_ALIGN, NULL);

		if (prot->slab == NULL) {
			printk(KERN_CRIT "%s: Can't create sock SLAB cache!\n",
			       prot->proto_orig.name);
			goto out;
		}
	}
	rc = 0;
out:
	return rc;
}

static inline
void proto_unregister(struct proto_sles9 *prot)
{
	if (prot->slab != NULL) {
		kmem_cache_destroy(prot->slab);
		prot->slab = NULL;
	}
}

static inline struct sock *sk_alloc_new(int family, gfp_t priority,
					struct proto_sles9 *prot, int zero_it)
{
	struct sock *sk = sk_alloc(family, priority,
				   zero_it ? prot->obj_size : 0,
				   prot->slab);
	if (!sk)
		return NULL;
	sk->sk_prot = (struct proto *) prot;
	sk->sk_owner = THIS_MODULE;
	__module_get(sk->sk_owner);
	return sk;
}

#define sk_alloc sk_alloc_new

static inline
void sock_init_data_new(struct socket *sock, struct sock *sk)
{
	sock_init_data(sock, sk);
	sk->sk_owner = THIS_MODULE;
}

#define sock_init_data sock_init_data_new

#define sk_refcnt_debug_inc(sk) do { } while (0)

static inline void sk_acceptq_removed(struct sock *sk)
{
	sk->sk_ack_backlog--;
}

static inline void sk_acceptq_added(struct sock *sk)
{
	sk->sk_ack_backlog++;
}

static inline int sk_acceptq_is_full(struct sock *sk);
extern void __sk_stream_mem_reclaim(struct sock *sk);
extern int sk_stream_mem_schedule(struct sock *sk, int size, int kind);
extern void sk_stream_write_space(struct sock *sk);
static inline int sk_acceptq_is_full(struct sock *sk)
{
	return sk->sk_ack_backlog > sk->sk_max_ack_backlog;
}

/*
 * Compute minimal free write space needed to queue new packets.
 */
static inline int sk_stream_min_wspace(struct sock *sk)
{
	return sk->sk_wmem_queued / 2;
}

static inline int sk_stream_wspace(struct sock *sk)
{
	return sk->sk_sndbuf - sk->sk_wmem_queued;
}

static inline int sk_stream_memory_free(struct sock *sk)
{
	return sk->sk_wmem_queued < sk->sk_sndbuf;
}

static inline void sk_stream_free_skb(struct sock *sk, struct sk_buff *skb)
{
	sk->sk_wmem_queued   -= skb->truesize;
	sk->sk_forward_alloc += skb->truesize;
	__kfree_skb(skb);
}

#define sk_wait_event(__sk, __timeo, __condition)		\
({	int rc;							\
	release_sock(__sk);					\
	rc = __condition;					\
	if (!rc) {						\
		*(__timeo) = schedule_timeout(*(__timeo));	\
		rc = __condition;				\
	}							\
	lock_sock(__sk);					\
	rc;							\
})

static inline void sk_charge_skb(struct sock *sk, struct sk_buff *skb)
{
	sk->sk_wmem_queued   += skb->truesize;
	sk->sk_forward_alloc -= skb->truesize;
}

static inline int skb_copy_to_page(struct sock *sk, char __user *from,
				   struct sk_buff *skb, struct page *page,
				   int off, int copy)
{
	if (copy_from_user(page_address(page) + off, from, copy))
		return -EFAULT;

	skb->len	     += copy;
	skb->data_len	     += copy;
	skb->truesize	     += copy;
	sk->sk_wmem_queued   += copy;
	sk->sk_forward_alloc -= copy;
	return 0;
}

extern int sk_stream_wait_connect(struct sock *sk, long *timeo_p);
extern int sk_stream_wait_memory(struct sock *sk, long *timeo_p);
extern void sk_stream_wait_close(struct sock *sk, long timeo_p);
extern int sk_stream_error(struct sock *sk, int flags, int err);
extern void sk_stream_kill_queues(struct sock *sk);

extern int sk_wait_data(struct sock *sk, long *timeo);

#define SK_STREAM_MEM_QUANTUM ((int)PAGE_SIZE)

#define SOCK_MIN_SNDBUF 2048
#define SOCK_MIN_RCVBUF 256

static inline void sk_stream_moderate_sndbuf(struct sock *sk)
{
	if (!(sk->sk_userlocks & SOCK_SNDBUF_LOCK)) {
		sk->sk_sndbuf = min(sk->sk_sndbuf, sk->sk_wmem_queued / 2);
		sk->sk_sndbuf = max(sk->sk_sndbuf, SOCK_MIN_SNDBUF);
	}
}

static inline struct page *sk_stream_alloc_page(struct sock *sk)
{
	struct page *page = NULL;

	if (sk->sk_forward_alloc >= (int)PAGE_SIZE ||
	    sk_stream_mem_schedule(sk, PAGE_SIZE, 0))
		page = alloc_pages(sk->sk_allocation, 0);
	else {
		((struct proto_sles9 *)sk->sk_prot)->enter_memory_pressure();
		sk_stream_moderate_sndbuf(sk);
	}
	return page;
}

static inline struct sk_buff *sk_stream_alloc_pskb(struct sock *sk,
						   int size, int mem, int gfp)
{
	struct sk_buff *skb = alloc_skb(size + ((struct proto_sles9 *)sk->sk_prot)->max_header, gfp);

	if (skb) {
		skb->truesize += mem;
		if (sk->sk_forward_alloc >= (int)skb->truesize ||
		    sk_stream_mem_schedule(sk, skb->truesize, 0)) {
			skb_reserve(skb, ((struct proto_sles9 *)sk->sk_prot)->max_header);
			return skb;
		}
		__kfree_skb(skb);
	} else {
		((struct proto_sles9 *)sk->sk_prot)->enter_memory_pressure();
		sk_stream_moderate_sndbuf(sk);
	}
	return NULL;
}

static inline struct sk_buff *sk_stream_alloc_skb(struct sock *sk,
						  int size, int gfp)
{
	return sk_stream_alloc_pskb(sk, size, 0, gfp);
}

static inline int sk_stream_pages(int amt)
{
	return (amt + SK_STREAM_MEM_QUANTUM - 1) / SK_STREAM_MEM_QUANTUM;
}

static inline void sk_stream_mem_reclaim(struct sock *sk)
{
	if (sk->sk_forward_alloc >= SK_STREAM_MEM_QUANTUM)
		__sk_stream_mem_reclaim(sk);
}

static inline int sk_stream_rmem_schedule(struct sock *sk, struct sk_buff *skb)
{
	return (int)skb->truesize <= sk->sk_forward_alloc ||
		sk_stream_mem_schedule(sk, skb->truesize, 1);
}

static inline int sk_stream_wmem_schedule(struct sock *sk, int size)
{
        return size <= sk->sk_forward_alloc ||
               sk_stream_mem_schedule(sk, size, 0);
}

/**
 * sk_eat_skb - Release a skb if it is no longer needed
 * @sk - socket to eat this skb from
 * @skb - socket buffer to eat
 *
 * This routine must be called with interrupts disabled or with the socket
 * locked so that the sk_buff queue operation is ok.
*/
static inline void sk_eat_skb(struct sock *sk, struct sk_buff *skb)
{
	__skb_unlink(skb, &sk->sk_receive_queue);
	__kfree_skb(skb);
}

static inline
struct sock *sk_clone(const struct sock *sk, const gfp_t priority)
{
	struct sock *newsk = sk_alloc_new(sk->sk_family, priority, (struct proto_sles9 *) sk->sk_prot, 0);

	if (newsk != NULL) {
		struct sk_filter *filter;

		memcpy(newsk, sk, ((struct proto_sles9 *)sk->sk_prot)->obj_size);

		/* SANITY */
		sk_node_init(&newsk->sk_node);
		sock_lock_init(newsk);
		bh_lock_sock(newsk);

		atomic_set(&newsk->sk_rmem_alloc, 0);
		atomic_set(&newsk->sk_wmem_alloc, 0);
		atomic_set(&newsk->sk_omem_alloc, 0);
		skb_queue_head_init(&newsk->sk_receive_queue);
		skb_queue_head_init(&newsk->sk_write_queue);

		rwlock_init(&newsk->sk_dst_lock);
		rwlock_init(&newsk->sk_callback_lock);

		newsk->sk_dst_cache	= NULL;
		newsk->sk_wmem_queued	= 0;
		newsk->sk_forward_alloc = 0;
#if 0
		newsk->sk_send_head	= NULL;
#endif
		newsk->sk_backlog.head	= newsk->sk_backlog.tail = NULL;
		newsk->sk_userlocks	= sk->sk_userlocks & ~SOCK_BINDPORT_LOCK;

		sock_reset_flag(newsk, SOCK_DONE);
		skb_queue_head_init(&newsk->sk_error_queue);

		filter = newsk->sk_filter;
		if (filter != NULL)
			sk_filter_charge(newsk, filter);


		newsk->sk_err	   = 0;
		newsk->sk_priority = 0;
		atomic_set(&newsk->sk_refcnt, 2);

		/*
		 * Increment the counter in the same struct proto as the master
		 * sock (sk_refcnt_debug_inc uses newsk->sk_prot->socks, that
		 * is the same as sk->sk_prot->socks, as this field was copied
		 * with memcpy).
		 *
		 * This _changes_ the previous behaviour, where
		 * tcp_create_openreq_child always was incrementing the
		 * equivalent to tcp_prot->socks (inet_sock_nr), so this have
		 * to be taken into account in all callers. -acme
		 */
		sk_refcnt_debug_inc(newsk);
		newsk->sk_socket = NULL;
		newsk->sk_sleep	 = NULL;

#if 0
		if (newsk->sk_prot->sockets_allocated)
			atomic_inc(newsk->sk_prot->sockets_allocated);
#endif
	}
	return newsk;
}
#endif
