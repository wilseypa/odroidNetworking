#ifndef BACKPORT_LINUX_KREF_TO_2_6_9
#define BACKPORT_LINUX_KREF_TO_2_6_9

#include <linux/slab.h>
#include_next <linux/kref.h>

static inline int
backport_kref_put(struct kref *kref, void (*release)(struct kref *kref))
{
        WARN_ON(release == NULL);
        WARN_ON(release == (void (*)(struct kref *))kfree);

        if (atomic_dec_and_test(&kref->refcount)) {
                release(kref);
                return 1;
        }
        return 0;
}

#undef kref_put
#define kref_put(kref, release) backport_kref_put(kref, release)

#endif
