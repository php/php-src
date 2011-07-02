
	/* $Id: fpm_status.h 312263 2011-06-18 17:46:16Z felipe $ */
	/* (c) 2009 Jerome Loyet */

#ifndef FPM_LOG_H
#define FPM_LOG_H 1
#include "fpm_worker_pool.h"

int fpm_log_init_child(struct fpm_worker_pool_s *wp);
int fpm_log_write(char *log_format TSRMLS_DC);
int fpm_log_open(int reopen);

#endif
