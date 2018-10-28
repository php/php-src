	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_STDIO_H
#define FPM_STDIO_H 1

#include "fpm_worker_pool.h"

int fpm_stdio_init_main();
int fpm_stdio_init_final();
int fpm_stdio_init_child(struct fpm_worker_pool_s *wp);
int fpm_stdio_flush_child();
int fpm_stdio_prepare_pipes(struct fpm_child_s *child);
void fpm_stdio_child_use_pipes(struct fpm_child_s *child);
int fpm_stdio_parent_use_pipes(struct fpm_child_s *child);
int fpm_stdio_discard_pipes(struct fpm_child_s *child);
int fpm_stdio_open_error_log(int reopen);

#endif
