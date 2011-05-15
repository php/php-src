
	/* $Id$ */
	/* (c) 2009 Jerome Loyet */

#include "php.h"
#include <stdio.h>

#include "fpm_config.h"
#include "fpm_status.h"
#include "fpm_clock.h"
#include "zlog.h"

struct fpm_shm_s *fpm_status_shm = NULL;
static char *fpm_status_pool = NULL;
static char *fpm_status_uri = NULL;
static char *fpm_status_ping= NULL;
static char *fpm_status_pong= NULL;


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
			fpm_status_ping = strdup(wp->config->ping_path);
			fpm_status_pong = strdup(wp->config->ping_response);
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

static void fpm_status_handle_status_txt(struct fpm_status_s *status, char **output, char **content_type) /* {{{ */
{
	if (!status || !output || !content_type) {
		return;
	}

	spprintf(output, 0, 
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
		"max children reached: %u\n",
		fpm_status_pool,
		status->pm == PM_STYLE_STATIC ? "static" : "dynamic",
		status->accepted_conn,
#if HAVE_FPM_LQ
		status->cur_lq,
		status->max_lq,
#endif
		status->idle,
		status->active,
		status->total,
		status->max_children_reached);

	spprintf(content_type, 0, "Content-Type: text/plain");
}
/* }}} */

static void fpm_status_handle_status_html(struct fpm_status_s *status, char **output, char **content_type) /* {{{ */
{
	if (!status || !output || !content_type) {
		return;
	}

	spprintf(output, 0, 
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
		"</table>",
		fpm_status_pool,
		status->pm == PM_STYLE_STATIC ? "static" : "dynamic",
		status->accepted_conn,
#if HAVE_FPM_LQ
		status->cur_lq,
		status->max_lq,
#endif
		status->idle,
		status->active,
		status->total,
		status->max_children_reached);

	spprintf(content_type, 0, "Content-Type: text/html");
}
/* }}} */

static void fpm_status_handle_status_json(struct fpm_status_s *status, char **output, char **content_type) /* {{{ */
{
	if (!status || !output || !content_type) {
		return;
	}

	spprintf(output, 0, 
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
		"}",
		fpm_status_pool,
		status->pm == PM_STYLE_STATIC ? "static" : "dynamic",
		status->accepted_conn,
#if HAVE_FPM_LQ
		status->cur_lq,
		status->max_lq,
#endif
		status->idle,
		status->active,
		status->total,
		status->max_children_reached);

	spprintf(content_type, 0, "Content-Type: application/json");
}
/* }}} */

/* return 0 if it's not the request page
 * return 1 if ouput has been set)
 * *output unchanged: error (return 500)
 * *output changed: no error (return 200)
 */
int fpm_status_handle_status(char *uri, char *query_string, char **output, char **content_type) /* {{{ */
{
	struct fpm_status_s status;

	if (!fpm_status_uri || !uri) {
		return 0;
	}

	/* It's not the status page */
	if (strcmp(fpm_status_uri, uri)) {
		return 0;
	}

	if (!output || !content_type || !fpm_status_shm) {
		return 1;
	}

	if (!fpm_status_shm->mem) {
		return 1;
	}

	/* one shot operation */
	status = *(struct fpm_status_s *)fpm_status_shm->mem;

	if (status.idle < 0 || status.active < 0 || status.total < 0) {
		return 1;
	}

	if (query_string && strstr(query_string, "html")) {
		fpm_status_handle_status_html(&status, output, content_type);
	} else if (query_string && strstr(query_string, "json")) {
		fpm_status_handle_status_json(&status, output, content_type);
	} else {
		fpm_status_handle_status_txt(&status, output, content_type);
	}

	if (!*output || !content_type) {
		zlog(ZLOG_ERROR, "[pool %s] unable to allocate status ouput buffer", fpm_status_pool);
		return 1;
	}

	return 1;
}
/* }}} */

char *fpm_status_handle_ping(char *uri) /* {{{ */
{
	if (!fpm_status_ping || !fpm_status_pong || !uri) {
		return NULL;
	}

	/* It's not the status page */
	if (strcmp(fpm_status_ping, uri)) {
		return NULL;
	}

	return fpm_status_pong;
}
/* }}} */

