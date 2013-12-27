#ifndef LINUX_SKBUFF_H_BACKPORT
#define LINUX_SKBUFF_H_BACKPORT

#include_next <linux/skbuff.h>

static inline struct sk_buff *netdev_alloc_skb(struct net_device *dev,
		unsigned int length)
{
	struct sk_buff *skb;
	skb = dev_alloc_skb(length);
	if (likely(skb))
		skb->dev = dev;

	return skb;
}

#define CHECKSUM_PARTIAL CHECKSUM_HW 
#define CHECKSUM_COMPLETE CHECKSUM_HW 

#define gso_size tso_size

static inline int backport_skb_linearize_to_2_6_17(struct sk_buff *skb)
{
	return skb_linearize(skb, GFP_ATOMIC);
}

#define skb_linearize backport_skb_linearize_to_2_6_17

#endif
#ifndef __BACKPORT_LINUX_SKBUFF_H_TO_2_6_21__
#define __BACKPORT_LINUX_SKBUFF_H_TO_2_6_21__

#include_next <linux/skbuff.h>

#define transport_header h.raw
#define network_header nh.raw
#define mac_header mac.raw

static inline void skb_reset_mac_header(struct sk_buff *skb)
{
	skb->mac.raw = skb->data;
}

static inline void skb_reset_network_header(struct sk_buff *skb)
{
	skb->network_header = skb->data;
}

static inline void skb_copy_from_linear_data(const struct sk_buff *skb,
					     void *to,
					     const unsigned int len)
{
	memcpy(to, skb->data, len);
}

static inline void skb_copy_to_linear_data(struct sk_buff *skb,
                                           const void *from,
                                           const unsigned int len)
{
        memcpy(skb->data, from, len);
}


static inline unsigned char *skb_end_pointer(const struct sk_buff *skb)
{
	return skb->end;
}

static inline unsigned char *skb_transport_header(const struct sk_buff *skb)
{
	return skb->transport_header;
}

static inline unsigned char *skb_network_header(const struct sk_buff *skb)
{
	return skb->network_header;
}

static inline void skb_reset_transport_header(struct sk_buff *skb)
{
	skb->transport_header = skb->data;
}

static inline int skb_transport_offset(const struct sk_buff *skb)
{
	return skb_transport_header(skb) - skb->data;
}

static inline int skb_network_offset(const struct sk_buff *skb)
{
	return skb_network_header(skb) - skb->data;
}

static inline void skb_set_mac_header(struct sk_buff *skb, const int offset)
{
	skb->mac_header = skb->data + offset;
}

static inline void skb_set_network_header(struct sk_buff *skb, const int offset)
{
	skb->network_header = skb->data + offset;
}

static inline void skb_set_transport_header(struct sk_buff *skb,
					    const int offset)
{
	skb->transport_header = skb->data + offset;
}


static inline int skb_is_gso(const struct sk_buff *skb)
{
	return skb_shinfo(skb)->tso_size;
}

static inline void skb_copy_from_linear_data_offset(const struct sk_buff *skb,
                                                    const int offset, void *to,
                                                    const unsigned int len)
{
        memcpy(to, skb->data + offset, len);
}

#endif
