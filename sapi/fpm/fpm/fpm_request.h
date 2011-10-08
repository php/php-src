
	/* $Id: fpm_request.h,v 1.4 2008/07/20 01:47:16 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_REQUEST_H
#define FPM_REQUEST_H 1

void fpm_request_accepting();				/* hanging in accept() */
void fpm_request_reading_headers();			/* start reading fastcgi request from very first byte */
void fpm_request_info();					/* not a stage really but a point in the php code, where all request params have become known to sapi */
void fpm_request_executing();				/* the script is executing */
void fpm_request_end(TSRMLS_D);				/* request ended: script response have been sent to web server */
void fpm_request_finished();				/* request processed: cleaning current request */

struct fpm_child_s;
struct timeval;

void fpm_request_check_timed_out(struct fpm_child_s *child, struct timeval *tv, int terminate_timeout, int slowlog_timeout);
int fpm_request_is_idle(struct fpm_child_s *child);
const char *fpm_request_get_stage_name(int stage);
int fpm_request_last_activity(struct fpm_child_s *child, struct timeval *tv);

enum fpm_request_stage_e {
	FPM_REQUEST_ACCEPTING = 1,
	FPM_REQUEST_READING_HEADERS,
	FPM_REQUEST_INFO,
	FPM_REQUEST_EXECUTING,
	FPM_REQUEST_END,
	FPM_REQUEST_FINISHED
};

#endif
