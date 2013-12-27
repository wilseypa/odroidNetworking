#ifndef BACKPORT_LINUX_CRYPTO_H
#define BACKPORT_LINUX_CRYPTO_H

#include_next <linux/crypto.h>

#define CRYPTO_ALG_ASYNC               0x00000080

struct hash_desc
{
	struct crypto_tfm *tfm;
	u32 flags;
};

struct crypto_blkcipher {
	struct crypto_tfm base;
};

struct blkcipher_desc {
	struct crypto_blkcipher *tfm;
	void *info;
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

#define crypto_hash_digestsize(tfm) crypto_tfm_alg_digestsize(tfm)
#define crypto_hash_setkey(a,b,c) crypto_digest_setkey(a,b,c)

static inline struct crypto_tfm *crypto_blkcipher_tfm(
	struct crypto_blkcipher *tfm)
{
	return &tfm->base;
}

static inline unsigned int crypto_blkcipher_blocksize(
	struct crypto_blkcipher *tfm)
{
	return crypto_tfm_alg_blocksize(crypto_blkcipher_tfm(tfm));
}

static inline unsigned int crypto_blkcipher_ivsize(struct crypto_blkcipher *tfm)
{
	return crypto_tfm_alg_ivsize(crypto_blkcipher_tfm(tfm));
}

static inline int crypto_blkcipher_encrypt_iv(struct blkcipher_desc *desc,
					      struct scatterlist *dst,
					      struct scatterlist *src,
					      unsigned int nbytes)
{
	return crypto_cipher_encrypt_iv(crypto_blkcipher_tfm(desc->tfm), dst, src, nbytes, (u8 *)desc->info); 
}

static inline int crypto_blkcipher_decrypt_iv(struct blkcipher_desc *desc,
					      struct scatterlist *dst,
					      struct scatterlist *src,
					      unsigned int nbytes)
{
	return crypto_cipher_decrypt_iv(crypto_blkcipher_tfm(desc->tfm), dst, src, nbytes, (u8 *)desc->info);
}

static inline struct crypto_blkcipher *crypto_alloc_blkcipher(
	const char *alg_name, u32 type, u32 mask)
{
	return (struct crypto_blkcipher *)crypto_alloc_tfm(alg_name, mask);
}

static inline void crypto_free_blkcipher(struct crypto_blkcipher *tfm)
{
	crypto_free_tfm(crypto_blkcipher_tfm(tfm));
}

static inline int crypto_blkcipher_setkey(struct crypto_blkcipher *tfm,
					  const u8 *key, unsigned int keylen)
{
	return crypto_cipher_setkey(crypto_blkcipher_tfm(tfm), key, keylen);
}

#endif
