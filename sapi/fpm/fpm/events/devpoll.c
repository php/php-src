/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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

#if HAVE_DEVPOLL

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/devpoll.h>
#include <errno.h>

static int fpm_event_devpoll_init(int max);
static int fpm_event_devpoll_clean();
static int fpm_event_devpoll_wait(struct fpm_event_queue_s *queue, unsigned long int timeout);
static int fpm_event_devpoll_add(struct fpm_event_s *ev);
static int fpm_event_devpoll_remove(struct fpm_event_s *ev);

static struct fpm_event_module_s devpoll_module = {
	.name = "/dev/poll",
	.support_edge_trigger = 0,
	.init = fpm_event_devpoll_init,
	.clean = fpm_event_devpoll_clean,
	.wait = fpm_event_devpoll_wait,
	.add = fpm_event_devpoll_add,
	.remove = fpm_event_devpoll_remove,
};

int dpfd = -1;
static struct pollfd *pollfds = NULL;
static struct pollfd *active_pollfds = NULL;
static int npollfds = 0;

#endif /* HAVE_DEVPOLL */

struct fpm_event_module_s *fpm_event_devpoll_module() /* {{{ */
{
#if HAVE_DEVPOLL
	return &devpoll_module;
#else
	return NULL;
#endif /* HAVE_DEVPOLL */
}
/* }}} */

#if HAVE_DEVPOLL

/*
 * Init module
 */
static int fpm_event_devpoll_init(int max) /* {{{ */
{
	int i;

	/* open /dev/poll for future usages */
	dpfd = open("/dev/poll", O_RDWR);
	if (dpfd < 0) {
		zlog(ZLOG_ERROR, "Unable to open /dev/poll");
		return -1;
	}

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
/* }}} */

/*
 * Clean the module
 */
static int fpm_event_devpoll_clean() /* {{{ */
{
	/* close /dev/poll if open */
	if (dpfd > -1) {
		close(dpfd);
		dpfd = -1;
	}

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
/* }}} */

/*
 * wait for events or timeout
 */
static int fpm_event_devpoll_wait(struct fpm_event_queue_s *queue, unsigned long int timeout) /* {{{ */
{
	int ret, i;
	struct fpm_event_queue_s *q;
	struct dvpoll dopoll;

	/* setup /dev/poll */
	dopoll.dp_fds = active_pollfds;
	dopoll.dp_nfds = npollfds;
	dopoll.dp_timeout = (int)timeout;

	/* wait for inconming event or timeout */
	ret = ioctl(dpfd, DP_POLL, &dopoll);

	if (ret < 0) {

		/* trigger error unless signal interrupt */
		if (errno != EINTR) {
			zlog(ZLOG_WARNING, "/dev/poll: ioctl() returns %d", errno);
			return -1;
		}
	}

	/* iterate through triggered events */
	for (i = 0; i < ret; i++) {

		/* find the corresponding event */
		q = queue;
		while (q) {

			/* found */
			if (q->ev && q->ev->fd == active_pollfds[i].fd) {

					/* fire the event */
					fpm_event_fire(q->ev);

					/* sanity check */
					if (fpm_globals.parent_pid != getpid()) {
						return -2;
					}
				break; /* next triggered event */
			}
			q = q->next; /* iterate */
		}
	}

	return ret;
}
/* }}} */

/*
 * Add a FD from the fd set
 */
static int fpm_event_devpoll_add(struct fpm_event_s *ev) /* {{{ */
{
	struct pollfd pollfd;

	/* fill pollfd with event information */
	pollfd.fd = ev->fd;
	pollfd.events = POLLIN;
	pollfd.revents = 0;

	/* add the event to the internal queue */
	if (write(dpfd, &pollfd, sizeof(struct pollfd)) != sizeof(struct pollfd)) {
		zlog(ZLOG_ERROR, "/dev/poll: Unable to add the event in the internal queue");
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
static int fpm_event_devpoll_remove(struct fpm_event_s *ev) /* {{{ */
{
	struct pollfd pollfd;

	/* fill pollfd with the same information as fpm_event_devpoll_add */
	pollfd.fd = ev->fd;
	pollfd.events = POLLIN | POLLREMOVE;
	pollfd.revents = 0;

	/* add the event to the internal queue */
	if (write(dpfd, &pollfd, sizeof(struct pollfd)) != sizeof(struct pollfd)) {
		zlog(ZLOG_ERROR, "/dev/poll: Unable to remove the event in the internal queue");
		return -1;
	}

	/* mark the event as registered */
	ev->index = -1;

	return 0;
}
/* }}} */

#endif /* HAVE_DEVPOLL */
