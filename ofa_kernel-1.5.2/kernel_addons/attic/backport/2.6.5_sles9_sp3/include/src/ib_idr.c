#ifndef TEST                        // to test in user space...
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>
#endif
#include <linux/string.h>
#include <linux/idr.h>

static struct idr_layer *alloc_layer(struct idr *idp)
{
       struct idr_layer *p;

       spin_lock(&idp->lock);
       if ((p = idp->id_free)) {
               idp->id_free = p->ary[0];
               idp->id_free_cnt--;
               p->ary[0] = NULL;
       }
       spin_unlock(&idp->lock);
       return(p);
}

static void free_layer(struct idr *idp, struct idr_layer *p)
{
       /*
        * Depends on the return element being zeroed.
        */
       spin_lock(&idp->lock);
       p->ary[0] = idp->id_free;
       idp->id_free = p;
       idp->id_free_cnt++;
       spin_unlock(&idp->lock);
}

static int sub_alloc(struct idr *idp, void *ptr, int *starting_id)
{
       int n, m, sh;
       struct idr_layer *p, *new;
       struct idr_layer *pa[MAX_LEVEL];
       int l, id;
       long bm;

       id = *starting_id;
       p = idp->top;
       l = idp->layers;
       pa[l--] = NULL;
       while (1) {
               /*
                * We run around this while until we reach the leaf node...
                */
               n = (id >> (IDR_BITS*l)) & IDR_MASK;
               bm = ~p->bitmap;
               m = find_next_bit(&bm, IDR_SIZE, n);
               if (m == IDR_SIZE) {
                       /* no space available go back to previous layer. */
                       l++;
                       id = (id | ((1 << (IDR_BITS * l)) - 1)) + 1;
                       if (!(p = pa[l])) {
                               *starting_id = id;
                               return -2;
                       }
                       continue;
               }
               if (m != n) {
                       sh = IDR_BITS*l;
                       id = ((id >> sh) ^ n ^ m) << sh;
               }
               if ((id >= MAX_ID_BIT) || (id < 0))
                       return -3;
               if (l == 0)
                       break;
               /*
                * Create the layer below if it is missing.
                */
               if (!p->ary[m]) {
                       if (!(new = alloc_layer(idp)))
                               return -1;
                       p->ary[m] = new;
                       p->count++;
               }
               pa[l--] = p;
               p = p->ary[m];
       }
       /*
        * We have reached the leaf node, plant the
        * users pointer and return the raw id.
        */
       p->ary[m] = (struct idr_layer *)ptr;
       __set_bit(m, &p->bitmap);
       p->count++;
       /*
        * If this layer is full mark the bit in the layer above
        * to show that this part of the radix tree is full.
        * This may complete the layer above and require walking
        * up the radix tree.
        */
       n = id;
       while (p->bitmap == IDR_FULL) {
               if (!(p = pa[++l]))
                       break;
               n = n >> IDR_BITS;
               __set_bit((n & IDR_MASK), &p->bitmap);
       }
       return(id);
}

int ib_idr_get_new_above_int(struct idr *idp, void *ptr, int starting_id)
{
       struct idr_layer *p, *new;
       int layers, v, id;

       id = starting_id;
build_up:
       p = idp->top;
       layers = idp->layers;
       if (unlikely(!p)) {
               if (!(p = alloc_layer(idp)))
                       return -1;
               layers = 1;
       }
       /*
        * Add a new layer to the top of the tree if the requested
        * id is larger than the currently allocated space.
        */
       while ((layers < (MAX_LEVEL - 1)) && (id >= (1 << (layers*IDR_BITS)))) {
               layers++;
               if (!p->count)
                       continue;
               if (!(new = alloc_layer(idp))) {
                       /*
                        * The allocation failed.  If we built part of
                        * the structure tear it down.
                        */
                       for (new = p; p && p != idp->top; new = p) {
                               p = p->ary[0];
                               new->ary[0] = NULL;
                               new->bitmap = new->count = 0;
                               free_layer(idp, new);
                       }
                       return -1;
               }
               new->ary[0] = p;
               new->count = 1;
               if (p->bitmap == IDR_FULL)
                       __set_bit(0, &new->bitmap);
               p = new;
       }
       idp->top = p;
       idp->layers = layers;
       v = sub_alloc(idp, ptr, &id);
       if (v == -2)
               goto build_up;
       return(v);
}
EXPORT_SYMBOL(ib_idr_get_new_above_int);

/**
 * idr_get_new_above - allocate new idr entry above or equal to a start id
 * @idp: idr handle
 * @ptr: pointer you want associated with the ide
 * @start_id: id to start search at
 * @id: pointer to the allocated handle
 *
 * This is the allocate id function.  It should be called with any
 * required locks.
 *
 * If memory is required, it will return -EAGAIN, you should unlock
 * and go back to the idr_pre_get() call.  If the idr is full, it will
 * return -ENOSPC.
 *
 * @id returns a value in the range 0 ... 0x7fffffff
 */
int ib_idr_get_new_above(struct idr *idp, void *ptr, int starting_id, int *id)
{
       int rv;

       rv = idr_get_new_above_int(idp, ptr, starting_id);
       /*
        * This is a cheap hack until the IDR code can be fixed to
        * return proper error values.
        */
       if (rv < 0) {
               if (rv == -1)
                       return -EAGAIN;
               else /* Will be -3 */
                       return -ENOSPC;
       }
       *id = rv;
       return 0;
}
EXPORT_SYMBOL(ib_idr_get_new_above);
