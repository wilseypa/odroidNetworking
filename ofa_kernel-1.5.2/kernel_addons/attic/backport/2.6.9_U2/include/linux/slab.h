#ifndef LINUX_SLAB_BACKPORT_H
#define LINUX_SLAB_BACKPORT_H

#include_next <linux/slab.h>

/* kzalloc (introduced in 2.6.14) */
static inline void *kzalloc(size_t size, gfp_t flags)
{
	void *ret = kmalloc(size, flags);
	if (ret)
		memset(ret, 0, size);
	return ret;
}

static inline char *kstrdup(const char *buf, gfp_t flags)
{
	int len = strlen(buf) + 1;
	char *s = kmalloc(len, flags);
	if (!s)
		return NULL;
	memcpy(s, buf, len);
	return s;
}

static inline
void *kmemdup(const void *src, size_t len, gfp_t gfp)
{
       void *p;

       p = kmalloc(len, gfp);
       if (p)
               memcpy(p, src, len);
       return p;
}

static inline
void *kmalloc_node(size_t size, gfp_t flags, int nid)
{
	return kmalloc(size, flags);
}

#define kmem_cache kmem_cache_s

#endif
#ifndef LINUX_SLAB_BACKPORT_tO_2_6_22_H
#define LINUX_SLAB_BACKPORT_tO_2_6_22_H

#include_next <linux/slab.h>

static inline
struct kmem_cache *
kmem_cache_create_for_2_6_22 (const char *name, size_t size, size_t align,
			      unsigned long flags,
			      void (*ctor)(void*, struct kmem_cache *, unsigned long)
			      )
{
	return kmem_cache_create(name, size, align, flags, ctor, NULL);
}

#define kmem_cache_create kmem_cache_create_for_2_6_22

#endif
#ifndef SLAB_H_KMEMCACHE_ZALLOC_BACKPORT_TO_2_6_16
#define SLAB_H_KMEMCACHE_ZALLOC_BACKPORT_TO_2_6_16

static inline
void *kmem_cache_zalloc(struct kmem_cache *cache, gfp_t flags)
{
       void *ret = kmem_cache_alloc(cache, flags);
       if (ret)
	       memset(ret, 0, kmem_cache_size(cache));

       return ret;
}

#endif

