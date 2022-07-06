	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_ENV_H
#define FPM_ENV_H 1

#include "fpm_worker_pool.h"

#define SETPROCTITLE_PREFIX "php-fpm: "

int fpm_env_init_child(struct fpm_worker_pool_s *wp);
int fpm_env_init_main();
void fpm_env_setproctitle(char *title);

extern char **environ;

#ifndef HAVE_SETENV
int setenv(char *name, char *value, int overwrite);
#endif

#ifndef HAVE_CLEARENV
void clearenv();
#endif

#endif
