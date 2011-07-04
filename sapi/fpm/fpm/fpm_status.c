
	/* $Id$ */
	/* (c) 2009 Jerome Loyet */

#include "php.h"
#include "SAPI.h"
#include <stdio.h>

#include "fpm_config.h"
#include "fpm_scoreboard.h"
#include "fpm_status.h"
#include "fpm_clock.h"
#include "fpm_scoreboard.h"
#include "zlog.h"
#include "fpm_atomic.h"

static char *fpm_status_uri = NULL;
static char *fpm_status_ping_uri = NULL;
static char *fpm_status_ping_response = NULL;


int fpm_status_init_child(struct fpm_worker_pool_s *wp) /* {{{ */
{
	if (!wp || !wp->config) {
		zlog(ZLOG_ERROR, "unable to init fpm_status because conf structure is NULL");
		return -1;
	}

	if (wp->config->pm_status_path) {
		fpm_status_uri = strdup(wp->config->pm_status_path);
	}

	if (wp->config->ping_path) {
		if (!wp->config->ping_response) {
			zlog(ZLOG_ERROR, "[pool %s] ping is set (%s) but pong is not set.", wp->config->name, wp->config->ping_path);
			return -1;
		}
		fpm_status_ping_uri = strdup(wp->config->ping_path);
		fpm_status_ping_response = strdup(wp->config->ping_response);
	}

	return 0;
}
/* }}} */

int fpm_status_handle_request(TSRMLS_D) /* {{{ */
{
	struct fpm_scoreboard_s scoreboard, *scoreboard_p;
//	struct fpm_scoreboard_proc_s proc;
	char *buffer, *syntax, *time_format, time_buffer[64];
	time_t now_epoch;

	if (!SG(request_info).request_uri) {
		return 0;
	}

	/* PING */
	if (fpm_status_ping_uri && fpm_status_ping_response && !strcmp(fpm_status_ping_uri, SG(request_info).request_uri)) {
		sapi_add_header_ex(ZEND_STRL("Content-Type: text/plain"), 1, 1 TSRMLS_CC);
		SG(sapi_headers).http_response_code = 200;

		/* handle HEAD */
		if (SG(request_info).headers_only) {
			return 1;
		}

		PUTS(fpm_status_ping_response);
		return 1;
	}

	/* STATUS */
	if (fpm_status_uri && !strcmp(fpm_status_uri, SG(request_info).request_uri)) {

		scoreboard_p = fpm_scoreboard_get();
		if (!scoreboard_p) {
			zlog(ZLOG_ERROR, "status: unable to find or access status shared memory");
			SG(sapi_headers).http_response_code = 500;
			sapi_add_header_ex(ZEND_STRL("Content-Type: text/plain"), 1, 1 TSRMLS_CC);
			PUTS("Internal error. Please review log file for errors.");
			return 1;
		}

		if (!fpm_spinlock(&scoreboard_p->lock, 1)) {
			zlog(ZLOG_NOTICE, "[pool %s] status: scoreboard already in used.", scoreboard_p->pool);
			SG(sapi_headers).http_response_code = 503;
			sapi_add_header_ex(ZEND_STRL("Content-Type: text/plain"), 1, 1 TSRMLS_CC);
			PUTS("Server busy. Please try again later.");
			return 1;
		}
		/* copy the scoreboard not to bother other processes */
		scoreboard = *scoreboard_p;
		fpm_unlock(scoreboard_p->lock);

		if (scoreboard.idle < 0 || scoreboard.active < 0) {
			zlog(ZLOG_ERROR, "[pool %s] invalid status values", scoreboard.pool);
			SG(sapi_headers).http_response_code = 500;
			sapi_add_header_ex(ZEND_STRL("Content-Type: text/plain"), 1, 1 TSRMLS_CC);
			PUTS("Internal error. Please review log file for errors.");
			return 1;
		}

		/* send common headers */
		SG(sapi_headers).http_response_code = 200;

		/* handle HEAD */
		if (SG(request_info).headers_only) {
			return 1;
		}

		/* HTML */
		if (SG(request_info).query_string && strstr(SG(request_info).query_string, "html")) {
			sapi_add_header_ex(ZEND_STRL("Content-Type: text/html"), 1, 1 TSRMLS_CC);
			time_format = "%d/%b/%Y:%H:%M:%S %z";

			syntax =
				"<table>\n"
					"<tr><th>pool</th><td>%s</td></tr>\n"
					"<tr><th>process manager</th><td>%s</td></tr>\n"
					"<tr><th>start time</th><td>%s</td></tr>\n"
					"<tr><th>start since</th><td>%lu</td></tr>\n"
					"<tr><th>accepted conn</th><td>%lu</td></tr>\n"
#if HAVE_FPM_LQ
					"<tr><th>listen queue</th><td>%u</td></tr>\n"
					"<tr><th>max listen queue</th><td>%u</td></tr>\n"
					"<tr><th>listen queue len</th><td>%d</td></tr>\n"
#endif
					"<tr><th>idle processes</th><td>%d</td></tr>\n"
					"<tr><th>active processes</th><td>%d</td></tr>\n"
					"<tr><th>total processes</th><td>%d</td></tr>\n"
					"<tr><th>max active processes</th><td>%d</td></tr>\n"
					"<tr><th>max children reached</th><td>%u</td></tr>\n"
				"</table>\n";

		/* XML */
		} else if (SG(request_info).request_uri && strstr(SG(request_info).query_string, "xml")) {
			sapi_add_header_ex(ZEND_STRL("Content-Type: text/xml"), 1, 1 TSRMLS_CC);
			time_format = "%s";

			syntax =
				"<?xml version=\"1.0\" ?>\n"
				"<status>\n"
				"<pool>%s</pool>\n"
				"<process-manager>%s</process-manager>\n"
				"<start-time>%s</start-time>\n"
				"<start-since>%lu</start-since>\n"
				"<accepted-conn>%lu</accepted-conn>\n"
#if HAVE_FPM_LQ
				"<listen-queue>%u</listen-queue>\n"
				"<max-listen-queue>%u</max-listen-queue>\n"
				"<listen-queue-len>%d</listen-queue-len>\n"
#endif
				"<idle-processes>%d</idle-processes>\n"
				"<active-processes>%d</active-processes>\n"
				"<total-processes>%d</total-processes>\n"
				"<max-active-processes>%d</max-active-processes>\n"
				"<max-children-reached>%u</max-children-reached>\n"
				"</status>";

			/* JSON */
		} else if (SG(request_info).request_uri && strstr(SG(request_info).query_string, "json")) {
			sapi_add_header_ex(ZEND_STRL("Content-Type: application/json"), 1, 1 TSRMLS_CC);
			time_format = "%s";
			syntax =
				"{"
				"\"pool\":\"%s\","
				"\"process manager\":\"%s\","
				"\"start time\":%s,"
				"\"start since\":%lu,"
				"\"accepted conn\":%lu,"
#if HAVE_FPM_LQ
				"\"listen queue\":%u,"
				"\"max listen queue\":%u,"
				"\"listen queue len\":%d,"
#endif
				"\"idle processes\":%d,"
				"\"active processes\":%d,"
				"\"total processes\":%d,"
				"\"max active processes\":%d,"
				"\"max children reached\":%u"
				"}";

		/* TEXT */
		} else {
			sapi_add_header_ex(ZEND_STRL("Content-Type: text/plain"), 1, 1 TSRMLS_CC);
			time_format = "%d/%b/%Y:%H:%M:%S %z";
			syntax =
				"pool:                 %s\n"
				"process manager:      %s\n"
				"start time:           %s\n"
				"start since:          %lu\n"
				"accepted conn:        %lu\n"
#if HAVE_FPM_LQ
				"listen queue:         %u\n"
				"max listen queue:     %u\n"
				"listen queue len:     %d\n"
#endif
				"idle processes:       %d\n"
				"active processes:     %d\n"
				"total processes:      %d\n"
				"max active processes: %d\n"
				"max children reached: %u\n";
		}

		strftime(time_buffer, sizeof(time_buffer) - 1, time_format, localtime(&scoreboard.start_epoch));
		now_epoch = time(NULL);
		spprintf(&buffer, 0, syntax,
				scoreboard.pool,
				scoreboard.pm == PM_STYLE_STATIC ? "static" : "dynamic",
				time_buffer,
				now_epoch - scoreboard.start_epoch,
				scoreboard.requests,
#if HAVE_FPM_LQ
				scoreboard.lq,
				scoreboard.lq_max,
				scoreboard.lq_len,
#endif
				scoreboard.idle,
				scoreboard.active,
				scoreboard.idle + scoreboard.active,
				scoreboard.active_max,
				scoreboard.max_children_reached);

		PUTS(buffer);
		efree(buffer);

		return 1;
	}

	return 0;
}
/* }}} */

