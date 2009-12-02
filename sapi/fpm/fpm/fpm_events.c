
	/* $Id: fpm_events.c,v 1.21.2.2 2008/12/13 03:21:18 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <unistd.h>
#include <errno.h>
#include <stdlib.h> /* for putenv */
#include <string.h>
#include <sys/types.h> /* for event.h below */
#include <event.h>

#include "fpm.h"
#include "fpm_process_ctl.h"
#include "fpm_events.h"
#include "fpm_cleanup.h"
#include "fpm_stdio.h"
#include "fpm_signals.h"
#include "fpm_children.h"
#include "zlog.h"

static void fpm_event_cleanup(int which, void *arg)
{
	event_base_free(0);
}

static void fpm_got_signal(int fd, short ev, void *arg)
{
	char c;
	int res;

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
				zlog(ZLOG_STUFF, ZLOG_NOTICE, "received SIGCHLD");
				fpm_children_bury();
				break;
			case 'I' :                  /* SIGINT  */
				zlog(ZLOG_STUFF, ZLOG_NOTICE, "received SIGINT");
				fpm_pctl(FPM_PCTL_STATE_TERMINATING, FPM_PCTL_ACTION_SET);
				break;
			case 'T' :                  /* SIGTERM */
				zlog(ZLOG_STUFF, ZLOG_NOTICE, "received SIGTERM");
				fpm_pctl(FPM_PCTL_STATE_TERMINATING, FPM_PCTL_ACTION_SET);
				break;
			case 'Q' :                  /* SIGQUIT */
				zlog(ZLOG_STUFF, ZLOG_NOTICE, "received SIGQUIT");
				fpm_pctl(FPM_PCTL_STATE_FINISHING, FPM_PCTL_ACTION_SET);
				break;
			case '1' :                  /* SIGUSR1 */
				zlog(ZLOG_STUFF, ZLOG_NOTICE, "received SIGUSR1");
				if (0 == fpm_stdio_open_error_log(1)) {
					zlog(ZLOG_STUFF, ZLOG_NOTICE, "log file re-opened");
				}
				break;
			case '2' :                  /* SIGUSR2 */
				zlog(ZLOG_STUFF, ZLOG_NOTICE, "received SIGUSR2");
				fpm_pctl(FPM_PCTL_STATE_RELOADING, FPM_PCTL_ACTION_SET);
				break;
		}

		if (fpm_globals.is_child) {
			break;
		}

	} while (1);

	return;
}

int fpm_event_init_main()
{
	event_init();

	zlog(ZLOG_STUFF, ZLOG_NOTICE, "libevent: using %s", event_get_method());

	if (0 > fpm_cleanup_add(FPM_CLEANUP_ALL, fpm_event_cleanup, 0)) {
		return -1;
	}

	return 0;
}

int fpm_event_loop()
{
	static struct event signal_fd_event;

	event_set(&signal_fd_event, fpm_signals_get_fd(), EV_PERSIST | EV_READ, &fpm_got_signal, 0);

	event_add(&signal_fd_event, 0);

	fpm_pctl_heartbeat(-1, 0, 0);

	zlog(ZLOG_STUFF, ZLOG_NOTICE, "libevent: entering main loop");

	event_loop(0);

	return 0;
}

int fpm_event_add(int fd, struct event *ev, void (*callback)(int, short, void *), void *arg)
{
	event_set(ev, fd, EV_PERSIST | EV_READ, callback, arg);

	return event_add(ev, 0);
}

int fpm_event_del(struct event *ev)
{
	return event_del(ev);
}

void fpm_event_exit_loop()
{
	event_loopbreak();
}

void fpm_event_fire(struct event *ev)
{
	(*ev->ev_callback)( (int) ev->ev_fd, (short) ev->ev_res, ev->ev_arg);	
}

