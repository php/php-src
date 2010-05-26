
	/* $Id: fpm_env.c,v 1.15 2008/09/18 23:19:59 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fpm_env.h"
#include "zlog.h"

#ifndef HAVE_SETENV
# ifdef (__sparc__ || __sparc)
int setenv(char *name, char *value, int clobber) /* {{{ */
{
	char   *malloc();
	char   *getenv();
	char   *cp;

	if (clobber == 0 && getenv(name) != 0) {
		return 0;
	}
	
	if ((cp = malloc(strlen(name) + strlen(value) + 2)) == 0) {
		return 1;
	}
	sprintf(cp, "%s=%s", name, value);
	return putenv(cp);
}
/* }}} */
# else
int setenv(char *name, char *value, int overwrite) /* {{{ */
{
	int name_len = strlen(name);
	int value_len = strlen(value);
	char *var = alloca(name_len + 1 + value_len + 1);

	memcpy(var, name, name_len);

	var[name_len] = '=';

	memcpy(var + name_len + 1, value, value_len);

	var[name_len + 1 + value_len] = '\0';

	return putenv(var);
}
/* }}} */
# endif
#endif

#ifndef HAVE_CLEARENV
void clearenv() /* {{{ */
{
	char **envp;
	char *s;

	/* this algo is the only one known to me
		that works well on all systems */
	while (*(envp = environ)) {
		char *eq = strchr(*envp, '=');

		s = strdup(*envp);

		if (eq) s[eq - *envp] = '\0';

		unsetenv(s);
		free(s);
	}

}
/* }}} */
#endif

#ifndef HAVE_UNSETENV
void unsetenv(const char *name) /* {{{ */
{
	if(getenv(name) != NULL) {
		int ct = 0;
		int del = 0;

		while(environ[ct] != NULL) {
			if (nvmatch(name, environ[ct]) != 0) del=ct; /* <--- WTF?! */
			{ ct++; } /* <--- WTF?! */
		}
		/* isn't needed free here?? */
		environ[del] = environ[ct-1];
		environ[ct-1] = NULL;
	}
}
/* }}} */

static char * nvmatch(char *s1, char *s2) /* {{{ */
{
	while(*s1 == *s2++)
	{
		if(*s1++ == '=') { 
			return s2;
		}
	}
	if(*s1 == '\0' && *(s2-1) == '=') { 
		return s2;
	}
	return NULL;
}
/* }}} */
#endif

int fpm_env_init_child(struct fpm_worker_pool_s *wp) /* {{{ */
{
	struct key_value_s *kv;

	clearenv();

	for (kv = wp->config->env; kv; kv = kv->next) {
		setenv(kv->key, kv->value, 1);
	}

	if (wp->user) {
		setenv("USER", wp->user, 1);
	}

	if (wp->home) {
		setenv("HOME", wp->home, 1);
	}

	return 0;
}
/* }}} */

static int fpm_env_conf_wp(struct fpm_worker_pool_s *wp) /* {{{ */
{
	struct key_value_s *kv;

	for (kv = wp->config->env; kv; kv = kv->next) {
		if (*kv->value == '$') {
			char *value = getenv(kv->value + 1);

			if (!value) {
				value = "";
			}

			free(kv->value);
			kv->value = strdup(value);
		}

		/* autodetected values should be removed
			if these vars specified in config */
		if (!strcmp(kv->key, "USER")) {
			free(wp->user);
			wp->user = 0;
		}

		if (!strcmp(kv->key, "HOME")) {
			free(wp->home);
			wp->home = 0;
		}
	}

	return 0;
}
/* }}} */

int fpm_env_init_main() /* {{{ */
{
	struct fpm_worker_pool_s *wp;

	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		if (0 > fpm_env_conf_wp(wp)) {
			return -1;
		}
	}
	return 0;
}
/* }}} */

