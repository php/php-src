	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_STDIO_H
#define FPM_STDIO_H 1

#include <stdbool.h>
#include "fpm_worker_pool.h"

#define STREAM_SET_MSG_PREFIX_FMT "[pool %s] child %d said into %s: "

bool fpm_stdio_init_main(void);
bool fpm_stdio_init_final(void);
bool fpm_stdio_init_child(struct fpm_worker_pool_s *wp);
int fpm_stdio_flush_child(void);
bool fpm_stdio_prepare_pipes(struct fpm_child_s *child);
void fpm_stdio_child_use_pipes(struct fpm_child_s *child);
void fpm_stdio_parent_use_pipes(struct fpm_child_s *child);
void fpm_stdio_discard_pipes(struct fpm_child_s *child);
bool fpm_stdio_open_error_log(bool reopen);
bool fpm_stdio_save_original_stderr(void);
bool fpm_stdio_restore_original_stderr(bool close_after_restore);
void fpm_stdio_redirect_stderr_to_dev_null_for_syslog(void);
bool fpm_stdio_redirect_stderr_to_error_log(void);

#endif
