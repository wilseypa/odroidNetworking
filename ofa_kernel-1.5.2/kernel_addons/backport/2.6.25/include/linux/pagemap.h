#ifndef BACKPORT_LINUX_PAGEMAP_H
#define BACKPORT_LINUX_PAGEMAP_H

#include_next <linux/pagemap.h>

static inline struct page *grab_cache_page_write_begin(struct address_space *mapping, pgoff_t index, unsigned flags)
{
	return __grab_cache_page(mapping, index);
}

#endif
