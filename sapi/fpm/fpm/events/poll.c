/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#ifdef HAVE_POLL

#include <poll.h>
#include <errno.h>
#include <string.h>

static int fpm_event_poll_init(int max);
static int fpm_event_poll_clean(void);
static int fpm_event_poll_wait(struct fpm_event_queue_s *queue, unsigned long int timeout);
static int fpm_event_poll_add(struct fpm_event_s *ev);
static int fpm_event_poll_remove(struct fpm_event_s *ev);

static struct fpm_event_module_s poll_module = {
	.name = "poll",
	.support_edge_trigger = 0,
	.init = fpm_event_poll_init,
	.clean = fpm_event_poll_clean,
	.wait = fpm_event_poll_wait,
	.add = fpm_event_poll_add,
	.remove = fpm_event_poll_remove,
};

static struct pollfd *pollfds = NULL;
static struct pollfd *active_pollfds = NULL;
static int npollfds = 0;
static int next_free_slot = 0;
#endif /* HAVE_POLL */

/*
 * return the module configuration
 */
struct fpm_event_module_s *fpm_event_poll_module(void)
{
#ifdef HAVE_POLL
	return &poll_module;
#else
	return NULL;
#endif /* HAVE_POLL */
}

#ifdef HAVE_POLL

/*
 * Init the module
 */
static int fpm_event_poll_init(int max)
{
	int i;

	if (max < 1) {
		return 0;
	}

	/* alloc and clear pollfds */
	pollfds = malloc(sizeof(struct pollfd) * max);
	if (!pollfds) {
		zlog(ZLOG_ERROR, "poll: unable to allocate %d events", max);
		return -1;
	}
	memset(pollfds, 0, sizeof(struct pollfd) * max);

	/* set all fd to -1 in order to ensure it's not set */
	for (i = 0; i < max; i++) {
			pollfds[i].fd = -1;
	}

	/* alloc and clear active_pollfds */
	active_pollfds = malloc(sizeof(struct pollfd) * max);
	if (!active_pollfds) {
		free(pollfds);
		zlog(ZLOG_ERROR, "poll: unable to allocate %d events", max);
		return -1;
	}
	memset(active_pollfds, 0, sizeof(struct pollfd) * max);

	/* save max */
	npollfds = max;
	return 0;
}

/*
 * Clean the module
 */
static int fpm_event_poll_clean(void)
{
	/* free pollfds */
	if (pollfds) {
		free(pollfds);
		pollfds = NULL;
	}

	/* free active_pollfds */
	if (active_pollfds) {
		free(active_pollfds);
		active_pollfds = NULL;
	}

	npollfds = 0;
	return 0;
}

/*
 * wait for events or timeout
 */
static int fpm_event_poll_wait(struct fpm_event_queue_s *queue, unsigned long int timeout) /* {{{ */
{
	int ret;
	struct fpm_event_queue_s *q;

	if (npollfds > 0) {
		/* copy pollfds because poll() alters it */
		memcpy(active_pollfds, pollfds, sizeof(struct pollfd) * npollfds);
	}

	/* wait for inconming event or timeout */
	ret = poll(active_pollfds, npollfds, timeout);
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
			/* ensure ev->index is valid */
			if (q->ev && q->ev->index >= 0 && q->ev->index < npollfds && q->ev->fd == active_pollfds[q->ev->index].fd) {

				/* has the event has been triggered ? */
				if (active_pollfds[q->ev->index].revents & POLLIN) {

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
static int fpm_event_poll_add(struct fpm_event_s *ev) /* {{{ */
{
	int i;

	/* do we have a direct free slot */
	if (pollfds[next_free_slot].fd == -1) {
		/* register the event */
		pollfds[next_free_slot].fd = ev->fd;
		pollfds[next_free_slot].events = POLLIN;

		/* remember the event place in the fd list and suppose next slot is free */
		ev->index = next_free_slot++;
		if (next_free_slot >= npollfds) {
			next_free_slot = 0;
		}
		return 0;
	}

	/* let's search */
	for (i = 0; i < npollfds; i++) {
		if (pollfds[i].fd != -1) {
			/* not free */
			continue;
		}

		/* register the event */
		pollfds[i].fd = ev->fd;
		pollfds[i].events = POLLIN;

		/* remember the event place in the fd list and suppose next slot is free */
		ev->index = next_free_slot++;
		if (next_free_slot >= npollfds) {
			next_free_slot = 0;
		}
		return 0;
	}

	zlog(ZLOG_ERROR, "poll: not enough space to add event (fd=%d)", ev->fd);
	return -1;
}
/* }}} */

/*
 * Remove a FD from the fd set
 */
static int fpm_event_poll_remove(struct fpm_event_s *ev) /* {{{ */
{
	int i;

	/* do we have a direct access */
	if (ev->index >= 0 && ev->index < npollfds && pollfds[ev->index].fd == ev->fd) {
		/* remember this slot as free */
		next_free_slot = ev->index;

		/* clear event in pollfds */
		pollfds[ev->index].fd = -1;
		pollfds[ev->index].events = 0;

		/* mark the event as not registered */
		ev->index = -1;

		return 0;
	}

	/* let's search */
	for (i = 0; i < npollfds; i++) {

		if (pollfds[i].fd != ev->fd) {
			/* not found */
			continue;
		}

		/* remember this slot as free */
		next_free_slot = i;

		/* clear event in pollfds */
		pollfds[i].fd = -1;
		pollfds[i].events = 0;

		/* mark the event as not registered */
		ev->index = -1;

		return 0;
	}

	zlog(ZLOG_ERROR, "poll: unable to remove event: not found (fd=%d, index=%d)", ev->fd, ev->index);
	return -1;
}
/* }}} */

#endif /* HAVE_POLL */
