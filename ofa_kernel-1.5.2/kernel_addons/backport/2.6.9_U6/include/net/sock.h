#ifndef NET_SOCK_H_BACKPORT
#define NET_SOCK_H_BACKPORT

#include_next <net/sock.h>

static inline
int proto_register(struct proto *prot, int alloc_slab)
{
	int rc = -ENOBUFS;

	if (alloc_slab) {
		prot->slab = kmem_cache_create(prot->name, prot->slab_obj_size, 0,
					       SLAB_HWCACHE_ALIGN, NULL);

		if (prot->slab == NULL) {
			printk(KERN_CRIT "%s: Can't create sock SLAB cache!\n",
			       prot->name);
			goto out;
		}
	}
	rc = 0;
out:
	return rc;
}

static inline
void proto_unregister(struct proto *prot)
{
	if (prot->slab != NULL) {
		kmem_cache_destroy(prot->slab);
		prot->slab = NULL;
	}
}

static inline struct sock *sk_alloc_new(int family, gfp_t priority,
					struct proto *prot, int zero_it)
{
	struct sock *sk = sk_alloc(family, priority,
				   zero_it ? prot->slab_obj_size : 0,
				   prot->slab);
	if (!sk)
		return NULL;
	sk->sk_prot = prot;
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

static inline int sk_stream_wmem_schedule(struct sock *sk, int size)
{
        return size <= sk->sk_forward_alloc ||
               sk_stream_mem_schedule(sk, size, 0);
}

static inline
struct sock *sk_clone(const struct sock *sk, const gfp_t priority)
{
	struct sock *newsk = sk_alloc(sk->sk_family, priority, sk->sk_prot, 0);

	if (newsk != NULL) {
		struct sk_filter *filter;

		memcpy(newsk, sk, sk->sk_prot->slab_obj_size);

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
		newsk->sk_send_head	= NULL;
		newsk->sk_backlog.head	= newsk->sk_backlog.tail = NULL;
		newsk->sk_userlocks	= sk->sk_userlocks & ~SOCK_BINDPORT_LOCK;

		sock_reset_flag(newsk, SOCK_DONE);
		skb_queue_head_init(&newsk->sk_error_queue);

		filter = newsk->sk_filter;
		if (filter != NULL)
			sk_filter_charge(newsk, filter);

#if 0
		if (unlikely(xfrm_sk_clone_policy(newsk))) {
			/* It is still raw copy of parent, so invalidate
			 * destructor and make plain sk_free() */
			newsk->sk_destruct = NULL;
			sk_free(newsk);
			newsk = NULL;
			goto out;
		}
#endif
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

		if (newsk->sk_prot->sockets_allocated)
			atomic_inc(newsk->sk_prot->sockets_allocated);
	}
#if 0
out:
#endif
	return newsk;
}

#define sk_eat_skb(a, b, c) sk_eat_skb(a, b)

#endif

#ifndef _NET_SOCK_LOCKDEP_BACKPORT_H
#define _NET_SOCK_LOCKDEP_BACKPORT_H

#define lock_sock_nested(_sk, _subclass) lock_sock(sk)

#endif
