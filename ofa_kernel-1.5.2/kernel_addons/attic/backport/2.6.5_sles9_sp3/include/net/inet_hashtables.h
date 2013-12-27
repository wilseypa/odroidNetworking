#ifndef NET_INET_HASHTABLES_H_BACKPORT
#define NET_INET_HASHTABLES_H_BACKPORT

static inline unsigned int inet_ehashfn(const __u32 laddr, const __u16 lport,
                                        const __u32 faddr, const __u16 fport)
{
        unsigned int h = (laddr ^ lport) ^ (faddr ^ fport);
        h ^= h >> 16;
        h ^= h >> 8;
        return h;
}

#endif /* NET_INET_HASHTABLES_H_BACKPORT */
