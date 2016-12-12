/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
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

/* $Id$ */

#include "../fpm_config.h"
#include "../fpm_events.h"
#include "../fpm.h"
#include "../zlog.h"

#if HAVE_SELECT

/* According to POSIX.1-2001 */
#include <sys/select.h>

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <errno.h>

static int fpm_event_select_init(int max);
static int fpm_event_select_wait(struct fpm_event_queue_s *queue, unsigned long int timeout);
static int fpm_event_select_add(struct fpm_event_s *ev);
static int fpm_event_select_remove(struct fpm_event_s *ev);

static struct fpm_event_module_s select_module = {
	.name = "select",
	.support_edge_trigger = 0,
	.init = fpm_event_select_init,
	.clean = NULL,
	.wait = fpm_event_select_wait,
	.add = fpm_event_select_add,
	.remove = fpm_event_select_remove,
};

static fd_set fds;

#endif /* HAVE_SELECT */

/*
 * return the module configuration
 */
struct fpm_event_module_s *fpm_event_select_module() /* {{{ */
{
#if HAVE_SELECT
	return &select_module;
#else
	return NULL;
#endif /* HAVE_SELECT */
}
/* }}} */

#if HAVE_SELECT

/*
 * Init the module
 */
static int fpm_event_select_init(int max) /* {{{ */
{
	FD_ZERO(&fds);
	return 0;
}
/* }}} */


/*
 * wait for events or timeout
 */
static int fpm_event_select_wait(struct fpm_event_queue_s *queue, unsigned long int timeout) /* {{{ */
{
	int ret;
	struct fpm_event_queue_s *q;
	fd_set current_fds;
	struct timeval t;

	/* copy fds because select() alters it */
	current_fds = fds;

	/* fill struct timeval with timeout */
	t.tv_sec = timeout / 1000;
	t.tv_usec = (timeout % 1000) * 1000;

	/* wait for inconming event or timeout */
	ret = select(FD_SETSIZE, &current_fds, NULL, NULL, &t);
	if (ret == -1) {

		/* trigger error unless signal interrupt */
		if (errno != EINTR) {
			zlog(ZLOG_WARNING, "poll() returns %d", errno);
			return -1;
		}
	}

	/* events have been triggered */
	if (ret > 0) {

		/* trigger POLLIN events */
		q = queue;
		while (q) {
			if (q->ev) { /* sanity check */

				/* check if the event has been triggered */
				if (FD_ISSET(q->ev->fd, &current_fds)) {

					/* fire the event */
					fpm_event_fire(q->ev);

					/* sanity check */
					if (fpm_globals.parent_pid != getpid()) {
						return -2;
					}
				}
			}
			q = q->next; /* iterate */
		}
	}
	return ret;

}
/* }}} */

/*
 * Add a FD to the fd set
 */
static int fpm_event_select_add(struct fpm_event_s *ev) /* {{{ */
{
	/* check size limitation */
	if (ev->fd >= FD_SETSIZE) {
		zlog(ZLOG_ERROR, "select: not enough space in the select fd list (max = %d). Please consider using another event mechanism.", FD_SETSIZE);
		return -1;
	}

	/* add the FD if not already in */
	if (!FD_ISSET(ev->fd, &fds)) {
		FD_SET(ev->fd, &fds);
		ev->index = ev->fd;
	}

	return 0;
}
/* }}} */

/*
 * Remove a FD from the fd set
 */
static int fpm_event_select_remove(struct fpm_event_s *ev) /* {{{ */
{
	/* remove the fd if it's in */
	if (FD_ISSET(ev->fd, &fds)) {
		FD_CLR(ev->fd, &fds);
		ev->index = -1;
	}

	return 0;
}
/* }}} */

#endif /* HAVE_SELECT */
