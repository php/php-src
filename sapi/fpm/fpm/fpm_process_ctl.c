
	/* $Id: fpm_process_ctl.c,v 1.19.2.2 2008/12/13 03:21:18 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include "fpm.h"
#include "fpm_clock.h"
#include "fpm_children.h"
#include "fpm_signals.h"
#include "fpm_events.h"
#include "fpm_process_ctl.h"
#include "fpm_cleanup.h"
#include "fpm_request.h"
#include "fpm_worker_pool.h"
#include "fpm_status.h"
#include "zlog.h"


static int fpm_state = FPM_PCTL_STATE_NORMAL;
static int fpm_signal_sent = 0;


static const char *fpm_state_names[] = {
	[FPM_PCTL_STATE_NORMAL] = "normal",
	[FPM_PCTL_STATE_RELOADING] = "reloading",
	[FPM_PCTL_STATE_TERMINATING] = "terminating",
	[FPM_PCTL_STATE_FINISHING] = "finishing"
};

static int saved_argc;
static char **saved_argv;

static void fpm_pctl_cleanup(int which, void *arg) /* {{{ */
{
	int i;
	if (which != FPM_CLEANUP_PARENT_EXEC) {
		for (i = 0; i < saved_argc; i++) {
			free(saved_argv[i]);
		}
		free(saved_argv);
	}
}
/* }}} */

static struct event pctl_event;

static void fpm_pctl_action(int fd, short which, void *arg) /* {{{ */
{
	struct event_base *base = (struct event_base *)arg;

	evtimer_del(&pctl_event);
	memset(&pctl_event, 0, sizeof(pctl_event));
	fpm_pctl(FPM_PCTL_STATE_UNSPECIFIED, FPM_PCTL_ACTION_TIMEOUT, base);
}
/* }}} */

static int fpm_pctl_timeout_set(int sec, struct event_base *base) /* {{{ */
{
	struct timeval tv = { .tv_sec = sec, .tv_usec = 0 };

	if (evtimer_initialized(&pctl_event)) {
		evtimer_del(&pctl_event);
	}

	evtimer_set(&pctl_event, &fpm_pctl_action, base);
	event_base_set(base, &pctl_event);
	evtimer_add(&pctl_event, &tv);
	return 0;
}
/* }}} */

static void fpm_pctl_exit() /* {{{ */
{
	zlog(ZLOG_STUFF, ZLOG_NOTICE, "exiting, bye-bye!");

	fpm_conf_unlink_pid();
	fpm_cleanups_run(FPM_CLEANUP_PARENT_EXIT_MAIN);
	exit(0);
}
/* }}} */

#define optional_arg(c) (saved_argc > c ? ", \"" : ""), (saved_argc > c ? saved_argv[c] : ""), (saved_argc > c ? "\"" : "")

static void fpm_pctl_exec() /* {{{ */
{

	zlog(ZLOG_STUFF, ZLOG_NOTICE, "reloading: execvp(\"%s\", {\"%s\""
			"%s%s%s" "%s%s%s" "%s%s%s" "%s%s%s" "%s%s%s"
			"%s%s%s" "%s%s%s" "%s%s%s" "%s%s%s" "%s%s%s"
		"})",
		saved_argv[0], saved_argv[0],
		optional_arg(1),
		optional_arg(2),
		optional_arg(3),
		optional_arg(4),
		optional_arg(5),
		optional_arg(6),
		optional_arg(7),
		optional_arg(8),
		optional_arg(9),
		optional_arg(10)
	);

	fpm_cleanups_run(FPM_CLEANUP_PARENT_EXEC);
	execvp(saved_argv[0], saved_argv);
	zlog(ZLOG_STUFF, ZLOG_SYSERROR, "execvp() failed");
	exit(1);
}
/* }}} */

static void fpm_pctl_action_last() /* {{{ */
{
	switch (fpm_state) {
		case FPM_PCTL_STATE_RELOADING:
			fpm_pctl_exec();
			break;

		case FPM_PCTL_STATE_FINISHING:
		case FPM_PCTL_STATE_TERMINATING:
			fpm_pctl_exit();
			break;
	}
}
/* }}} */

int fpm_pctl_kill(pid_t pid, int how) /* {{{ */
{
	int s = 0;

	switch (how) {
		case FPM_PCTL_TERM :
			s = SIGTERM;
			break;
		case FPM_PCTL_STOP :
			s = SIGSTOP;
			break;
		case FPM_PCTL_CONT :
			s = SIGCONT;
			break;
		default :
			break;
	}
	return kill(pid, s);
}
/* }}} */

static void fpm_pctl_kill_all(int signo) /* {{{ */
{
	struct fpm_worker_pool_s *wp;
	int alive_children = 0;

	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		struct fpm_child_s *child;

		for (child = wp->children; child; child = child->next) {
			int res = kill(child->pid, signo);

			zlog(ZLOG_STUFF, ZLOG_DEBUG, "[pool %s] sending signal %d %s to child %d",
				child->wp->config->name, signo,
				fpm_signal_names[signo] ? fpm_signal_names[signo] : "", (int) child->pid);

			if (res == 0) {
				++alive_children;
			}
		}
	}

	if (alive_children) {
		zlog(ZLOG_STUFF, ZLOG_DEBUG, "%d child(ren) still alive", alive_children);
	}
}
/* }}} */

static void fpm_pctl_action_next(struct event_base *base) /* {{{ */
{
	int sig, timeout;

	if (!fpm_globals.running_children) {
		fpm_pctl_action_last();
	}

	if (fpm_signal_sent == 0) {
		if (fpm_state == FPM_PCTL_STATE_TERMINATING) {
			sig = SIGTERM;
		} else {
			sig = SIGQUIT;
		}
		timeout = fpm_global_config.process_control_timeout;
	} else {
		if (fpm_signal_sent == SIGQUIT) {
			sig = SIGTERM;
		} else {
			sig = SIGKILL;
		}
		timeout = 1;
	}

	fpm_pctl_kill_all(sig);
	fpm_signal_sent = sig;
	fpm_pctl_timeout_set(timeout, base);
}
/* }}} */

void fpm_pctl(int new_state, int action, struct event_base *base) /* {{{ */
{
	switch (action) {
		case FPM_PCTL_ACTION_SET :
			if (fpm_state == new_state) { /* already in progress - just ignore duplicate signal */
				return;
			}

			switch (fpm_state) { /* check which states can be overridden */
				case FPM_PCTL_STATE_NORMAL :
					/* 'normal' can be overridden by any other state */
					break;
				case FPM_PCTL_STATE_RELOADING :
					/* 'reloading' can be overridden by 'finishing' */
					if (new_state == FPM_PCTL_STATE_FINISHING) break;
				case FPM_PCTL_STATE_FINISHING :
					/* 'reloading' and 'finishing' can be overridden by 'terminating' */
					if (new_state == FPM_PCTL_STATE_TERMINATING) break;
				case FPM_PCTL_STATE_TERMINATING :
					/* nothing can override 'terminating' state */
					zlog(ZLOG_STUFF, ZLOG_DEBUG, "not switching to '%s' state, because already in '%s' state",
						fpm_state_names[new_state], fpm_state_names[fpm_state]);
					return;
			}

			fpm_signal_sent = 0;
			fpm_state = new_state;

			zlog(ZLOG_STUFF, ZLOG_DEBUG, "switching to '%s' state", fpm_state_names[fpm_state]);
			/* fall down */

		case FPM_PCTL_ACTION_TIMEOUT :
			fpm_pctl_action_next(base);
			break;
		case FPM_PCTL_ACTION_LAST_CHILD_EXITED :
			fpm_pctl_action_last();
			break;

	}
}
/* }}} */

int fpm_pctl_can_spawn_children() /* {{{ */
{
	return fpm_state == FPM_PCTL_STATE_NORMAL;
}
/* }}} */

int fpm_pctl_child_exited(struct event_base *base) /* {{{ */
{
	if (fpm_state == FPM_PCTL_STATE_NORMAL) {
		return 0;
	}

	if (!fpm_globals.running_children) {
		fpm_pctl(FPM_PCTL_STATE_UNSPECIFIED, FPM_PCTL_ACTION_LAST_CHILD_EXITED, base);
	}
	return 0;
}
/* }}} */

int fpm_pctl_init_main() /* {{{ */
{
	int i;

	saved_argc = fpm_globals.argc;
	saved_argv = malloc(sizeof(char *) * (saved_argc + 1));

	if (!saved_argv) {
		return -1;
	}

	for (i = 0; i < saved_argc; i++) {
		saved_argv[i] = strdup(fpm_globals.argv[i]);

		if (!saved_argv[i]) {
			return -1;
		}
	}

	saved_argv[i] = 0;

	if (0 > fpm_cleanup_add(FPM_CLEANUP_ALL, fpm_pctl_cleanup, 0)) {
		return -1;
	}
	return 0;
}
/* }}} */

static void fpm_pctl_check_request_timeout(struct timeval *now) /* {{{ */
{
	struct fpm_worker_pool_s *wp;

	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		int terminate_timeout = wp->config->request_terminate_timeout;
		int slowlog_timeout = wp->config->request_slowlog_timeout;
		struct fpm_child_s *child;

		if (terminate_timeout || slowlog_timeout) {
			for (child = wp->children; child; child = child->next) {
				fpm_request_check_timed_out(child, now, terminate_timeout, slowlog_timeout);
			}
		}
	}
}
/* }}} */

static void fpm_pctl_perform_idle_server_maintenance(struct timeval *now, struct event_base *base) /* {{{ */
{
	struct fpm_worker_pool_s *wp;
	struct fpm_child_s *last_idle_child = NULL;
	int i;

	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		struct fpm_child_s *child;
		int idle = 0;
		int active = 0;

		if (wp->config == NULL) continue;

		for (child = wp->children; child; child = child->next) {
			int ret = fpm_request_is_idle(child);
			if (ret == 1) {
				if (last_idle_child == NULL) {
					last_idle_child = child;
				} else {
					if (child->started.tv_sec < last_idle_child->started.tv_sec) {
						last_idle_child = child;
					}
				}
				idle++;
			} else if (ret == 0) {
				active++;
			}
		}

		if ((active + idle) != wp->running_children) {
			zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] unable to retrieve process activity of one or more child(ren). Will try again later.", wp->config->name);
			continue;
		}

		/* update status structure for all PMs */
		fpm_status_update_activity(wp->shm_status, idle, active, idle + active, 0);

		/* the rest is only used by PM_STYLE_DYNAMIC */
		if (wp->config->pm != PM_STYLE_DYNAMIC) continue;

		zlog(ZLOG_STUFF, ZLOG_DEBUG, "[pool %s] currently %d active children, %d spare children, %d running children. Spawning rate %d", wp->config->name, active, idle, wp->running_children, wp->idle_spawn_rate);

		if (idle > wp->config->pm_max_spare_servers && last_idle_child) {
			last_idle_child->idle_kill = 1;
			fpm_pctl_kill(last_idle_child->pid, FPM_PCTL_TERM);
			wp->idle_spawn_rate = 1;
			continue;
		}

		if (idle < wp->config->pm_min_spare_servers) {
			if (wp->running_children >= wp->config->pm_max_children) {
				if (!wp->warn_max_children) {
					zlog(ZLOG_STUFF, ZLOG_WARNING, "[pool %s] server reached max_children setting (%d), consider raising it", wp->config->name, wp->config->pm_max_children);
					wp->warn_max_children = 1;
				}
				wp->idle_spawn_rate = 1;
				continue;
			}

			if (wp->idle_spawn_rate >= 8) {
				zlog(ZLOG_STUFF, ZLOG_WARNING, "[pool %s] seems busy (you may need to increase start_servers, or min/max_spare_servers), spawning %d children, there are %d idle, and %d total children", wp->config->name, wp->idle_spawn_rate, idle, wp->running_children);
			}

			/* compute the number of idle process to spawn */
			i = MIN(wp->idle_spawn_rate, wp->config->pm_min_spare_servers - idle);

			/* get sure it won't exceed max_children */
			i = MIN(i, wp->config->pm_max_children - wp->running_children);
			if (i <= 0) {
				if (!wp->warn_max_children) {
					zlog(ZLOG_STUFF, ZLOG_WARNING, "[pool %s] server reached max_children setting (%d), consider raising it", wp->config->name, wp->config->pm_max_children);
					wp->warn_max_children = 1;
				}
				wp->idle_spawn_rate = 1;
				continue;
			}
			wp->warn_max_children = 0;

			fpm_children_make(wp, 1, i, 1, base);

			/* if it's a child, stop here without creating the next event
			 * this event is reserved to the master process
			 */
			if (fpm_globals.is_child) {
				return;
			}

			zlog(ZLOG_STUFF, ZLOG_DEBUG, "[pool %s] %d child(ren) have been created dynamically", wp->config->name, i);	

			/* Double the spawn rate for the next iteration */
			if (wp->idle_spawn_rate < FPM_MAX_SPAWN_RATE) {
				wp->idle_spawn_rate *= 2;
			}
			continue;
		}
		wp->idle_spawn_rate = 1;
	}
}
/* }}} */

void fpm_pctl_heartbeat(int fd, short which, void *arg) /* {{{ */
{
	static struct event heartbeat;
	struct timeval tv = { .tv_sec = 0, .tv_usec = 130000 };
	struct timeval now;
	struct event_base *base = (struct event_base *)arg;

	if (which == EV_TIMEOUT) {
		evtimer_del(&heartbeat);
		fpm_clock_get(&now);
		fpm_pctl_check_request_timeout(&now);
	}

	evtimer_set(&heartbeat, &fpm_pctl_heartbeat, base);
	event_base_set(base, &heartbeat);
	evtimer_add(&heartbeat, &tv);
}
/* }}} */

void fpm_pctl_perform_idle_server_maintenance_heartbeat(int fd, short which, void *arg) /* {{{ */
{
	static struct event heartbeat;
	struct timeval tv = { .tv_sec = 0, .tv_usec = FPM_IDLE_SERVER_MAINTENANCE_HEARTBEAT };
	struct timeval now;
	struct event_base *base = (struct event_base *)arg;

	if (which == EV_TIMEOUT) {
		evtimer_del(&heartbeat);
		fpm_clock_get(&now);
		if (fpm_pctl_can_spawn_children()) {
			fpm_pctl_perform_idle_server_maintenance(&now, base);

			/* if it's a child, stop here without creating the next event
			 * this event is reserved to the master process
			 */
			if (fpm_globals.is_child) {
				return;
			}
		}
	}

	evtimer_set(&heartbeat, &fpm_pctl_perform_idle_server_maintenance_heartbeat, base);
	event_base_set(base, &heartbeat);
	evtimer_add(&heartbeat, &tv);
}
/* }}} */

