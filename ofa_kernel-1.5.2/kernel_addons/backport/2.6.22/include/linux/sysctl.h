#ifndef __BACKPORT_SYSCTL_H_TO_2_6_22__
#define __BACKPORT_SYSCTL_H_TO_2_6_22__

#include <linux/slab.h>
#include <linux/err.h>
#include_next <linux/sysctl.h>

/* struct ctl_path describes where in the hierarchy a table is added */
struct ctl_path {
	const char *procname;
	int ctl_name;
};

#define FAKE_SYSCTL_MAGIC1	((void *) 0xcafebabe)

static inline void __fake_sysctl_table_destroy(struct ctl_table *node)
{
	struct ctl_table *next;

	while (node && node[1].extra1 == FAKE_SYSCTL_MAGIC1) {
		next = node->child;
		kfree(node);
		node = next;
	}
}

/*
 * Given a ctl_path and a ctl_table, convert this to the old-fashioned
 * table hierarchy, linked through table->child.
 */
static inline struct ctl_table_header *
register_sysctl_paths(const struct ctl_path *path, struct ctl_table *table)
{
	struct ctl_table_header *result = NULL;
	struct ctl_table *root = NULL, *tp, **prev = &root;

	for (; path->procname; ++path) {
		tp = kzalloc(2 * sizeof(struct ctl_table), GFP_KERNEL);
		if (!tp)
			goto out;

		tp->ctl_name = path->ctl_name;
		tp->procname = path->procname;
		tp->mode = 0555;
		tp[1].extra1 = FAKE_SYSCTL_MAGIC1;
		*prev = tp;
		prev = &tp->child;
	}
	*prev = table;

	result = register_sysctl_table(root);

out:
	if (result == NULL)
		__fake_sysctl_table_destroy(root);

	return result;
}

static inline void
fake_unregister_sysctl_table(struct ctl_table_header *hdr)
{
	struct ctl_table *node = hdr->ctl_table;

	unregister_sysctl_table(hdr);
	__fake_sysctl_table_destroy(node);
}

#define unregister_sysctl_table(hdr)	fake_unregister_sysctl_table(hdr)

#endif /* __BACKPORT_SYSCTL_H_TO_2_6_22__ */
