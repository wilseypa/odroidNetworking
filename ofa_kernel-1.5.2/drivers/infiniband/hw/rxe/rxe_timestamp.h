#include <linux/spinlock.h>
#include <linux/time.h>
#include <linux/smp.h>

#define TS_SIZE			(32)

struct ts {
	struct ts		*next;
	int			init;
	int			i;
	struct timespec		ts[TS_SIZE];
	int			num[TS_SIZE];
	int			cpu[TS_SIZE];
	int			line;
	char			func[64];
	char			file[64];
};

extern spinlock_t ts_lock;
extern struct ts *ts_list;
extern struct timespec ts_start;

void timestamp(struct ts *t, int num, char *file, char *func, int line);
void ts_init(void);
void ts_dump_list(void);

#define TS(num) { \
	static struct ts t; \
	timestamp(&t, num, __FILE__, __func__, __LINE__); \
}
