	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_EVENTS_H
#define FPM_EVENTS_H 1

#define FPM_EV_TIMEOUT  (1 << 0)
#define FPM_EV_READ     (1 << 1)
#define FPM_EV_PERSIST  (1 << 2)
#define FPM_EV_EDGE     (1 << 3)

#define fpm_event_set_timer(ev, flags, cb, arg) fpm_event_set((ev), -1, (flags), (cb), (arg))

struct fpm_event_s {
	int fd;                   /* not set with FPM_EV_TIMEOUT */
	struct timeval timeout;   /* next time to trigger */
	struct timeval frequency;
	void (*callback)(struct fpm_event_s *, short, void *);
	void *arg;
	int flags;
	int index;                /* index of the fd in the ufds array */
	short which;              /* type of event */
};

typedef struct fpm_event_queue_s {
	struct fpm_event_queue_s *prev;
	struct fpm_event_queue_s *next;
	struct fpm_event_s *ev;
} fpm_event_queue;

struct fpm_event_module_s {
	const char *name;
	int support_edge_trigger;
	int (*init)(int max_fd);
	int (*clean)(void);
	int (*wait)(struct fpm_event_queue_s *queue, unsigned long int timeout);
	int (*add)(struct fpm_event_s *ev);
	int (*remove)(struct fpm_event_s *ev);
};

void fpm_event_loop(int err);
void fpm_event_fire(struct fpm_event_s *ev);
int fpm_event_init_main();
int fpm_event_set(struct fpm_event_s *ev, int fd, int flags, void (*callback)(struct fpm_event_s *, short, void *), void *arg);
int fpm_event_add(struct fpm_event_s *ev, unsigned long int timeout);
int fpm_event_del(struct fpm_event_s *ev);
int fpm_event_pre_init(char *machanism);
const char *fpm_event_machanism_name();
int fpm_event_support_edge_trigger();

#endif
