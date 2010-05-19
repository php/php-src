
	/* $Id: fpm_events.c,v 1.21.2.2 2008/12/13 03:21:18 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <unistd.h>
#include <errno.h>
#include <stdlib.h> /* for putenv */
#include <string.h>

#include "fpm.h"
#include "fpm_process_ctl.h"
#include "fpm_events.h"
#include "fpm_cleanup.h"
#include "fpm_stdio.h"
#include "fpm_signals.h"
#include "fpm_children.h"
#include "zlog.h"

static void fpm_event_cleanup(int which, void *arg) /* {{{ */
{
	struct event_base *base = (struct event_base *)arg;
	event_base_free(base);
}
/* }}} */

static void fpm_got_signal(int fd, short ev, void *arg) /* {{{ */
{
	char c;
	int res;
	struct event_base *base = (struct event_base *)arg;

	do {
		do {
			res = read(fd, &c, 1);
		} while (res == -1 && errno == EINTR);

		if (res <= 0) {
			if (res < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
				zlog(ZLOG_STUFF, ZLOG_SYSERROR, "read() failed");
			}
			return;
		}

		switch (c) {
			case 'C' :                  /* SIGCHLD */
				zlog(ZLOG_STUFF, ZLOG_DEBUG, "received SIGCHLD");
				fpm_children_bury(base);
				break;
			case 'I' :                  /* SIGINT  */
				zlog(ZLOG_STUFF, ZLOG_DEBUG, "received SIGINT");
				zlog(ZLOG_STUFF, ZLOG_NOTICE, "Terminating ...");
				fpm_pctl(FPM_PCTL_STATE_TERMINATING, FPM_PCTL_ACTION_SET, base);
				break;
			case 'T' :                  /* SIGTERM */
				zlog(ZLOG_STUFF, ZLOG_DEBUG, "received SIGTERM");
				zlog(ZLOG_STUFF, ZLOG_NOTICE, "Terminating ...");
				fpm_pctl(FPM_PCTL_STATE_TERMINATING, FPM_PCTL_ACTION_SET, base);
				break;
			case 'Q' :                  /* SIGQUIT */
				zlog(ZLOG_STUFF, ZLOG_DEBUG, "received SIGQUIT");
				zlog(ZLOG_STUFF, ZLOG_NOTICE, "Finishing ...");
				fpm_pctl(FPM_PCTL_STATE_FINISHING, FPM_PCTL_ACTION_SET, base);
				break;
			case '1' :                  /* SIGUSR1 */
				zlog(ZLOG_STUFF, ZLOG_DEBUG, "received SIGUSR1");
				if (0 == fpm_stdio_open_error_log(1)) {
					zlog(ZLOG_STUFF, ZLOG_NOTICE, "log file re-opened");
				} else {
					zlog(ZLOG_STUFF, ZLOG_ERROR, "unable to re-opened log file");
				}
				break;
			case '2' :                  /* SIGUSR2 */
				zlog(ZLOG_STUFF, ZLOG_DEBUG, "received SIGUSR2");
				zlog(ZLOG_STUFF, ZLOG_NOTICE, "Reloading in progress ...");
				fpm_pctl(FPM_PCTL_STATE_RELOADING, FPM_PCTL_ACTION_SET, base);
				break;
		}

		if (fpm_globals.is_child) {
			break;
		}
	} while (1);
	return;
}
/* }}} */

int fpm_event_init_main(struct event_base **base) /* {{{ */
{
	*base = event_base_new();

	zlog(ZLOG_STUFF, ZLOG_DEBUG, "libevent: using %s", event_base_get_method(*base));

	if (0 > fpm_cleanup_add(FPM_CLEANUP_ALL, fpm_event_cleanup, *base)) {
		return -1;
	}
	return 0;
}
/* }}} */

int fpm_event_loop(struct event_base *base) /* {{{ */
{
	static struct event signal_fd_event;

	event_set(&signal_fd_event, fpm_signals_get_fd(), EV_PERSIST | EV_READ, &fpm_got_signal, base);
	event_base_set(base, &signal_fd_event);
	event_add(&signal_fd_event, 0);
	fpm_pctl_heartbeat(-1, 0, base);
	fpm_pctl_perform_idle_server_maintenance_heartbeat(-1, 0, base);
	zlog(ZLOG_STUFF, ZLOG_NOTICE, "ready to handle connections");
	event_base_dispatch(base);
	return 0;
}
/* }}} */

int fpm_event_add(int fd, struct event_base *base, struct event *ev, void (*callback)(int, short, void *), void *arg) /* {{{ */
{
	event_set(ev, fd, EV_PERSIST | EV_READ, callback, arg);
	event_base_set(base, ev);
	return event_add(ev, 0);
}
/* }}} */

int fpm_event_del(struct event *ev) /* {{{ */
{
	return event_del(ev);
}
/* }}} */

void fpm_event_exit_loop(struct event_base *base) /* {{{ */
{
	event_base_loopbreak(base);
}
/* }}} */

void fpm_event_fire(struct event *ev) /* {{{ */
{
	(*ev->ev_callback)( (int) ev->ev_fd, (short) ev->ev_res, ev->ev_arg);	
}
/* }}} */

