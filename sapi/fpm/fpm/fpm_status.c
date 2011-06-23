
	/* $Id$ */
	/* (c) 2009 Jerome Loyet */

#include "php.h"
#include "SAPI.h"
#include <stdio.h>

#include "fpm_config.h"
#include "fpm_status.h"
#include "fpm_clock.h"
#include "fpm_shm_slots.h"
#include "zlog.h"

struct fpm_shm_s *fpm_status_shm = NULL;
static char *fpm_status_pool = NULL;
static char *fpm_status_uri = NULL;
static char *fpm_status_ping_uri = NULL;
static char *fpm_status_ping_response = NULL;


int fpm_status_init_child(struct fpm_worker_pool_s *wp) /* {{{ */
{
	if (!wp || !wp->config) {
		zlog(ZLOG_ERROR, "unable to init fpm_status because conf structure is NULL");
		return -1;
	}
	if (wp->config->pm_status_path || wp->config->ping_path) {
		if (wp->config->pm_status_path) {
			if (!wp->shm_status) {
				zlog(ZLOG_ERROR, "[pool %s] unable to init fpm_status because the dedicated SHM has not been set", wp->config->name);
				return -1;
			}
			fpm_status_shm = wp->shm_status;
		}
		fpm_status_pool = strdup(wp->config->name);
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
	}
	return 0;
}
/* }}} */

void fpm_status_set_pm(struct fpm_shm_s *shm, int pm) /* {{{ */
{
	struct fpm_status_s status;

	if (!shm) shm = fpm_status_shm;
	if (!shm || !shm->mem) return;

	/* one shot operation */
	status = *(struct fpm_status_s *)shm->mem;

	status.pm = pm;

	/* one shot operation */
	*(struct fpm_status_s *)shm->mem = status;
}
/* }}} */

void fpm_status_increment_accepted_conn(struct fpm_shm_s *shm) /* {{{ */
{
	struct fpm_status_s status;

	if (!shm) shm = fpm_status_shm;
	if (!shm || !shm->mem) return;

	/* one shot operation */
	status = *(struct fpm_status_s *)shm->mem;

	status.accepted_conn++;

	/* one shot operation */
	*(struct fpm_status_s *)shm->mem = status;
}
/* }}} */

void fpm_status_update_accepted_conn(struct fpm_shm_s *shm, unsigned long int accepted_conn) /* {{{ */
{
	struct fpm_status_s status;

	if (!shm) shm = fpm_status_shm;
	if (!shm || !shm->mem) return;

	/* one shot operation */
	status = *(struct fpm_status_s *)shm->mem;

	status.accepted_conn = accepted_conn;

	/* one shot operation */
	*(struct fpm_status_s *)shm->mem = status;
}
/* }}} */

void fpm_status_increment_max_children_reached(struct fpm_shm_s *shm) /* {{{ */
{
	struct fpm_status_s status;

	if (!shm) shm = fpm_status_shm;
	if (!shm || !shm->mem) return;

	/* one shot operation */
	status = *(struct fpm_status_s *)shm->mem;

	status.max_children_reached++;

	/* one shot operation */
	*(struct fpm_status_s *)shm->mem = status;
}
/* }}} */

void fpm_status_update_max_children_reached(struct fpm_shm_s *shm, unsigned int max_children_reached) /* {{{ */
{
	struct fpm_status_s status;

	if (!shm) shm = fpm_status_shm;
	if (!shm || !shm->mem) return;

	/* one shot operation */
	status = *(struct fpm_status_s *)shm->mem;

	status.max_children_reached = max_children_reached;

	/* one shot operation */
	*(struct fpm_status_s *)shm->mem = status;
}
/* }}} */

void fpm_status_update_activity(struct fpm_shm_s *shm, int idle, int active, int total, unsigned cur_lq, int max_lq, int clear_last_update) /* {{{ */
{
	struct fpm_status_s status;

	if (!shm) shm = fpm_status_shm;
	if (!shm || !shm->mem) return;

	/* one shot operation */
	status = *(struct fpm_status_s *)shm->mem;

	status.idle = idle;
	status.active = active;
	status.total = total;
	status.cur_lq = cur_lq;
	status.max_lq = max_lq;
	if (clear_last_update) {
		memset(&status.last_update, 0, sizeof(status.last_update));
	} else {
		fpm_clock_get(&status.last_update);
	}

	/* one shot operation */
	*(struct fpm_status_s *)shm->mem = status;
}
/* }}} */

int fpm_status_handle_request(TSRMLS_D) /* {{{ */
{
	struct fpm_status_s status;
	char *buffer, *syntax;

	if (!SG(request_info).request_uri) {
		return 0;
	}

	/* PING */
	if (fpm_status_ping_uri && fpm_status_ping_response && !strcmp(fpm_status_ping_uri, SG(request_info).request_uri)) {
		sapi_add_header_ex(ZEND_STRL("Content-Type: text/plain"), 1, 1 TSRMLS_CC);
		SG(sapi_headers).http_response_code = 200;
		PUTS(fpm_status_ping_response);
		return 1;
	}

	/* STATUS */
	if (fpm_status_uri && !strcmp(fpm_status_uri, SG(request_info).request_uri)) {

		if (!fpm_status_shm || !fpm_status_shm->mem) {
			zlog(ZLOG_ERROR, "[pool %s] unable to find or access status shared memory", fpm_status_pool);
			SG(sapi_headers).http_response_code = 500;
			sapi_add_header_ex(ZEND_STRL("Content-Type: text/plain"), 1, 1 TSRMLS_CC);
			PUTS("Internal error. Please review log file for errors.");
			return 1;
		}

		/* one shot operation */
		status = *(struct fpm_status_s *)fpm_status_shm->mem;

		if (status.idle < 0 || status.active < 0 || status.total < 0) {
			zlog(ZLOG_ERROR, "[pool %s] invalid status values", fpm_status_pool);
			SG(sapi_headers).http_response_code = 500;
			sapi_add_header_ex(ZEND_STRL("Content-Type: text/plain"), 1, 1 TSRMLS_CC);
			PUTS("Internal error. Please review log file for errors.");
			return 1;
		}

		/* HTML */
		if (SG(request_info).query_string && strstr(SG(request_info).query_string, "html")) {
			sapi_add_header_ex(ZEND_STRL("Content-Type: text/html"), 1, 1 TSRMLS_CC);
			syntax =
				"<table>\n"
				"<tr><th>pool</th><td>%s</td></tr>\n"
				"<tr><th>process manager</th><td>%s</td></tr>\n"
				"<tr><th>accepted conn</th><td>%lu</td></tr>\n"
#if HAVE_FPM_LQ
				"<tr><th>listen queue len</th><td>%u</td></tr>\n"
				"<tr><th>max listen queue len</th><td>%d</td></tr>\n"
#endif
				"<tr><th>idle processes</th><td>%d</td></tr>\n"
				"<tr><th>active processes</th><td>%d</td></tr>\n"
				"<tr><th>total processes</th><td>%d</td></tr>\n"
				"<tr><th>max children reached</th><td>%u</td></tr>\n"
				"</table>";

		/* XML */
		} else if (SG(request_info).request_uri && strstr(SG(request_info).query_string, "xml")) {
			sapi_add_header_ex(ZEND_STRL("Content-Type: text/xml"), 1, 1 TSRMLS_CC);
			syntax =
				"<?xml version=\"1.0\" ?>\n"
				"<status>\n"
				"<pool>%s</pool>\n"
				"<process-manager>%s</process-manager>\n"
				"<accepted-conn>%lu</accepted-conn>\n"
#if HAVE_FPM_LQ
				"<listen-queue-len>%u</listen-queue-len>\n"
				"<max-listen-queue-len>%d</max-listen-queue-len>\n"
#endif
				"<idle-processes>%d</idle-processes>\n"
				"<active-processes>%d</active-processes>\n"
				"<total-processes>%d</total-processes>\n"
				"<max-children-reached>%u</max-children-reached>\n"
				"</status>";

			/* JSON */
		} else if (SG(request_info).request_uri && strstr(SG(request_info).query_string, "json")) {
			sapi_add_header_ex(ZEND_STRL("Content-Type: application/json"), 1, 1 TSRMLS_CC);

			syntax =
				"{"
				"\"pool\":\"%s\","
				"\"process manager\":\"%s\","
				"\"accepted conn\":%lu,"
#if HAVE_FPM_LQ
				"\"listen queue len\":%u,"
				"\"max listen queue len\":%d,"
#endif
				"\"idle processes\":%d,"
				"\"active processes\":%d,"
				"\"total processes\":%d,"
				"\"max children reached\":%u"
				"}";

		/* TEXT */
		} else {
			sapi_add_header_ex(ZEND_STRL("Content-Type: text/plain"), 1, 1 TSRMLS_CC);
			syntax =
				"pool:                 %s\n"
				"process manager:      %s\n"
				"accepted conn:        %lu\n"
#if HAVE_FPM_LQ
				"listen queue len:     %u\n"
				"max listen queue len: %d\n"
#endif
				"idle processes:       %d\n"
				"active processes:     %d\n"
				"total processes:      %d\n"
				"max children reached: %u\n";
		}

		spprintf(&buffer, 0, syntax,
				fpm_status_pool,
				status.pm == PM_STYLE_STATIC ? "static" : "dynamic",
				status.accepted_conn,
#if HAVE_FPM_LQ
				status.cur_lq,
				status.max_lq,
#endif
				status.idle,
				status.active,
				status.total,
				status.max_children_reached);

		SG(sapi_headers).http_response_code = 200;
		PUTS(buffer);
		efree(buffer);

		return 1;
	}

	return 0;
}
/* }}} */

