	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fpm_env.h"
#include "fpm.h"

#ifndef HAVE_SETPROCTITLE
#ifdef __linux__
static char **fpm_env_argv = NULL;
static size_t fpm_env_argv_len = 0;
#endif
#endif

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

void fpm_env_setproctitle(char *title) /* {{{ */
{
#if defined(HAVE_SETPROCTITLE_FAST)
	setproctitle_fast("%s", title);
#elif defined(HAVE_SETPROCTITLE)
	setproctitle("%s", title);
#else
#ifdef __linux__
	if (fpm_env_argv != NULL && fpm_env_argv_len > strlen(SETPROCTITLE_PREFIX) + 3) {
		memset(fpm_env_argv[0], 0, fpm_env_argv_len);
		strncpy(fpm_env_argv[0], SETPROCTITLE_PREFIX, fpm_env_argv_len - 2);
		strncpy(fpm_env_argv[0] + strlen(SETPROCTITLE_PREFIX), title, fpm_env_argv_len - strlen(SETPROCTITLE_PREFIX) - 2);
		fpm_env_argv[1] = NULL;
	}
#endif
#endif
}
/* }}} */

int fpm_env_init_child(struct fpm_worker_pool_s *wp) /* {{{ */
{
	struct key_value_s *kv;
	char *title;
	spprintf(&title, 0, "pool %s", wp->config->name);
	fpm_env_setproctitle(title);
	efree(title);

	if (wp->config->clear_env) {
		clearenv();
	}

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
	int i;
	char *first = NULL;
	char *last = NULL;
	char *title;

	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		if (0 > fpm_env_conf_wp(wp)) {
			return -1;
		}
	}
#ifndef HAVE_SETPROCTITLE
#ifdef __linux__
	/*
	 * This piece of code has been inspirated from nginx and pureftpd code, which
	 * are under BSD licence.
	 *
	 * To change the process title in Linux we have to set argv[1] to NULL
	 * and to copy the title to the same place where the argv[0] points to.
	 * However, argv[0] may be too small to hold a new title.  Fortunately, Linux
	 * store argv[] and environ[] one after another.  So we should ensure that is
	 * the continuous memory and then we allocate the new memory for environ[]
	 * and copy it.  After this we could use the memory starting from argv[0] for
	 * our process title.
	 */

	for (i = 0; i < fpm_globals.argc; i++) {
		if (first == NULL) {
			first = fpm_globals.argv[i];
		}
		if (last == NULL || fpm_globals.argv[i] == last + 1) {
			last = fpm_globals.argv[i] + strlen(fpm_globals.argv[i]);
		}
	}
	if (environ) {
		for (i = 0; environ[i]; i++) {
			if (first == NULL) {
				first = environ[i];
			}
			if (last == NULL || environ[i] == last + 1) {
				last = environ[i] + strlen(environ[i]);
			}
		}
	}
	if (first == NULL || last == NULL) {
		return 0;
	}

	fpm_env_argv_len = last - first;
	fpm_env_argv = fpm_globals.argv;
	if (environ != NULL) {
		char **new_environ;
		unsigned int env_nb = 0U;

		while (environ[env_nb]) {
			env_nb++;
		}

		if ((new_environ = malloc((1U + env_nb) * sizeof (char *))) == NULL) {
			return -1;
		}
		new_environ[env_nb] = NULL;
		while (env_nb > 0U) {
			env_nb--;
			new_environ[env_nb] = strdup(environ[env_nb]);
		}
		environ = new_environ;
	}
#endif
#endif

	spprintf(&title, 0, "master process (%s)", fpm_globals.config);
	fpm_env_setproctitle(title);
	efree(title);
	return 0;
}
/* }}} */
