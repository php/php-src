	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include "zend_time.h"

#include "fpm.h"
#include "fpm_php.h"
#include "fpm_str.h"
#include "fpm_conf.h"
#include "fpm_trace.h"
#include "fpm_php_trace.h"
#include "fpm_process_ctl.h"
#include "fpm_children.h"
#include "fpm_scoreboard.h"
#include "fpm_status.h"
#include "fpm_request.h"
#include "fpm_log.h"

#include "zlog.h"

static const char *requests_stages[] = {
	[FPM_REQUEST_CREATING]        = "Creating",
	[FPM_REQUEST_ACCEPTING]       = "Idle",
	[FPM_REQUEST_READING_HEADERS] = "Reading headers",
	[FPM_REQUEST_INFO]            = "Getting request information",
	[FPM_REQUEST_EXECUTING]       = "Running",
	[FPM_REQUEST_END]             = "Ending",
	[FPM_REQUEST_FINISHED]        = "Finishing",
};

const char *fpm_request_get_stage_name(int stage) {
	return requests_stages[stage];
}

void fpm_request_accepting(void)
{
	struct fpm_scoreboard_proc_s *proc;

	fpm_scoreboard_update_begin(NULL);

	proc = fpm_scoreboard_proc_acquire(NULL, -1, 0);
	if (proc == NULL) {
		zlog(ZLOG_WARNING, "failed to acquire proc scoreboard");
		return;
	}

	proc->request_stage = FPM_REQUEST_ACCEPTING;
	proc->last_activity_ns = zend_time_mono_fallback();
	fpm_scoreboard_proc_release(proc);

	/* idle++, active-- */
	fpm_scoreboard_update_commit(1, -1, 0, 0, 0, 0, 0, 0, FPM_SCOREBOARD_ACTION_INC, NULL);
}

void fpm_request_reading_headers(void)
{
	struct fpm_scoreboard_proc_s *proc;

	uint64_t now_ns;
	time_t now_epoch;
#ifdef HAVE_TIMES
	struct tms cpu;
#endif

	now_ns = zend_time_mono_fallback();
	now_epoch = zend_time_real_get();
#ifdef HAVE_TIMES
	times(&cpu);
#endif

	fpm_scoreboard_update_begin(NULL);

	proc = fpm_scoreboard_proc_acquire(NULL, -1, 0);
	if (proc == NULL) {
		zlog(ZLOG_WARNING, "failed to acquire proc scoreboard");
		return;
	}

	proc->request_stage = FPM_REQUEST_READING_HEADERS;
	proc->last_activity_ns = now_ns;
	proc->accepted_ns = now_ns;
	proc->accepted_epoch = now_epoch;
#ifdef HAVE_TIMES
	proc->cpu_accepted = cpu;
#endif
	proc->requests++;
	proc->request_uri[0] = '\0';
	proc->request_method[0] = '\0';
	proc->script_filename[0] = '\0';
	proc->query_string[0] = '\0';
	proc->auth_user[0] = '\0';
	proc->content_length = 0;
	fpm_scoreboard_proc_release(proc);

	/* idle--, active++, request++ */
	fpm_scoreboard_update_commit(-1, 1, 0, 0, 1, 0, 0, 0, FPM_SCOREBOARD_ACTION_INC, NULL);
}

void fpm_request_info(void)
{
	struct fpm_scoreboard_proc_s *proc;
	char *request_uri = fpm_php_request_uri();
	char *request_method = fpm_php_request_method();
	char *script_filename = fpm_php_script_filename();
	char *query_string = fpm_php_query_string();
	char *auth_user = fpm_php_auth_user();
	size_t content_length = fpm_php_content_length();
	uint64_t now_ns = zend_time_mono_fallback();

	proc = fpm_scoreboard_proc_acquire(NULL, -1, 0);
	if (proc == NULL) {
		zlog(ZLOG_WARNING, "failed to acquire proc scoreboard");
		return;
	}

	proc->request_stage = FPM_REQUEST_INFO;
	proc->last_activity_ns = now_ns;

	if (request_uri) {
		strlcpy(proc->request_uri, request_uri, sizeof(proc->request_uri));
	}

	if (request_method) {
		strlcpy(proc->request_method, request_method, sizeof(proc->request_method));
	}

	if (query_string) {
		strlcpy(proc->query_string, query_string, sizeof(proc->query_string));
	}

	if (auth_user) {
		strlcpy(proc->auth_user, auth_user, sizeof(proc->auth_user));
	}

	proc->content_length = content_length;

	/* if cgi.fix_pathinfo is set to "1" and script cannot be found (404)
		the sapi_globals.request_info.path_translated is set to NULL */
	if (script_filename) {
		strlcpy(proc->script_filename, script_filename, sizeof(proc->script_filename));
	}

	fpm_scoreboard_proc_release(proc);
}

void fpm_request_executing(void)
{
	struct fpm_scoreboard_proc_s *proc;
	uint64_t now_ns = zend_time_mono_fallback();

	proc = fpm_scoreboard_proc_acquire(NULL, -1, 0);
	if (proc == NULL) {
		zlog(ZLOG_WARNING, "failed to acquire proc scoreboard");
		return;
	}

	proc->request_stage = FPM_REQUEST_EXECUTING;
	proc->last_activity_ns = now_ns;
	fpm_scoreboard_proc_release(proc);
}

void fpm_request_end(void)
{
	struct fpm_scoreboard_proc_s *proc;
	uint64_t now_ns = zend_time_mono_fallback();
#ifdef HAVE_TIMES
	struct tms cpu;
#endif
	size_t memory = zend_memory_peak_usage(1);
#ifdef HAVE_TIMES
	times(&cpu);
#endif

	proc = fpm_scoreboard_proc_acquire(NULL, -1, 0);
	if (proc == NULL) {
		zlog(ZLOG_WARNING, "failed to acquire proc scoreboard");
		return;
	}
	proc->request_stage = FPM_REQUEST_FINISHED;
	proc->last_activity_ns = now_ns;
	proc->duration_ns = now_ns - proc->accepted_ns;
#ifdef HAVE_TIMES
	proc->cpu_duration_ns = now_ns - proc->accepted_ns;
	proc->last_request_cpu.tms_utime = cpu.tms_utime - proc->cpu_accepted.tms_utime;
	proc->last_request_cpu.tms_stime = cpu.tms_stime - proc->cpu_accepted.tms_stime;
	proc->last_request_cpu.tms_cutime = cpu.tms_cutime - proc->cpu_accepted.tms_cutime;
	proc->last_request_cpu.tms_cstime = cpu.tms_cstime - proc->cpu_accepted.tms_cstime;
#endif
	proc->memory = memory;
	fpm_scoreboard_proc_release(proc);

	/* memory_peak */
	fpm_scoreboard_update_commit(-1, -1, -1, -1, -1, -1, -1, proc->memory, FPM_SCOREBOARD_ACTION_SET, NULL);
}

void fpm_request_finished(void)
{
	struct fpm_scoreboard_proc_s *proc;

	proc = fpm_scoreboard_proc_acquire(NULL, -1, 0);
	if (proc == NULL) {
		zlog(ZLOG_WARNING, "failed to acquire proc scoreboard");
		return;
	}

	proc->request_stage = FPM_REQUEST_FINISHED;
	proc->last_activity_ns = zend_time_mono_fallback();
	fpm_scoreboard_proc_release(proc);
}

void fpm_request_check_timed_out(struct fpm_child_s *child, int terminate_timeout, int slowlog_timeout, int track_finished) /* {{{ */
{
	struct fpm_scoreboard_proc_s proc, *proc_p;
	uint64_t now_ns;

	proc_p = fpm_scoreboard_proc_acquire(child->wp->scoreboard, child->scoreboard_i, 1);
	if (!proc_p) {
		zlog(ZLOG_NOTICE, "failed to acquire scoreboard");
		return;
	}

	proc = *proc_p;
	fpm_scoreboard_proc_release(proc_p);

	now_ns = zend_time_mono_fallback();

#if HAVE_FPM_TRACE
	if (child->slow_logged_ns && child->slow_logged_ns != proc.accepted_ns) {
		child->slow_logged_ns = 0;
	}
#endif

	if (proc.request_stage > FPM_REQUEST_ACCEPTING && ((proc.request_stage < FPM_REQUEST_END) || track_finished)) {
		char purified_script_filename[sizeof(proc.script_filename)];
		uint64_t elapsed_ns = now_ns - proc.accepted_ns;
		long elapsed_sec = (long) (elapsed_ns / ZEND_NANO_IN_SEC);

#if HAVE_FPM_TRACE
		if (child->slow_logged_ns == 0 && slowlog_timeout &&
				proc.request_stage == FPM_REQUEST_EXECUTING && elapsed_sec >= slowlog_timeout) {

			str_purify_filename(purified_script_filename, proc.script_filename, sizeof(proc.script_filename));

			child->slow_logged_ns = proc.accepted_ns;
			child->tracer = fpm_php_trace;

			fpm_trace_signal(child->pid);

			zlog(ZLOG_WARNING, "[pool %s] child %d, script '%s' (request: \"%s %s%s%s\") executing too slow (%ld.%09ld sec), logging",
				child->wp->config->name, (int) child->pid, purified_script_filename, proc.request_method, proc.request_uri,
				(proc.query_string[0] ? "?" : ""), proc.query_string,
				elapsed_sec, (long) (elapsed_ns % ZEND_NANO_IN_SEC));
		}
		else
#endif
		if (terminate_timeout && elapsed_sec >= terminate_timeout) {
			str_purify_filename(purified_script_filename, proc.script_filename, sizeof(proc.script_filename));
			fpm_pctl_kill(child->pid, FPM_PCTL_TERM);

			zlog(ZLOG_WARNING, "[pool %s] child %d, script '%s' (request: \"%s %s%s%s\") execution timed out (%ld.%09ld sec), terminating",
				child->wp->config->name, (int) child->pid, purified_script_filename, proc.request_method, proc.request_uri,
				(proc.query_string[0] ? "?" : ""), proc.query_string,
				elapsed_sec, (long) (elapsed_ns % ZEND_NANO_IN_SEC));
		}
	}
}
/* }}} */

int fpm_request_is_idle(struct fpm_child_s *child) /* {{{ */
{
	struct fpm_scoreboard_proc_s *proc;

	/* no need in atomicity here */
	proc = fpm_scoreboard_proc_get_from_child(child);
	if (!proc) {
		return 0;
	}

	return proc->request_stage == FPM_REQUEST_ACCEPTING;
}
/* }}} */

int fpm_request_last_activity(struct fpm_child_s *child, uint64_t *last_activity_ns) /* {{{ */
{
	struct fpm_scoreboard_proc_s *proc;

	if (!last_activity_ns) return -1;

	proc = fpm_scoreboard_proc_get_from_child(child);
	if (!proc) {
		return -1;
	}

	*last_activity_ns = proc->last_activity_ns;

	return 1;
}
/* }}} */
