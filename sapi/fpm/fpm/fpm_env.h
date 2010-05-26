
	/* $Id: fpm_env.h,v 1.9 2008/09/18 23:19:59 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_ENV_H
#define FPM_ENV_H 1

#include "fpm_worker_pool.h"

int fpm_env_init_child(struct fpm_worker_pool_s *wp);
int fpm_env_init_main();

extern char **environ;

#ifndef HAVE_SETENV
int setenv(char *name, char *value, int overwrite);
#endif

#ifndef HAVE_CLEARENV
void clearenv();
#endif

#endif

