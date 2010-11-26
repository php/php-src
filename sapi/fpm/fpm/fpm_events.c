
	/* $Id: fpm_events.c,v 1.21.2.2 2008/12/13 03:21:18 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <unistd.h>
#include <errno.h>
#include <stdlib.h> /* for putenv */
#include <string.h>

#include <php.h>
#include <php_network.h>

#include "fpm.h"
#include "fpm_process_ctl.h"
#include "fpm_events.h"
#include "fpm_cleanup.h"
#include "fpm_stdio.h"
#include "fpm_signals.h"
#include "fpm_children.h"
#include "zlog.h"
#include "fpm_clock.h"

#define fpm_event_set_timeout(ev, now) timeradd(&(now), &(ev)->frequency, &(ev)->timeout);

typedef struct fpm_event_queue_s {
	struct fpm_event_queue_s *prev;
	struct fpm_event_queue_s *next;
	struct fpm_event_s *ev;
} fpm_event_queue;

static void fpm_event_cleanup(int which, void *arg);
static void fpm_got_signal(struct fpm_event_s *ev, short which, void *arg);
static struct fpm_event_s *fpm_event_queue_isset(struct fpm_event_queue_s *queue, struct fpm_event_s *ev);
static int fpm_event_queue_add(struct fpm_event_queue_s **queue, struct fpm_event_s *ev);
static int fpm_event_queue_del(struct fpm_event_queue_s **queue, struct fpm_event_s *ev);
static void fpm_event_queue_destroy(struct fpm_event_queue_s **queue);

static int fpm_event_nfds_max;
static struct fpm_event_queue_s *fpm_event_queue_timer = NULL;
static struct fpm_event_queue_s *fpm_event_queue_fd = NULL;
static php_pollfd *fpm_event_ufds = NULL;

static void fpm_event_cleanup(int which, void *arg) /* {{{ */
{
	if (fpm_event_ufds) {
		free(fpm_event_ufds);
	}
	fpm_event_queue_destroy(&fpm_event_queue_timer);
	fpm_event_queue_destroy(&fpm_event_queue_fd);
}
/* }}} */

static void fpm_got_signal(struct fpm_event_s *ev, short which, void *arg) /* {{{ */
{
	char c;
	int res;
	int fd = ev->fd;;

	do {
		do {
			res = read(fd, &c, 1);
		} while (res == -1 && errno == EINTR);

		if (res <= 0) {
			if (res < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
				zlog(ZLOG_SYSERROR, "read() failed");
			}
			return;
		}

		switch (c) {
			case 'C' :                  /* SIGCHLD */
				zlog(ZLOG_DEBUG, "received SIGCHLD");
				fpm_children_bury();
				break;
			case 'I' :                  /* SIGINT  */
				zlog(ZLOG_DEBUG, "received SIGINT");
				zlog(ZLOG_NOTICE, "Terminating ...");
				fpm_pctl(FPM_PCTL_STATE_TERMINATING, FPM_PCTL_ACTION_SET);
				break;
			case 'T' :                  /* SIGTERM */
				zlog(ZLOG_DEBUG, "received SIGTERM");
				zlog(ZLOG_NOTICE, "Terminating ...");
				fpm_pctl(FPM_PCTL_STATE_TERMINATING, FPM_PCTL_ACTION_SET);
				break;
			case 'Q' :                  /* SIGQUIT */
				zlog(ZLOG_DEBUG, "received SIGQUIT");
				zlog(ZLOG_NOTICE, "Finishing ...");
				fpm_pctl(FPM_PCTL_STATE_FINISHING, FPM_PCTL_ACTION_SET);
				break;
			case '1' :                  /* SIGUSR1 */
				zlog(ZLOG_DEBUG, "received SIGUSR1");
				if (0 == fpm_stdio_open_error_log(1)) {
					zlog(ZLOG_NOTICE, "log file re-opened");
				} else {
					zlog(ZLOG_ERROR, "unable to re-opened log file");
				}
				break;
			case '2' :                  /* SIGUSR2 */
				zlog(ZLOG_DEBUG, "received SIGUSR2");
				zlog(ZLOG_NOTICE, "Reloading in progress ...");
				fpm_pctl(FPM_PCTL_STATE_RELOADING, FPM_PCTL_ACTION_SET);
				break;
		}

		if (fpm_globals.is_child) {
			break;
		}
	} while (1);
	return;
}
/* }}} */

static struct fpm_event_s *fpm_event_queue_isset(struct fpm_event_queue_s *queue, struct fpm_event_s *ev) /* {{{ */
{
	if (!ev) {
		return NULL;
	}

	while (queue) {
		if (queue->ev == ev) {
			return ev;
		}
		queue = queue->next;
	}

	return NULL;
}
/* }}} */

static int fpm_event_queue_add(struct fpm_event_queue_s **queue, struct fpm_event_s *ev) /* {{{ */
{
	struct fpm_event_queue_s *elt;

	if (!queue || !ev) {
		return -1;
	}

	if (fpm_event_queue_isset(*queue, ev)) {
		return 0;
	}

	if (!(elt = malloc(sizeof(struct fpm_event_queue_s)))) {
		zlog(ZLOG_SYSERROR, "malloc() failed");
		return -1;
	}
	elt->prev = NULL;
	elt->next = NULL;
	elt->ev = ev;

	if (*queue) {
		(*queue)->prev = elt;
		elt->next = *queue;
	}
	*queue = elt;

	return 0;	
}
/* }}} */

static int fpm_event_queue_del(struct fpm_event_queue_s **queue, struct fpm_event_s *ev) /* {{{ */
{
	struct fpm_event_queue_s *q;
	if (!queue || !ev) {
		return -1;
	}
	q = *queue;
	while (q) {
		if (q->ev == ev) {
			if (q->prev) {
				q->prev->next = q->next;
			}
			if (q->next) {
				q->next->prev = q->prev;
			}
			if (q == *queue) {
				*queue = q->next;
				(*queue)->prev = NULL;
			}
			free(q);
			return 0;
		}
		q = q->next;
	}
	return -1;
}
/* }}} */

static void fpm_event_queue_destroy(struct fpm_event_queue_s **queue) /* {{{ */
{
	struct fpm_event_queue_s *q, *tmp;

	if (!queue) {
		return;
	}
	q = *queue;
	while (q) {
		tmp = q;
		q = q->next;
		/* q->prev = NULL */
		free(tmp);
	}
	*queue = NULL;
}
/* }}} */

int fpm_event_init_main() /* {{{ */
{
	struct fpm_worker_pool_s *wp;

	/* count the max number of necessary fds for polling */
	fpm_event_nfds_max = 1; /* only one FD is necessary at startup for the master process signal pipe */
	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		if (!wp->config) continue;
		if (wp->config->catch_workers_output && wp->config->pm_max_children > 0) {
			fpm_event_nfds_max += (wp->config->pm_max_children * 2);
		}
	}

	/* malloc the max number of necessary fds for polling */
	fpm_event_ufds = malloc(sizeof(php_pollfd) * fpm_event_nfds_max);
	if (!fpm_event_ufds) {
		zlog(ZLOG_SYSERROR, "malloc() failed");
		return -1;
	}

	zlog(ZLOG_DEBUG, "%d fds have been reserved", fpm_event_nfds_max);

	if (0 > fpm_cleanup_add(FPM_CLEANUP_ALL, fpm_event_cleanup, NULL)) {
		return -1;
	}
	return 0;
}
/* }}} */

void fpm_event_loop() /* {{{ */
{
	static struct fpm_event_s signal_fd_event;

	/* sanity check */
	if (fpm_globals.parent_pid != getpid()) {
		return;
	}

	fpm_event_set(&signal_fd_event, fpm_signals_get_fd(), FPM_EV_READ, &fpm_got_signal, NULL);
	fpm_event_add(&signal_fd_event, 0);

	/* add timers */
	fpm_pctl_heartbeat(NULL, 0, NULL);
	fpm_pctl_perform_idle_server_maintenance_heartbeat(NULL, 0, NULL);

	zlog(ZLOG_NOTICE, "ready to handle connections");

	while (1) {
		struct fpm_event_queue_s *q, *q2;
		struct timeval ms;
		struct timeval tmp;
		struct timeval now;
		unsigned long int timeout;
		int i, ret;

		/* sanity check */
		if (fpm_globals.parent_pid != getpid()) {
			return;
		}

		fpm_clock_get(&now);
		timerclear(&ms);

		/* search in the timeout queue for the next timer to trigger */
		q = fpm_event_queue_timer;
		while (q) {
			if (!timerisset(&ms)) {
				ms = q->ev->timeout;
			} else {
				if (timercmp(&q->ev->timeout, &ms, <)) {
					ms = q->ev->timeout;
				}
			}
			q = q->next;
		}

		/* 1s timeout if none has been set */
		if (!timerisset(&ms) || timercmp(&ms, &now, <) || timercmp(&ms, &now, ==)) {
			timeout = 1000;
		} else {
			timersub(&ms, &now, &tmp);
			timeout = (tmp.tv_sec * 1000) + (tmp.tv_usec / 1000) + 1;
		}

		/* init fpm_event_ufds for php_poll2 */
		memset(fpm_event_ufds, 0, sizeof(php_pollfd) * fpm_event_nfds_max);
		i = 0;
		q = fpm_event_queue_fd;
		while (q && i < fpm_event_nfds_max) {
			fpm_event_ufds[i].fd = q->ev->fd;
			fpm_event_ufds[i].events = POLLIN;
			q->ev->index = i++;
			q = q->next;
		}

		/* wait for inconming event or timeout */
		if ((ret = php_poll2(fpm_event_ufds, i, timeout)) == -1) {
			if (errno != EINTR) {
				zlog(ZLOG_WARNING, "php_poll2() returns %d", errno);
			}
		} else if (ret > 0) {

			/* trigger POLLIN events */
			q = fpm_event_queue_fd;
			while (q) {
				if (q->ev && q->ev->index >= 0 && q->ev->index < fpm_event_nfds_max) {
					if (q->ev->fd == fpm_event_ufds[q->ev->index].fd) {
						if (fpm_event_ufds[q->ev->index].revents & POLLIN) {
							fpm_event_fire(q->ev);
							/* sanity check */
							if (fpm_globals.parent_pid != getpid()) {
								return;
							}
						}
					}
					q->ev->index = -1;
				}
				q = q->next;
			}
		}

		/* trigger timers */
		q = fpm_event_queue_timer;
		while (q) {
			fpm_clock_get(&now);
			if (q->ev) {
				if (timercmp(&now, &q->ev->timeout, >) || timercmp(&now, &q->ev->timeout, ==)) {
					fpm_event_fire(q->ev);
					/* sanity check */
					if (fpm_globals.parent_pid != getpid()) {
						return;
					}
					if (q->ev->flags & FPM_EV_PERSIST) {
						fpm_event_set_timeout(q->ev, now);
					} else { /* delete the event */
						q2 = q;
						if (q->prev) {
							q->prev->next = q->next;
						}
						if (q->next) {
							q->next->prev = q->prev;
						}
						if (q == fpm_event_queue_timer) {
							fpm_event_queue_timer = q->next;
							fpm_event_queue_timer->prev = NULL;
						}
						q = q->next;
						free(q2);
						continue;
					}
				}
			}
			q = q->next;
		}
	}
}
/* }}} */

void fpm_event_fire(struct fpm_event_s *ev) /* {{{ */
{
	if (!ev || !ev->callback) {
		return;
	}

	(*ev->callback)( (struct fpm_event_s *) ev, ev->which, ev->arg);	
}
/* }}} */

int fpm_event_set(struct fpm_event_s *ev, int fd, int flags, void (*callback)(struct fpm_event_s *, short, void *), void *arg) /* {{{ */
{
	if (!ev || !callback || fd < -1) {
		return -1;
	}
	memset(ev, 0, sizeof(struct fpm_event_s));
	ev->fd = fd;
	ev->callback = callback;
	ev->arg = arg;
	ev->flags = flags;
	return 0;
}
/* }}} */

int fpm_event_add(struct fpm_event_s *ev, unsigned long int frequency) /* {{{ */
{
	struct timeval now;
	struct timeval tmp;

	if (!ev) {
		return -1;
	}

	ev->index = -1;

	/* it's a triggered event on incoming data */
	if (ev->flags & FPM_EV_READ) {
		ev->which = FPM_EV_READ;
		if (fpm_event_queue_add(&fpm_event_queue_fd, ev) != 0) {
			return -1;
		}
		return 0;
	}

	/* it's a timer event */
	ev->which = FPM_EV_TIMEOUT;

	fpm_clock_get(&now);
	if (frequency >= 1000) {
		tmp.tv_sec = frequency / 1000;
		tmp.tv_usec = (frequency % 1000) * 1000;
	} else {
		tmp.tv_sec = 0;
		tmp.tv_usec = frequency * 1000;
	}
	ev->frequency = tmp;
	fpm_event_set_timeout(ev, now);

	if (fpm_event_queue_add(&fpm_event_queue_timer, ev) != 0) {
		return -1;
	}

	return 0;
}
/* }}} */

int fpm_event_del(struct fpm_event_s *ev) /* {{{ */
{
	if (fpm_event_queue_del(&fpm_event_queue_fd, ev) != 0) {
		return -1;
	}

	if (fpm_event_queue_del(&fpm_event_queue_timer, ev) != 0) {
		return -1;
	}

	return 0;
}
/* }}} */

/* }}} */
