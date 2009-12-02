
	/* $Id: fpm_php.h,v 1.10.2.1 2008/11/15 00:57:24 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_PHP_H
#define FPM_PHP_H 1

#include <TSRM.h>

#include "build-defs.h" /* for PHP_ defines */

struct fpm_worker_pool_s;

int fpm_php_init_child(struct fpm_worker_pool_s *wp);
char *fpm_php_script_filename(TSRMLS_D);
char *fpm_php_request_method(TSRMLS_D);
size_t fpm_php_content_length(TSRMLS_D);
void fpm_php_soft_quit();
int fpm_php_init_main();

#endif

