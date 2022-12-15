	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_UNIX_H
#define FPM_UNIX_H 1

#include <stdbool.h>
#include "fpm_worker_pool.h"

bool fpm_unix_resolve_socket_permissions(struct fpm_worker_pool_s *wp);
bool fpm_unix_set_socket_permissions(struct fpm_worker_pool_s *wp, const char *path);
bool fpm_unix_free_socket_permissions(struct fpm_worker_pool_s *wp);

bool fpm_unix_init_child(struct fpm_worker_pool_s *wp);
bool fpm_unix_init_main(void);

extern size_t fpm_pagesize;

#endif
