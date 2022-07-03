	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <unistd.h>
#include <errno.h>
#include <stdlib.h> /* for putenv */
#include <string.h>

#include <php.h>

#include "fpm.h"
#include "fpm_process_ctl.h"
#include "fpm_events.h"
#include "fpm_cleanup.h"
#include "fpm_stdio.h"
#include "fpm_signals.h"
#include "fpm_children.h"
#include "zlog.h"
#include "fpm_clock.h"
#include "fpm_log.h"

#include "events/select.h"
#include "events/poll.h"
#include "events/epoll.h"
#include "events/devpoll.h"
#include "events/port.h"
#include "events/kqueue.h"

#ifdef HAVE_SYSTEMD
#include "fpm_systemd.h"
#endif

#define fpm_event_set_timeout(ev, now) timeradd(&(now), &(ev)->frequency, &(ev)->timeout);

static void fpm_event_cleanup(int which, void *arg);
static void fpm_postponed_children_bury(struct fpm_event_s *ev, short which, void *arg);
static void fpm_got_signal(struct fpm_event_s *ev, short which, void *arg);
static struct fpm_event_s *fpm_event_queue_isset(struct fpm_event_queue_s *queue, struct fpm_event_s *ev);
static int fpm_event_queue_add(struct fpm_event_queue_s **queue, struct fpm_event_s *ev);
static int fpm_event_queue_del(struct fpm_event_queue_s **queue, struct fpm_event_s *ev);
static void fpm_event_queue_destroy(struct fpm_event_queue_s **queue);

static struct fpm_event_module_s *module;
static struct fpm_event_queue_s *fpm_event_queue_timer = NULL;
static struct fpm_event_queue_s *fpm_event_queue_fd = NULL;
static struct fpm_event_s children_bury_timer;

static void fpm_event_cleanup(int which, void *arg) /* {{{ */
{
	fpm_event_queue_destroy(&fpm_event_queue_timer);
	fpm_event_queue_destroy(&fpm_event_queue_fd);
}
/* }}} */

static void fpm_postponed_children_bury(struct fpm_event_s *ev, short which, void *arg) /* {{{ */
{
	fpm_children_bury();
}
/* }}} */

static void fpm_got_signal(struct fpm_event_s *ev, short which, void *arg) /* {{{ */
{
	char c;
	int res, ret;
	int fd = ev->fd;

	do {
		do {
			res = read(fd, &c, 1);
		} while (res == -1 && errno == EINTR);

		if (res <= 0) {
			if (res < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
				zlog(ZLOG_SYSERROR, "unable to read from the signal pipe");
			}
			return;
		}

		switch (c) {
			case 'C' :                  /* SIGCHLD */
				zlog(ZLOG_DEBUG, "received SIGCHLD");
				/* epoll_wait() may report signal fd before read events for a finished child
				 * in the same bunch of events. Prevent immediate free of the child structure
				 * and so the fpm_event_s instance. Otherwise use after free happens during
				 * attempt to process following read event. */
				fpm_event_set_timer(&children_bury_timer, 0, &fpm_postponed_children_bury, NULL);
				fpm_event_add(&children_bury_timer, 0);
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

				/* fpm_stdio_init_final tied STDERR fd with error_log fd. This affects logging to the
				 * access.log if it was configured to write to the stderr. Check #8885. */
				fpm_stdio_restore_original_stderr(0);

				if (0 == fpm_stdio_open_error_log(1)) {
					zlog(ZLOG_NOTICE, "error log file re-opened");
				} else {
					zlog(ZLOG_ERROR, "unable to re-opened error log file");
				}

				ret = fpm_log_open(1);
				if (ret == 0) {
					zlog(ZLOG_NOTICE, "access log file re-opened");
				} else if (ret == -1) {
					zlog(ZLOG_ERROR, "unable to re-opened access log file");
				}
				/* else no access log are set */

				/* We need to tie stderr with error_log in the master process after log files reload. Check #8885. */
				fpm_stdio_redirect_stderr_to_error_log();

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
		zlog(ZLOG_SYSERROR, "Unable to add the event to queue: malloc() failed");
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

	/* ask the event module to add the fd from its own queue */
	if (*queue == fpm_event_queue_fd && module->add) {
		module->add(ev);
	}

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
				if (*queue) {
					(*queue)->prev = NULL;
				}
			}

			/* ask the event module to remove the fd from its own queue */
			if (*queue == fpm_event_queue_fd && module->remove) {
				module->remove(ev);
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

	if (*queue == fpm_event_queue_fd && module->clean) {
		module->clean();
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

int fpm_event_pre_init(char *machanism) /* {{{ */
{
	/* kqueue */
	module = fpm_event_kqueue_module();
	if (module) {
		if (!machanism || strcasecmp(module->name, machanism) == 0) {
			return 0;
		}
	}

	/* port */
	module = fpm_event_port_module();
	if (module) {
		if (!machanism || strcasecmp(module->name, machanism) == 0) {
			return 0;
		}
	}

	/* epoll */
	module = fpm_event_epoll_module();
	if (module) {
		if (!machanism || strcasecmp(module->name, machanism) == 0) {
			return 0;
		}
	}

	/* /dev/poll */
	module = fpm_event_devpoll_module();
	if (module) {
		if (!machanism || strcasecmp(module->name, machanism) == 0) {
			return 0;
		}
	}

	/* poll */
	module = fpm_event_poll_module();
	if (module) {
		if (!machanism || strcasecmp(module->name, machanism) == 0) {
			return 0;
		}
	}

	/* select */
	module = fpm_event_select_module();
	if (module) {
		if (!machanism || strcasecmp(module->name, machanism) == 0) {
			return 0;
		}
	}

	if (machanism) {
		zlog(ZLOG_ERROR, "event mechanism '%s' is not available on this system", machanism);
	} else {
		zlog(ZLOG_ERROR, "unable to find a suitable event mechanism on this system");
	}
	return -1;
}
/* }}} */

const char *fpm_event_machanism_name(void)
{
	return module ? module->name : NULL;
}

int fpm_event_support_edge_trigger(void)
{
	return module ? module->support_edge_trigger : 0;
}

int fpm_event_init_main(void)
{
	struct fpm_worker_pool_s *wp;
	int max;

	if (!module) {
		zlog(ZLOG_ERROR, "no event module found");
		return -1;
	}

	if (!module->wait) {
		zlog(ZLOG_ERROR, "Incomplete event implementation. Please open a bug report on https://bugs.php.net.");
		return -1;
	}

	/* count the max number of necessary fds for polling */
	max = 1; /* only one FD is necessary at startup for the master process signal pipe */
	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		if (!wp->config) continue;
		if (wp->config->catch_workers_output && wp->config->pm_max_children > 0) {
			max += (wp->config->pm_max_children * 2);
		}
	}

	if (module->init(max) < 0) {
		zlog(ZLOG_ERROR, "Unable to initialize the event module %s", module->name);
		return -1;
	}

	zlog(ZLOG_DEBUG, "event module is %s and %d fds have been reserved", module->name, max);

	if (0 > fpm_cleanup_add(FPM_CLEANUP_ALL, fpm_event_cleanup, NULL)) {
		return -1;
	}
	return 0;
}

void fpm_event_loop(int err) /* {{{ */
{
	static struct fpm_event_s signal_fd_event;

	/* sanity check */
	if (fpm_globals.parent_pid != getpid()) {
		return;
	}

	fpm_event_set(&signal_fd_event, fpm_signals_get_fd(), FPM_EV_READ, &fpm_got_signal, NULL);
	fpm_event_add(&signal_fd_event, 0);

	/* add timers */
	if (fpm_globals.heartbeat > 0) {
		fpm_pctl_heartbeat(NULL, 0, NULL);
	}

	if (!err) {
		fpm_pctl_perform_idle_server_maintenance_heartbeat(NULL, 0, NULL);

		zlog(ZLOG_DEBUG, "%zu bytes have been reserved in SHM", fpm_shm_get_size_allocated());
		zlog(ZLOG_NOTICE, "ready to handle connections");

#ifdef HAVE_SYSTEMD
		fpm_systemd_heartbeat(NULL, 0, NULL);
#endif
	}

	while (1) {
		struct fpm_event_queue_s *q, *q2;
		struct timeval ms;
		struct timeval tmp;
		struct timeval now;
		unsigned long int timeout;
		int ret;

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

		ret = module->wait(fpm_event_queue_fd, timeout);

		/* is a child, nothing to do here */
		if (ret == -2) {
			return;
		}

		if (ret > 0) {
			zlog(ZLOG_DEBUG, "event module triggered %d events", ret);
		}

		/* trigger timers */
		q = fpm_event_queue_timer;
		while (q) {
			struct fpm_event_queue_s *next = q->next;
			fpm_clock_get(&now);
			if (q->ev) {
				if (timercmp(&now, &q->ev->timeout, >) || timercmp(&now, &q->ev->timeout, ==)) {
					struct fpm_event_s *ev = q->ev;
					if (ev->flags & FPM_EV_PERSIST) {
						fpm_event_set_timeout(ev, now);
					} else {
						/* Delete the event. Make sure this happens before it is fired,
						 * so that the event callback may register the same timer again. */
						q2 = q;
						if (q->prev) {
							q->prev->next = q->next;
						}
						if (q->next) {
							q->next->prev = q->prev;
						}
						if (q == fpm_event_queue_timer) {
							fpm_event_queue_timer = q->next;
							if (fpm_event_queue_timer) {
								fpm_event_queue_timer->prev = NULL;
							}
						}
						free(q2);
					}

					fpm_event_fire(ev);

					/* sanity check */
					if (fpm_globals.parent_pid != getpid()) {
						return;
					}
				}
			}
			q = next;
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
	if (ev->index >= 0 && fpm_event_queue_del(&fpm_event_queue_fd, ev) != 0) {
		return -1;
	}

	if (ev->index < 0 && fpm_event_queue_del(&fpm_event_queue_timer, ev) != 0) {
		return -1;
	}

	return 0;
}
/* }}} */
