#include "fpm_config.h"

#include <sys/types.h>
#include <systemd/sd-daemon.h>

#include "fpm.h"
#include "fpm_clock.h"
#include "fpm_worker_pool.h"
#include "fpm_scoreboard.h"
#include "zlog.h"
#include "fpm_systemd.h"


static void fpm_systemd() /* {{{ */
{
	static unsigned long int last=0;
	struct fpm_worker_pool_s *wp;
	unsigned long int requests=0, slow_req=0;
	int active=0, idle=0;


	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		if (wp->scoreboard) {
			active   += wp->scoreboard->active;
			idle     += wp->scoreboard->idle;
			requests += wp->scoreboard->requests;
			slow_req += wp->scoreboard->slow_rq;
		}
	}

/*
	zlog(ZLOG_DEBUG, "systemd %s (Processes active:%d, idle:%d, Requests:%lu, slow:%lu, Traffic:%.3greq/sec)",
			fpm_global_config.systemd_watchdog ? "watchdog" : "heartbeat",
			active, idle, requests, slow_req, ((float)requests - last) * 1000.0 / fpm_global_config.systemd_interval);
*/

	if (0 > sd_notifyf(0, "READY=1\n%s"
				"STATUS=Processes active: %d, idle: %d, Requests: %lu, slow: %lu, Traffic: %.3greq/sec",
				fpm_global_config.systemd_watchdog ? "WATCHDOG=1\n" : "",
				active, idle, requests, slow_req, ((float)requests - last) * 1000.0 / fpm_global_config.systemd_interval)) {
		zlog(ZLOG_NOTICE, "failed to notify status to systemd");
	}

	last = requests;
}
/* }}} */

void fpm_systemd_heartbeat(struct fpm_event_s *ev, short which, void *arg) /* {{{ */
{
	static struct fpm_event_s heartbeat;

	if (fpm_globals.parent_pid != getpid()) {
		return; /* sanity check */
	}

	if (which == FPM_EV_TIMEOUT) {
		fpm_systemd();

		return;
	}

	if (0 > sd_notifyf(0, "READY=1\n"
			          "STATUS=Ready to handle connections\n"
			          "MAINPID=%lu",
			          (unsigned long) getpid())) {
		zlog(ZLOG_WARNING, "failed to notify start to systemd");
	} else {
		zlog(ZLOG_DEBUG, "have notify start to systemd");
	}

	/* first call without setting which to initialize the timer */
	if (fpm_global_config.systemd_interval > 0) {
		fpm_event_set_timer(&heartbeat, FPM_EV_PERSIST, &fpm_systemd_heartbeat, NULL);
		fpm_event_add(&heartbeat, fpm_global_config.systemd_interval);
		zlog(ZLOG_NOTICE, "systemd monitor interval set to %dms", fpm_global_config.systemd_interval);
	} else {
		zlog(ZLOG_NOTICE, "systemd monitor disabled");
	}
}
/* }}} */

int fpm_systemd_conf() /* {{{ */
{
	char *watchdog;
	int  interval = 0;

	watchdog = getenv("WATCHDOG_USEC");
	if (watchdog) {
		/* usec to msec, and half the configured delay */
		interval = (int)(atol(watchdog) / 2000L);
		zlog(ZLOG_DEBUG, "WATCHDOG_USEC=%s, interval=%d", watchdog, interval);
	}

	if (interval > 1000) {
		if (fpm_global_config.systemd_interval > 0) {
			zlog(ZLOG_WARNING, "systemd_interval option ignored");
		}
		zlog(ZLOG_NOTICE, "systemd watchdog configured to %.3gsec", (float)interval / 1000.0);
		fpm_global_config.systemd_watchdog = 1;
		fpm_global_config.systemd_interval = interval;

	} else if (fpm_global_config.systemd_interval < 0) {
		/* not set => default value */
		fpm_global_config.systemd_interval = FPM_SYSTEMD_DEFAULT_HEARTBEAT;

	} else {
		/* sec to msec */
		fpm_global_config.systemd_interval *= 1000;
	}
	return 0;
}
/* }}} */

