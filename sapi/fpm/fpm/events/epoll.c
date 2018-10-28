/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Jerome Loyet <jerome@loyet.net>                             |
   +----------------------------------------------------------------------+
*/

#include "../fpm_config.h"
#include "../fpm_events.h"
#include "../fpm.h"
#include "../zlog.h"

#if HAVE_EPOLL

#include <sys/epoll.h>
#include <errno.h>

static int fpm_event_epoll_init(int max);
static int fpm_event_epoll_clean();
static int fpm_event_epoll_wait(struct fpm_event_queue_s *queue, unsigned long int timeout);
static int fpm_event_epoll_add(struct fpm_event_s *ev);
static int fpm_event_epoll_remove(struct fpm_event_s *ev);

static struct fpm_event_module_s epoll_module = {
	.name = "epoll",
	.support_edge_trigger = 1,
	.init = fpm_event_epoll_init,
	.clean = fpm_event_epoll_clean,
	.wait = fpm_event_epoll_wait,
	.add = fpm_event_epoll_add,
	.remove = fpm_event_epoll_remove,
};

static struct epoll_event *epollfds = NULL;
static int nepollfds = 0;
static int epollfd = -1;

#endif /* HAVE_EPOLL */

struct fpm_event_module_s *fpm_event_epoll_module() /* {{{ */
{
#if HAVE_EPOLL
	return &epoll_module;
#else
	return NULL;
#endif /* HAVE_EPOLL */
}
/* }}} */

#if HAVE_EPOLL

/*
 * Init the module
 */
static int fpm_event_epoll_init(int max) /* {{{ */
{
	if (max < 1) {
		return 0;
	}

	/* init epoll */
	epollfd = epoll_create(max + 1);
	if (epollfd < 0) {
		zlog(ZLOG_ERROR, "epoll: unable to initialize");
		return -1;
	}

	/* allocate fds */
	epollfds = malloc(sizeof(struct epoll_event) * max);
	if (!epollfds) {
		zlog(ZLOG_ERROR, "epoll: unable to allocate %d events", max);
		return -1;
	}
	memset(epollfds, 0, sizeof(struct epoll_event) * max);

	/* save max */
	nepollfds = max;

	return 0;
}
/* }}} */

/*
 * Clean the module
 */
static int fpm_event_epoll_clean() /* {{{ */
{
	/* free epollfds */
	if (epollfds) {
		free(epollfds);
		epollfds = NULL;
	}
	if (epollfd != -1) {
		close(epollfd);
		epollfd = -1;
	}

	nepollfds = 0;

	return 0;
}
/* }}} */

/*
 * wait for events or timeout
 */
static int fpm_event_epoll_wait(struct fpm_event_queue_s *queue, unsigned long int timeout) /* {{{ */
{
	int ret, i;

	/* ensure we have a clean epoolfds before calling epoll_wait() */
	memset(epollfds, 0, sizeof(struct epoll_event) * nepollfds);

	/* wait for inconming event or timeout */
	ret = epoll_wait(epollfd, epollfds, nepollfds, timeout);
	if (ret == -1) {

		/* trigger error unless signal interrupt */
		if (errno != EINTR) {
			zlog(ZLOG_WARNING, "epoll_wait() returns %d", errno);
			return -1;
		}
	}

	/* events have been triggered, let's fire them */
	for (i = 0; i < ret; i++) {

		/* do we have a valid ev ptr ? */
		if (!epollfds[i].data.ptr) {
			continue;
		}

		/* fire the event */
		fpm_event_fire((struct fpm_event_s *)epollfds[i].data.ptr);

		/* sanity check */
		if (fpm_globals.parent_pid != getpid()) {
			return -2;
		}
	}

	return ret;
}
/* }}} */

/*
 * Add a FD to the fd set
 */
static int fpm_event_epoll_add(struct fpm_event_s *ev) /* {{{ */
{
	struct epoll_event e;

	/* fill epoll struct */
#if SIZEOF_SIZE_T == 4
	/* Completely initialize event data to prevent valgrind reports */
	e.data.u64 = 0;
#endif
	e.events = EPOLLIN;
	e.data.fd = ev->fd;
	e.data.ptr = (void *)ev;

	if (ev->flags & FPM_EV_EDGE) {
		e.events = e.events | EPOLLET;
	}

	/* add the event to epoll internal queue */
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, ev->fd, &e) == -1) {
		zlog(ZLOG_ERROR, "epoll: unable to add fd %d", ev->fd);
		return -1;
	}

	/* mark the event as registered */
	ev->index = ev->fd;
	return 0;
}
/* }}} */

/*
 * Remove a FD from the fd set
 */
static int fpm_event_epoll_remove(struct fpm_event_s *ev) /* {{{ */
{
	struct epoll_event e;

	/* fill epoll struct the same way we did in fpm_event_epoll_add() */
	e.events = EPOLLIN;
	e.data.fd = ev->fd;
	e.data.ptr = (void *)ev;

	if (ev->flags & FPM_EV_EDGE) {
		e.events = e.events | EPOLLET;
	}

	/* remove the event from epoll internal queue */
	if (epoll_ctl(epollfd, EPOLL_CTL_DEL, ev->fd, &e) == -1) {
		zlog(ZLOG_ERROR, "epoll: unable to remove fd %d", ev->fd);
		return -1;
	}

	/* mark the event as not registered */
	ev->index = -1;
	return 0;
}
/* }}} */

#endif /* HAVE_EPOLL */
