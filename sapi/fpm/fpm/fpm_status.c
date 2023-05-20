	/* (c) 2009 Jerome Loyet */

#include "php.h"
#include "zend_long.h"
#include "SAPI.h"
#include <stdio.h>

#include "fpm_config.h"
#include "fpm_scoreboard.h"
#include "fpm_status.h"
#include "fpm_clock.h"
#include "zlog.h"
#include "fpm_atomic.h"
#include "fpm_conf.h"
#include "fpm_php.h"
#include "ext/standard/html.h"
#include "ext/json/php_json.h"

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
			zlog(ZLOG_ERROR, "[pool %s] ping is set (%s) but ping.response is not set.", wp->config->name, wp->config->ping_path);
			return -1;
		}
		fpm_status_ping_uri = strdup(wp->config->ping_path);
		fpm_status_ping_response = strdup(wp->config->ping_response);
	}

	return 0;
}
/* }}} */

int fpm_status_export_to_zval(zval *status)
{
	struct fpm_scoreboard_s scoreboard, *scoreboard_p;
	zval fpm_proc_stats, fpm_proc_stat;
	time_t now_epoch;
	struct timeval duration, now;
	double cpu;
	int i;

	scoreboard_p = fpm_scoreboard_acquire(NULL, 1);
	if (!scoreboard_p) {
		zlog(ZLOG_NOTICE, "[pool %s] status: scoreboard already in use.", scoreboard_p->pool);
		return -1;
	}

	/* copy the scoreboard not to bother other processes */
	scoreboard = *scoreboard_p;
	struct fpm_scoreboard_proc_s procs[scoreboard.nprocs];

	struct fpm_scoreboard_proc_s *proc_p;
	for(i=0; i<scoreboard.nprocs; i++) {
		proc_p = fpm_scoreboard_proc_acquire(scoreboard_p, i, 1);
		if (!proc_p){
			procs[i].used=-1;
			continue;
		}
		procs[i] = *proc_p;
		fpm_scoreboard_proc_release(proc_p);
	}
	fpm_scoreboard_release(scoreboard_p);

	now_epoch = time(NULL);
	fpm_clock_get(&now);

	array_init(status);
	add_assoc_string(status, "pool", scoreboard.pool);
	add_assoc_string(status, "process-manager", PM2STR(scoreboard.pm));
	add_assoc_long(status, "start-time", scoreboard.start_epoch);
	add_assoc_long(status, "start-since", now_epoch - scoreboard.start_epoch);
	add_assoc_long(status, "accepted-conn", scoreboard.requests);
	add_assoc_long(status, "listen-queue", scoreboard.lq);
	add_assoc_long(status, "max-listen-queue", scoreboard.lq_max);
	add_assoc_long(status, "listen-queue-len", scoreboard.lq_len);
	add_assoc_long(status, "idle-processes", scoreboard.idle);
	add_assoc_long(status, "active-processes", scoreboard.active);
	add_assoc_long(status, "total-processes", scoreboard.idle + scoreboard.active);
	add_assoc_long(status, "max-active-processes", scoreboard.active_max);
	add_assoc_long(status, "max-children-reached", scoreboard.max_children_reached);
	add_assoc_long(status, "slow-requests", scoreboard.slow_rq);

	array_init(&fpm_proc_stats);
	for(i=0; i<scoreboard.nprocs; i++) {
		if (!procs[i].used) {
			continue;
		}
		proc_p = &procs[i];
		/* prevent NaN */
		if (procs[i].cpu_duration.tv_sec == 0 && procs[i].cpu_duration.tv_usec == 0) {
			cpu = 0.;
		} else {
			cpu = (procs[i].last_request_cpu.tms_utime + procs[i].last_request_cpu.tms_stime + procs[i].last_request_cpu.tms_cutime + procs[i].last_request_cpu.tms_cstime) / fpm_scoreboard_get_tick() / (procs[i].cpu_duration.tv_sec + procs[i].cpu_duration.tv_usec / 1000000.) * 100.;
		}

		array_init(&fpm_proc_stat);
		add_assoc_long(&fpm_proc_stat, "pid", procs[i].pid);
		add_assoc_string(&fpm_proc_stat, "state", fpm_request_get_stage_name(procs[i].request_stage));
		add_assoc_long(&fpm_proc_stat, "start-time", procs[i].start_epoch);
		add_assoc_long(&fpm_proc_stat, "start-since", now_epoch - procs[i].start_epoch);
		add_assoc_long(&fpm_proc_stat, "requests", procs[i].requests);
		if (procs[i].request_stage == FPM_REQUEST_ACCEPTING) {
			duration = procs[i].duration;
		} else {
			timersub(&now, &procs[i].accepted, &duration);
		}
		add_assoc_long(&fpm_proc_stat, "request-duration", duration.tv_sec * 1000000UL + duration.tv_usec);
		add_assoc_string(&fpm_proc_stat, "request-method", procs[i].request_method[0] != '\0' ? procs[i].request_method : "-");
		add_assoc_string(&fpm_proc_stat, "request-uri", procs[i].request_uri);
		add_assoc_string(&fpm_proc_stat, "query-string", procs[i].query_string);
		add_assoc_long(&fpm_proc_stat, "request-length", procs[i].content_length);
		add_assoc_string(&fpm_proc_stat, "user", procs[i].auth_user[0] != '\0' ? procs[i].auth_user : "-");
		add_assoc_string(&fpm_proc_stat, "script", procs[i].script_filename[0] != '\0' ? procs[i].script_filename : "-");
		add_assoc_double(&fpm_proc_stat, "last-request-cpu", procs[i].request_stage == FPM_REQUEST_ACCEPTING ? cpu : 0.);
		add_assoc_long(&fpm_proc_stat, "last-request-memory", procs[i].request_stage == FPM_REQUEST_ACCEPTING ? procs[i].memory : 0);
		add_next_index_zval(&fpm_proc_stats, &fpm_proc_stat);
	}
	add_assoc_zval(status, "procs", &fpm_proc_stats);
	return 0;
}
/* }}} */

int fpm_status_handle_request(void) /* {{{ */
{
	struct fpm_scoreboard_s *scoreboard_p;
	struct fpm_scoreboard_proc_s *proc;
	char *buffer, *time_format, time_buffer[64];
	time_t now_epoch;
	int full, has_start_time;
	bool encode_html, encode_json;
	char *short_syntax, *short_post;
	char *full_pre, *full_syntax, *full_post, *full_separator;

	if (!SG(request_info).request_uri) {
		return 0;
	}

	/* PING */
	if (fpm_status_ping_uri && fpm_status_ping_response && !strcmp(fpm_status_ping_uri, SG(request_info).request_uri)) {
		fpm_request_executing();
		sapi_add_header_ex(ZEND_STRL("Content-Type: text/plain"), 1, 1);
		sapi_add_header_ex(ZEND_STRL("Expires: Thu, 01 Jan 1970 00:00:00 GMT"), 1, 1);
		sapi_add_header_ex(ZEND_STRL("Cache-Control: no-cache, no-store, must-revalidate, max-age=0"), 1, 1);
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
		zend_string *_GET_str;

		fpm_request_executing();

		/* full status ? */
		_GET_str = ZSTR_INIT_LITERAL("_GET", 0);
		full = fpm_php_is_key_in_table(_GET_str, ZEND_STRL("full"));
		short_syntax = short_post = NULL;
		full_separator = full_pre = full_syntax = full_post = NULL;
		encode_html = false;
		encode_json = false;
		has_start_time = 1;

		scoreboard_p = fpm_scoreboard_get();
		if (scoreboard_p) {
			scoreboard_p = fpm_scoreboard_copy(scoreboard_p->shared ? scoreboard_p->shared : scoreboard_p, full);
		}
		if (!scoreboard_p) {
			zlog(ZLOG_ERROR, "status: unable to find or access status shared memory");
			SG(sapi_headers).http_response_code = 500;
			sapi_add_header_ex(ZEND_STRL("Content-Type: text/plain"), 1, 1);
			sapi_add_header_ex(ZEND_STRL("Expires: Thu, 01 Jan 1970 00:00:00 GMT"), 1, 1);
			sapi_add_header_ex(ZEND_STRL("Cache-Control: no-cache, no-store, must-revalidate, max-age=0"), 1, 1);
			PUTS("Internal error. Please review log file for errors.");
			return 1;
		}

		if (scoreboard_p->idle < 0 || scoreboard_p->active < 0) {
			fpm_scoreboard_free_copy(scoreboard_p);
			zlog(ZLOG_ERROR, "[pool %s] invalid status values", scoreboard_p->pool);
			SG(sapi_headers).http_response_code = 500;
			sapi_add_header_ex(ZEND_STRL("Content-Type: text/plain"), 1, 1);
			sapi_add_header_ex(ZEND_STRL("Expires: Thu, 01 Jan 1970 00:00:00 GMT"), 1, 1);
			sapi_add_header_ex(ZEND_STRL("Cache-Control: no-cache, no-store, must-revalidate, max-age=0"), 1, 1);
			PUTS("Internal error. Please review log file for errors.");
			return 1;
		}

		/* send common headers */
		sapi_add_header_ex(ZEND_STRL("Expires: Thu, 01 Jan 1970 00:00:00 GMT"), 1, 1);
		sapi_add_header_ex(ZEND_STRL("Cache-Control: no-cache, no-store, must-revalidate, max-age=0"), 1, 1);
		SG(sapi_headers).http_response_code = 200;

		/* handle HEAD */
		if (SG(request_info).headers_only) {
			fpm_scoreboard_free_copy(scoreboard_p);
			return 1;
		}

		/* HTML */
		if (fpm_php_is_key_in_table(_GET_str, ZEND_STRL("html"))) {
			sapi_add_header_ex(ZEND_STRL("Content-Type: text/html"), 1, 1);
			time_format = "%d/%b/%Y:%H:%M:%S %z";
			encode_html = true;

			short_syntax =
				"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
				"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
				"<head><title>PHP-FPM Status Page</title></head>\n"
				"<body>\n"
				"<table>\n"
					"<tr><th>pool</th><td>%s</td></tr>\n"
					"<tr><th>process manager</th><td>%s</td></tr>\n"
					"<tr><th>start time</th><td>%s</td></tr>\n"
					"<tr><th>start since</th><td>%lu</td></tr>\n"
					"<tr><th>accepted conn</th><td>%lu</td></tr>\n"
					"<tr><th>listen queue</th><td>%d</td></tr>\n"
					"<tr><th>max listen queue</th><td>%d</td></tr>\n"
					"<tr><th>listen queue len</th><td>%u</td></tr>\n"
					"<tr><th>idle processes</th><td>%d</td></tr>\n"
					"<tr><th>active processes</th><td>%d</td></tr>\n"
					"<tr><th>total processes</th><td>%d</td></tr>\n"
					"<tr><th>max active processes</th><td>%d</td></tr>\n"
					"<tr><th>max children reached</th><td>%u</td></tr>\n"
					"<tr><th>slow requests</th><td>%lu</td></tr>\n"
				"</table>\n";

			if (!full) {
				short_post = "</body></html>";
			} else {
				full_pre =
					"<table border=\"1\">\n"
					"<tr>"
						"<th>pid</th>"
						"<th>state</th>"
						"<th>start time</th>"
						"<th>start since</th>"
						"<th>requests</th>"
						"<th>request duration</th>"
						"<th>request method</th>"
						"<th>request uri</th>"
						"<th>content length</th>"
						"<th>user</th>"
						"<th>script</th>"
						"<th>last request cpu</th>"
						"<th>last request memory</th>"
					"</tr>\n";

				full_syntax =
					"<tr>"
						"<td>%d</td>"
						"<td>%s</td>"
						"<td>%s</td>"
						"<td>%lu</td>"
						"<td>%lu</td>"
						"<td>%lu</td>"
						"<td>%s</td>"
						"<td>%s%s%s</td>"
						"<td>%zu</td>"
						"<td>%s</td>"
						"<td>%s</td>"
						"<td>%.2f</td>"
						"<td>%zu</td>"
					"</tr>\n";

				full_post = "</table></body></html>";
			}

		/* XML */
		} else if (fpm_php_is_key_in_table(_GET_str, ZEND_STRL("xml"))) {
			sapi_add_header_ex(ZEND_STRL("Content-Type: text/xml"), 1, 1);
			time_format = "%s";
			encode_html = true;

			short_syntax =
				"<?xml version=\"1.0\" ?>\n"
				"<status>\n"
				"<pool>%s</pool>\n"
				"<process-manager>%s</process-manager>\n"
				"<start-time>%s</start-time>\n"
				"<start-since>%lu</start-since>\n"
				"<accepted-conn>%lu</accepted-conn>\n"
				"<listen-queue>%d</listen-queue>\n"
				"<max-listen-queue>%d</max-listen-queue>\n"
				"<listen-queue-len>%u</listen-queue-len>\n"
				"<idle-processes>%d</idle-processes>\n"
				"<active-processes>%d</active-processes>\n"
				"<total-processes>%d</total-processes>\n"
				"<max-active-processes>%d</max-active-processes>\n"
				"<max-children-reached>%u</max-children-reached>\n"
				"<slow-requests>%lu</slow-requests>\n";

				if (!full) {
					short_post = "</status>";
				} else {
					full_pre = "<processes>\n";
					full_syntax =
						"<process>"
							"<pid>%d</pid>"
							"<state>%s</state>"
							"<start-time>%s</start-time>"
							"<start-since>%lu</start-since>"
							"<requests>%lu</requests>"
							"<request-duration>%lu</request-duration>"
							"<request-method>%s</request-method>"
							"<request-uri>%s%s%s</request-uri>"
							"<content-length>%zu</content-length>"
							"<user>%s</user>"
							"<script>%s</script>"
							"<last-request-cpu>%.2f</last-request-cpu>"
							"<last-request-memory>%zu</last-request-memory>"
						"</process>\n"
					;
					full_post = "</processes>\n</status>";
				}

			/* JSON */
		} else if (fpm_php_is_key_in_table(_GET_str, ZEND_STRL("json"))) {
			sapi_add_header_ex(ZEND_STRL("Content-Type: application/json"), 1, 1);
			time_format = "%s";

			encode_json = true;

			short_syntax =
				"{"
				"\"pool\":\"%s\","
				"\"process manager\":\"%s\","
				"\"start time\":%s,"
				"\"start since\":%lu,"
				"\"accepted conn\":%lu,"
				"\"listen queue\":%d,"
				"\"max listen queue\":%d,"
				"\"listen queue len\":%u,"
				"\"idle processes\":%d,"
				"\"active processes\":%d,"
				"\"total processes\":%d,"
				"\"max active processes\":%d,"
				"\"max children reached\":%u,"
				"\"slow requests\":%lu";

			if (!full) {
				short_post = "}";
			} else {
				full_separator = ",";
				full_pre = ", \"processes\":[";

				full_syntax = "{"
					"\"pid\":%d,"
					"\"state\":\"%s\","
					"\"start time\":%s,"
					"\"start since\":%lu,"
					"\"requests\":%lu,"
					"\"request duration\":%lu,"
					"\"request method\":\"%s\","
					"\"request uri\":\"%s%s%s\","
					"\"content length\":%zu,"
					"\"user\":\"%s\","
					"\"script\":\"%s\","
					"\"last request cpu\":%.2f,"
					"\"last request memory\":%zu"
					"}";

				full_post = "]}";
			}

			/* OpenMetrics */
		} else if (fpm_php_is_key_in_table(_GET_str, ZEND_STRL("openmetrics"))) {
			sapi_add_header_ex(ZEND_STRL("Content-Type: application/openmetrics-text; version=1.0.0; charset=utf-8"), 1, 1);
			time_format = "%s";

			short_syntax =
				"# HELP phpfpm_up Could pool %s using a %s PM on PHP-FPM be reached?\n"
				"# TYPE phpfpm_up gauge\n"
				"phpfpm_up 1\n"
				"# HELP phpfpm_start_since The number of seconds since FPM has started.\n"
				"# TYPE phpfpm_start_since counter\n"
				"phpfpm_start_since %lu\n"
				"# HELP phpfpm_accepted_connections The number of requests accepted by the pool.\n"
				"# TYPE phpfpm_accepted_connections counter\n"
				"phpfpm_accepted_connections %lu\n"
				"# HELP phpfpm_listen_queue The number of requests in the queue of pending connections.\n"
				"# TYPE phpfpm_listen_queue gauge\n"
				"phpfpm_listen_queue %d\n"
				"# HELP phpfpm_max_listen_queue The maximum number of requests in the queue of pending connections since FPM has started.\n"
				"# TYPE phpfpm_max_listen_queue counter\n"
				"phpfpm_max_listen_queue %d\n"
				"# TYPE phpfpm_listen_queue_length gauge\n"
				"# HELP phpfpm_listen_queue_length The size of the socket queue of pending connections.\n"
				"phpfpm_listen_queue_length %u\n"
				"# HELP phpfpm_idle_processes The number of idle processes.\n"
				"# TYPE phpfpm_idle_processes gauge\n"
				"phpfpm_idle_processes %d\n"
				"# HELP phpfpm_active_processes The number of active processes.\n"
				"# TYPE phpfpm_active_processes gauge\n"
				"phpfpm_active_processes %d\n"
				"# HELP phpfpm_total_processes The number of idle + active processes.\n"
				"# TYPE phpfpm_total_processes gauge\n"
				"phpfpm_total_processes %d\n"
				"# HELP phpfpm_max_active_processes The maximum number of active processes since FPM has started.\n"
				"# TYPE phpfpm_max_active_processes counter\n"
				"phpfpm_max_active_processes %d\n"
				"# HELP phpfpm_max_children_reached The number of times, the process limit has been reached, when pm tries to start more children (works only for pm 'dynamic' and 'ondemand').\n"
				"# TYPE phpfpm_max_children_reached counter\n"
				"phpfpm_max_children_reached %u\n"
				"# HELP phpfpm_slow_requests The number of requests that exceeded your 'request_slowlog_timeout' value.\n"
				"# TYPE phpfpm_slow_requests counter\n"
				"phpfpm_slow_requests %lu\n"
				"# EOF\n";

			has_start_time = 0;
			if (!full) {
				short_post = "";
			} else {
				full_separator = "";
				full_pre = "";
				full_syntax = "";
				full_post = "";
			}

		/* TEXT */
		} else {
			sapi_add_header_ex(ZEND_STRL("Content-Type: text/plain"), 1, 1);
			time_format = "%d/%b/%Y:%H:%M:%S %z";

			short_syntax =
				"pool:                 %s\n"
				"process manager:      %s\n"
				"start time:           %s\n"
				"start since:          %lu\n"
				"accepted conn:        %lu\n"
				"listen queue:         %d\n"
				"max listen queue:     %d\n"
				"listen queue len:     %u\n"
				"idle processes:       %d\n"
				"active processes:     %d\n"
				"total processes:      %d\n"
				"max active processes: %d\n"
				"max children reached: %u\n"
				"slow requests:        %lu\n";

				if (full) {
					full_syntax =
						"\n"
						"************************\n"
						"pid:                  %d\n"
						"state:                %s\n"
						"start time:           %s\n"
						"start since:          %lu\n"
						"requests:             %lu\n"
						"request duration:     %lu\n"
						"request method:       %s\n"
						"request URI:          %s%s%s\n"
						"content length:       %zu\n"
						"user:                 %s\n"
						"script:               %s\n"
						"last request cpu:     %.2f\n"
						"last request memory:  %zu\n";
				}
		}

		now_epoch = time(NULL);
		if (has_start_time) {
			strftime(time_buffer, sizeof(time_buffer) - 1, time_format, localtime(&scoreboard_p->start_epoch));
			spprintf(&buffer, 0, short_syntax,
					scoreboard_p->pool,
					PM2STR(scoreboard_p->pm),
					time_buffer,
					(unsigned long) (now_epoch - scoreboard_p->start_epoch),
					scoreboard_p->requests,
					scoreboard_p->lq,
					scoreboard_p->lq_max,
					scoreboard_p->lq_len,
					scoreboard_p->idle,
					scoreboard_p->active,
					scoreboard_p->idle + scoreboard_p->active,
					scoreboard_p->active_max,
					scoreboard_p->max_children_reached,
					scoreboard_p->slow_rq);
		} else {
			spprintf(&buffer, 0, short_syntax,
					scoreboard_p->pool,
					PM2STR(scoreboard_p->pm),
					(unsigned long) (now_epoch - scoreboard_p->start_epoch),
					scoreboard_p->requests,
					scoreboard_p->lq,
					scoreboard_p->lq_max,
					scoreboard_p->lq_len,
					scoreboard_p->idle,
					scoreboard_p->active,
					scoreboard_p->idle + scoreboard_p->active,
					scoreboard_p->active_max,
					scoreboard_p->max_children_reached,
					scoreboard_p->slow_rq);
		}

		PUTS(buffer);
		efree(buffer);
		zend_string_release_ex(_GET_str, 0);

		if (short_post) {
			PUTS(short_post);
		}

		/* no need to test the var 'full' */
		if (full_syntax) {
			unsigned int i;
			int first;
			zend_string *tmp_query_string;
			char *query_string;
			struct timeval duration, now;
			float cpu;

			fpm_clock_get(&now);

			if (full_pre) {
				PUTS(full_pre);
			}

			first = 1;
			for (i=0; i<scoreboard_p->nprocs; i++) {
				if (!scoreboard_p->procs[i].used) {
					continue;
				}
				proc = &scoreboard_p->procs[i];

				if (first) {
					first = 0;
				} else {
					if (full_separator) {
						PUTS(full_separator);
					}
				}

				query_string = NULL;
				tmp_query_string = NULL;
				if (proc->query_string[0] != '\0') {
					if (encode_html) {
						tmp_query_string = php_escape_html_entities_ex(
								(const unsigned char *) proc->query_string,
								strlen(proc->query_string), 1, ENT_HTML_IGNORE_ERRORS & ENT_COMPAT,
								NULL, /* double_encode */ 1, /* quiet */ 0);
					} else if (encode_json) {
						tmp_query_string = php_json_encode_string(proc->query_string,
								strlen(proc->query_string), PHP_JSON_INVALID_UTF8_IGNORE);
					} else {
						query_string = proc->query_string;
					}
					if (tmp_query_string) {
						query_string = ZSTR_VAL(tmp_query_string);
						/* remove quotes around the string */
						if (encode_json && ZSTR_LEN(tmp_query_string) >= 2) {
							query_string[ZSTR_LEN(tmp_query_string) - 1] = '\0';
							++query_string;
						}
					}
				}

				/* prevent NaN */
				if (proc->cpu_duration.tv_sec == 0 && proc->cpu_duration.tv_usec == 0) {
					cpu = 0.;
				} else {
					cpu = (proc->last_request_cpu.tms_utime + proc->last_request_cpu.tms_stime + proc->last_request_cpu.tms_cutime + proc->last_request_cpu.tms_cstime) / fpm_scoreboard_get_tick() / (proc->cpu_duration.tv_sec + proc->cpu_duration.tv_usec / 1000000.) * 100.;
				}

				if (proc->request_stage == FPM_REQUEST_ACCEPTING) {
					duration = proc->duration;
				} else {
					timersub(&now, &proc->accepted, &duration);
				}
				strftime(time_buffer, sizeof(time_buffer) - 1, time_format, localtime(&proc->start_epoch));
				spprintf(&buffer, 0, full_syntax,
					(int) proc->pid,
					fpm_request_get_stage_name(proc->request_stage),
					time_buffer,
					(unsigned long) (now_epoch - proc->start_epoch),
					proc->requests,
					duration.tv_sec * 1000000UL + duration.tv_usec,
					proc->request_method[0] != '\0' ? proc->request_method : "-",
					proc->request_uri[0] != '\0' ? proc->request_uri : "-",
					query_string ? "?" : "",
					query_string ? query_string : "",
					proc->content_length,
					proc->auth_user[0] != '\0' ? proc->auth_user : "-",
					proc->script_filename[0] != '\0' ? proc->script_filename : "-",
					proc->request_stage == FPM_REQUEST_ACCEPTING ? cpu : 0.,
					proc->request_stage == FPM_REQUEST_ACCEPTING ? proc->memory : 0);
				PUTS(buffer);
				efree(buffer);

				if (tmp_query_string) {
					zend_string_free(tmp_query_string);
				}
			}

			if (full_post) {
				PUTS(full_post);
			}
		}

		fpm_scoreboard_free_copy(scoreboard_p);
		return 1;
	}

	return 0;
}
/* }}} */
