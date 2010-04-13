
	/* $Id: fpm_request.c,v 1.9.2.1 2008/11/15 00:57:24 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include "fpm.h"
#include "fpm_php.h"
#include "fpm_str.h"
#include "fpm_clock.h"
#include "fpm_conf.h"
#include "fpm_trace.h"
#include "fpm_php_trace.h"
#include "fpm_process_ctl.h"
#include "fpm_children.h"
#include "fpm_shm_slots.h"
#include "fpm_status.h"
#include "fpm_request.h"

#include "zlog.h"

void fpm_request_accepting() /* {{{ */
{
	struct fpm_shm_slot_s *slot;

	slot = fpm_shm_slots_acquire(0, 0);
	slot->request_stage = FPM_REQUEST_ACCEPTING;
	fpm_clock_get(&slot->tv);
	memset(slot->request_method, 0, sizeof(slot->request_method));
	slot->content_length = 0;
	memset(slot->script_filename, 0, sizeof(slot->script_filename));
	fpm_shm_slots_release(slot);
}
/* }}} */

void fpm_request_reading_headers() /* {{{ */
{
	struct fpm_shm_slot_s *slot;

	slot = fpm_shm_slots_acquire(0, 0);
	slot->request_stage = FPM_REQUEST_READING_HEADERS;
	fpm_clock_get(&slot->tv);
	slot->accepted = slot->tv;
	fpm_shm_slots_release(slot);

	fpm_status_increment_accepted_conn(fpm_status_shm);
}
/* }}} */

void fpm_request_info() /* {{{ */
{
	TSRMLS_FETCH();
	struct fpm_shm_slot_s *slot;
	char *request_method = fpm_php_request_method(TSRMLS_C);
	char *script_filename = fpm_php_script_filename(TSRMLS_C);

	slot = fpm_shm_slots_acquire(0, 0);
	slot->request_stage = FPM_REQUEST_INFO;
	fpm_clock_get(&slot->tv);

	if (request_method) {
		cpystrn(slot->request_method, request_method, sizeof(slot->request_method));
	}

	slot->content_length = fpm_php_content_length(TSRMLS_C);

	/* if cgi.fix_pathinfo is set to "1" and script cannot be found (404)
		the sapi_globals.request_info.path_translated is set to NULL */
	if (script_filename) {
		cpystrn(slot->script_filename, script_filename, sizeof(slot->script_filename));
	}

	fpm_shm_slots_release(slot);
}
/* }}} */

void fpm_request_executing() /* {{{ */
{
	struct fpm_shm_slot_s *slot;

	slot = fpm_shm_slots_acquire(0, 0);
	slot->request_stage = FPM_REQUEST_EXECUTING;
	fpm_clock_get(&slot->tv);
	fpm_shm_slots_release(slot);
}
/* }}} */

void fpm_request_finished() /* {{{ */
{
	struct fpm_shm_slot_s *slot;

	slot = fpm_shm_slots_acquire(0, 0);
	slot->request_stage = FPM_REQUEST_FINISHED;
	fpm_clock_get(&slot->tv);
	memset(&slot->accepted, 0, sizeof(slot->accepted));
	fpm_shm_slots_release(slot);
}
/* }}} */

void fpm_request_check_timed_out(struct fpm_child_s *child, struct timeval *now, int terminate_timeout, int slowlog_timeout) /* {{{ */
{
	struct fpm_shm_slot_s *slot;
	struct fpm_shm_slot_s slot_c;

	slot = fpm_shm_slot(child);
	if (!fpm_shm_slots_acquire(slot, 1)) {
		return;
	}

	slot_c = *slot;
	fpm_shm_slots_release(slot);

#if HAVE_FPM_TRACE
	if (child->slow_logged.tv_sec) {
		if (child->slow_logged.tv_sec != slot_c.accepted.tv_sec || child->slow_logged.tv_usec != slot_c.accepted.tv_usec) {
			child->slow_logged.tv_sec = 0;
			child->slow_logged.tv_usec = 0;
		}
	}
#endif

	if (slot_c.request_stage > FPM_REQUEST_ACCEPTING && slot_c.request_stage < FPM_REQUEST_FINISHED) {
		char purified_script_filename[sizeof(slot_c.script_filename)];
		struct timeval tv;

		timersub(now, &slot_c.accepted, &tv);

#if HAVE_FPM_TRACE
		if (child->slow_logged.tv_sec == 0 && slowlog_timeout &&
				slot_c.request_stage == FPM_REQUEST_EXECUTING && tv.tv_sec >= slowlog_timeout) {
			
			str_purify_filename(purified_script_filename, slot_c.script_filename, sizeof(slot_c.script_filename));

			child->slow_logged = slot_c.accepted;
			child->tracer = fpm_php_trace;

			fpm_trace_signal(child->pid);

			zlog(ZLOG_STUFF, ZLOG_WARNING, "[pool %s] child %d, script '%s' executing too slow (%d.%06d sec), logging",
				child->wp->config->name, (int) child->pid, purified_script_filename, (int) tv.tv_sec, (int) tv.tv_usec);
		}
		else
#endif
		if (terminate_timeout && tv.tv_sec >= terminate_timeout) {
			str_purify_filename(purified_script_filename, slot_c.script_filename, sizeof(slot_c.script_filename));
			fpm_pctl_kill(child->pid, FPM_PCTL_TERM);

			zlog(ZLOG_STUFF, ZLOG_WARNING, "[pool %s] child %d, script '%s' execution timed out (%d.%06d sec), terminating",
				child->wp->config->name, (int) child->pid, purified_script_filename, (int) tv.tv_sec, (int) tv.tv_usec);
		}
	}
}
/* }}} */

int fpm_request_is_idle(struct fpm_child_s *child) /* {{{ */
{
	struct fpm_shm_slot_s *slot;
	struct fpm_shm_slot_s slot_c;

	slot = fpm_shm_slot(child);
	if (!fpm_shm_slots_acquire(slot, 1)) {
		return -1;
	}

	slot_c = *slot;
	fpm_shm_slots_release(slot);
	return(!slot_c.accepted.tv_sec && !slot_c.accepted.tv_usec ? 1 : 0);
}
/* }}} */
