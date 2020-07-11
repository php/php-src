	/* (c) 2009 Jerome Loyet */

#ifndef FPM_LOG_H
#define FPM_LOG_H 1
#include "fpm_worker_pool.h"

int fpm_log_init_child(struct fpm_worker_pool_s *wp);
int fpm_log_write(char *log_format);
int fpm_log_open(int reopen);

#endif
