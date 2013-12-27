#ifndef BACKPORT_LINUX_CRYPTO_H
#define BACKPORT_LINUX_CRYPTO_H

#include_next <linux/crypto.h>

#define CRYPTO_ALG_ASYNC               0x00000080

struct hash_desc
{
	struct crypto_tfm *tfm;
	u32 flags;
};

static inline int crypto_hash_init(struct hash_desc *desc)
{
	crypto_digest_init(desc->tfm);
	return 0;
}

static inline int crypto_hash_digest(struct hash_desc *desc,
                                    struct scatterlist *sg,
                                    unsigned int nbytes, u8 *out)
{
	crypto_digest_digest(desc->tfm, sg, 1, out);
	return nbytes;
}

static inline int crypto_hash_update(struct hash_desc *desc,
                                    struct scatterlist *sg,
                                    unsigned int nbytes)
{
	crypto_digest_update(desc->tfm, sg, 1);
	return nbytes;
}

static inline int crypto_hash_final(struct hash_desc *desc, u8 *out)
{
	crypto_digest_final(desc->tfm, out);
	return 0;
}

static inline struct crypto_tfm *crypto_alloc_hash(const char *alg_name,
                                                   u32 type, u32 mask)
{
	struct crypto_tfm *ret = crypto_alloc_tfm(alg_name ,type);
	return ret ? ret : ERR_PTR(-ENOMEM);
}

static inline void crypto_free_hash(struct crypto_tfm *tfm)
{
	crypto_free_tfm(tfm);
}

#endif
