/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
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

#if HAVE_PORT

#include <port.h>
#include <poll.h>
#include <errno.h>

static int fpm_event_port_init(int max);
static int fpm_event_port_clean();
static int fpm_event_port_wait(struct fpm_event_queue_s *queue, unsigned long int timeout);
static int fpm_event_port_add(struct fpm_event_s *ev);
static int fpm_event_port_remove(struct fpm_event_s *ev);

static struct fpm_event_module_s port_module = {
	.name = "port",
	.support_edge_trigger = 0,
	.init = fpm_event_port_init,
	.clean = fpm_event_port_clean,
	.wait = fpm_event_port_wait,
	.add = fpm_event_port_add,
	.remove = fpm_event_port_remove, 
};

port_event_t *events = NULL;
int nevents = 0;
static int pfd = -1;

#endif /* HAVE_PORT */

struct fpm_event_module_s *fpm_event_port_module() /* {{{ */
{
#if HAVE_PORT
	return &port_module;
#else
	return NULL;
#endif /* HAVE_PORT */
}
/* }}} */

#if HAVE_PORT

/*
 * Init the module
 */
static int fpm_event_port_init(int max) /* {{{ */
{
	/* open port */
	pfd = port_create();
	if (pfd < 0) {
		zlog(ZLOG_ERROR, "port: unable to initialize port_create()");
		return -1;
	}

	if (max < 1) {
		return 0;
	}

	/* alloc and clear active_pollfds */
	events = malloc(sizeof(port_event_t) * max);
	if (!events) {
		zlog(ZLOG_ERROR, "port: Unable to allocate %d events", max);
		return -1;
	}

	nevents = max;
	return 0;
}
/* }}} */

/*
 * Clean the module
 */
static int fpm_event_port_clean() /* {{{ */
{
	if (pfd > -1) {
		close(pfd);
		pfd = -1;
	}

	if (events) {
		free(events);
		events = NULL;
	}

	nevents = 0;
	return 0;
}
/* }}} */

/*
 * wait for events or timeout
 */
static int fpm_event_port_wait(struct fpm_event_queue_s *queue, unsigned long int timeout) /* {{{ */
{
	int ret, i, nget;
	timespec_t t;

	/* convert timeout into timespec_t */
	t.tv_sec = (int)(timeout / 1000);
	t.tv_nsec = (timeout % 1000) * 1000 * 1000;

	/* wait for inconming event or timeout. We want at least one event or timeout */
	nget = 1;
	ret = port_getn(pfd, events, nevents, &nget, &t);
	if (ret < 0) {

		/* trigger error unless signal interrupt or timeout */
		if (errno != EINTR && errno != ETIME) {
			zlog(ZLOG_WARNING, "poll() returns %d", errno);
			return -1;
		}
	}

	for (i = 0; i < nget; i++) {

		/* do we have a ptr to the event ? */
		if (!events[i].portev_user) {
			continue;
		}

		/* fire the event */
		fpm_event_fire((struct fpm_event_s *)events[i].portev_user);

		/* sanity check */
		if (fpm_globals.parent_pid != getpid()) {
			return -2;
		}
	}
	return nget;
}
/* }}} */

/*
 * Add a FD to the fd set
 */
static int fpm_event_port_add(struct fpm_event_s *ev) /* {{{ */
{
	/* add the event to port */
	if (port_associate(pfd, PORT_SOURCE_FD, ev->fd, POLLIN, (void *)ev) < 0) {
		zlog(ZLOG_ERROR, "port: unable to add the event");
		return -1;
	}
	return 0;
}
/* }}} */

/*
 * Remove a FD from the fd set
 */
static int fpm_event_port_remove(struct fpm_event_s *ev) /* {{{ */
{
	/* remove the event from port */
	if (port_dissociate(pfd, PORT_SOURCE_FD, ev->fd) < 0) {
		zlog(ZLOG_ERROR, "port: unable to add the event");
		return -1;
	}
	return 0;
}
/* }}} */

#endif /* HAVE_PORT */
