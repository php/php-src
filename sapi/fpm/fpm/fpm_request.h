	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_REQUEST_H
#define FPM_REQUEST_H 1

/* hanging in accept() */
void fpm_request_accepting();
/* start reading fastcgi request from very first byte */
void fpm_request_reading_headers();
/* not a stage really but a point in the php code, where all request params have become known to sapi */
void fpm_request_info();
/* the script is executing */
void fpm_request_executing();
/* request ended: script response have been sent to web server */
void fpm_request_end(void);
/* request processed: cleaning current request */
void fpm_request_finished();

struct fpm_child_s;
struct timeval;

void fpm_request_check_timed_out(struct fpm_child_s *child, struct timeval *tv, int terminate_timeout, int slowlog_timeout, int track_finished);
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
