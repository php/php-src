/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Jerome Loyet <jerome@loyet.net>                             |
   +----------------------------------------------------------------------+
*/

#include "../fpm_config.h"
#include "../fpm_events.h"
#include "../fpm.h"
#include "../zlog.h"

#ifdef HAVE_KQUEUE

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include <errno.h>

static int fpm_event_kqueue_init(int max);
static int fpm_event_kqueue_clean(void);
static int fpm_event_kqueue_wait(struct fpm_event_queue_s *queue, unsigned long int timeout);
static int fpm_event_kqueue_add(struct fpm_event_s *ev);
static int fpm_event_kqueue_remove(struct fpm_event_s *ev);

static struct fpm_event_module_s kqueue_module = {
	.name = "kqueue",
	.support_edge_trigger = 1,
	.init = fpm_event_kqueue_init,
	.clean = fpm_event_kqueue_clean,
	.wait = fpm_event_kqueue_wait,
	.add = fpm_event_kqueue_add,
	.remove = fpm_event_kqueue_remove,
};

static struct kevent *kevents = NULL;
static int nkevents = 0;
static int kfd = 0;

#endif /* HAVE_KQUEUE */

/*
 * Return the module configuration
 */
struct fpm_event_module_s *fpm_event_kqueue_module(void) /* {{{ */
{
#ifdef HAVE_KQUEUE
	return &kqueue_module;
#else
	return NULL;
#endif /* HAVE_KQUEUE */
}
/* }}} */

#ifdef HAVE_KQUEUE

/*
 * init kqueue and stuff
 */
static int fpm_event_kqueue_init(int max) /* {{{ */
{
	if (max < 1) {
		return 0;
	}

	kfd = kqueue();
	if (kfd < 0) {
		zlog(ZLOG_ERROR, "kqueue: unable to initialize");
		return -1;
	}

	kevents = calloc(max, sizeof(struct kevent));
	if (!kevents) {
		zlog(ZLOG_ERROR, "kevent: unable to allocate %d events", max);
		return -1;
	}

	nkevents = max;

	return 0;
}
/* }}} */

/*
 * release kqueue stuff
 */
static int fpm_event_kqueue_clean(void) /* {{{ */
{
	if (kevents) {
		free(kevents);
		kevents = NULL;
	}

	nkevents = 0;

	return 0;
}
/* }}} */

/*
 * wait for events or timeout
 */
static int fpm_event_kqueue_wait(struct fpm_event_queue_s *queue, unsigned long int timeout) /* {{{ */
{
	struct timespec t;
	int ret, i;

	/* ensure we have a clean kevents before calling kevent() */
	memset(kevents, 0, sizeof(struct kevent) * nkevents);

	/* convert ms to timespec struct */
	t.tv_sec = timeout / 1000;
	t.tv_nsec = (timeout % 1000) * 1000 * 1000;

	/* wait for incoming event or timeout */
	ret = kevent(kfd, NULL, 0, kevents, nkevents, &t);
	if (ret == -1) {

		/* trigger error unless signal interrupt */
		if (errno != EINTR) {
			zlog(ZLOG_WARNING, "kevent() returns %d", errno);
			return -1;
		}
	}

	/* fire triggered events */
	for (i = 0; i < ret; i++) {
		if (kevents[i].udata) {
			struct fpm_event_s *ev = (struct fpm_event_s *)kevents[i].udata;
			fpm_event_fire(ev);
			/* sanity check */
			if (fpm_globals.parent_pid != getpid()) {
				return -2;
			}
		}
	}

	return ret;
}
/* }}} */

/*
 * Add a FD to kevent queue
 */
static int fpm_event_kqueue_add(struct fpm_event_s *ev) /* {{{ */
{
	struct kevent k;
	int flags = EV_ADD;

	if (ev->flags & FPM_EV_EDGE) {
			flags = flags | EV_CLEAR;
	}

	EV_SET(&k, ev->fd, EVFILT_READ, flags, 0, 0, (void *)ev);

	if (kevent(kfd, &k, 1, NULL, 0, NULL) < 0) {
		zlog(ZLOG_ERROR, "kevent: unable to add event");
		return -1;
	}

	/* mark the event as registered */
	ev->index = ev->fd;
	return 0;
}
/* }}} */

/*
 * Remove a FD from the kevent queue
 */
static int fpm_event_kqueue_remove(struct fpm_event_s *ev) /* {{{ */
{
	struct kevent k;
	int flags = EV_DELETE;

	if (ev->flags & FPM_EV_EDGE) {
			flags = flags | EV_CLEAR;
	}

	EV_SET(&k, ev->fd, EVFILT_READ, flags, 0, 0, (void *)ev);

	if (kevent(kfd, &k, 1, NULL, 0, NULL) < 0) {
		zlog(ZLOG_ERROR, "kevent: unable to delete event");
		return -1;
	}

	/* mark the event as not registered */
	ev->index = -1;
	return 0;
}
/* }}} */

#endif /* HAVE_KQUEUE */
