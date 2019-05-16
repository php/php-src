	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "php.h"
#include "php_main.h"
#include "php_ini.h"
#include "ext/standard/dl.h"

#include "fastcgi.h"

#include "fpm.h"
#include "fpm_php.h"
#include "fpm_cleanup.h"
#include "fpm_worker_pool.h"
#include "zlog.h"

static char **limit_extensions = NULL;

static int fpm_php_zend_ini_alter_master(char *name, int name_length, char *new_value, int new_value_length, int mode, int stage) /* {{{ */
{
	zend_ini_entry *ini_entry;
	zend_string *duplicate;

	if ((ini_entry = zend_hash_str_find_ptr(EG(ini_directives), name, name_length)) == NULL) {
		return FAILURE;
	}

	duplicate = zend_string_init(new_value, new_value_length, 1);

	if (!ini_entry->on_modify
			|| ini_entry->on_modify(ini_entry, duplicate,
				ini_entry->mh_arg1, ini_entry->mh_arg2, ini_entry->mh_arg3, stage) == SUCCESS) {
		ini_entry->value = duplicate;
		/* when mode == ZEND_INI_USER keep unchanged to allow ZEND_INI_PERDIR (.user.ini) */
		if (mode == ZEND_INI_SYSTEM) {
			ini_entry->modifiable = mode;
		}
	} else {
		zend_string_release_ex(duplicate, 1);
	}

	return SUCCESS;
}
/* }}} */

static void fpm_php_disable(char *value, int (*zend_disable)(char *, size_t)) /* {{{ */
{
	char *s = 0, *e = value;

	while (*e) {
		switch (*e) {
			case ' ':
			case ',':
				if (s) {
					*e = '\0';
					zend_disable(s, e - s);
					s = 0;
				}
				break;
			default:
				if (!s) {
					s = e;
				}
				break;
		}
		e++;
	}

	if (s) {
		zend_disable(s, e - s);
	}
}
/* }}} */

int fpm_php_apply_defines_ex(struct key_value_s *kv, int mode) /* {{{ */
{

	char *name = kv->key;
	char *value = kv->value;
	int name_len = strlen(name);
	int value_len = strlen(value);

	if (!strcmp(name, "extension") && *value) {
		zval zv;
		php_dl(value, MODULE_PERSISTENT, &zv, 1);
		return Z_TYPE(zv) == IS_TRUE;
	}

	if (fpm_php_zend_ini_alter_master(name, name_len, value, value_len, mode, PHP_INI_STAGE_ACTIVATE) == FAILURE) {
		return -1;
	}

	if (!strcmp(name, "disable_functions") && *value) {
		char *v = strdup(value);
		PG(disable_functions) = v;
		fpm_php_disable(v, zend_disable_function);
		return 1;
	}

	if (!strcmp(name, "disable_classes") && *value) {
		char *v = strdup(value);
		PG(disable_classes) = v;
		fpm_php_disable(v, zend_disable_class);
		return 1;
	}

	return 1;
}
/* }}} */

static int fpm_php_apply_defines(struct fpm_worker_pool_s *wp) /* {{{ */
{
	struct key_value_s *kv;

	for (kv = wp->config->php_values; kv; kv = kv->next) {
		if (fpm_php_apply_defines_ex(kv, ZEND_INI_USER) == -1) {
			zlog(ZLOG_ERROR, "Unable to set php_value '%s'", kv->key);
		}
	}

	for (kv = wp->config->php_admin_values; kv; kv = kv->next) {
		if (fpm_php_apply_defines_ex(kv, ZEND_INI_SYSTEM) == -1) {
			zlog(ZLOG_ERROR, "Unable to set php_admin_value '%s'", kv->key);
		}
	}

	return 0;
}
/* }}} */

static int fpm_php_set_allowed_clients(struct fpm_worker_pool_s *wp) /* {{{ */
{
	if (wp->listen_address_domain == FPM_AF_INET) {
		fcgi_set_allowed_clients(wp->config->listen_allowed_clients);
	}
	return 0;
}
/* }}} */

#if 0 /* Comment out this non used function. It could be used later. */
static int fpm_php_set_fcgi_mgmt_vars(struct fpm_worker_pool_s *wp) /* {{{ */
{
	char max_workers[10 + 1]; /* 4294967295 */
	int len;

	len = sprintf(max_workers, "%u", (unsigned int) wp->config->pm_max_children);

	fcgi_set_mgmt_var("FCGI_MAX_CONNS", sizeof("FCGI_MAX_CONNS")-1, max_workers, len);
	fcgi_set_mgmt_var("FCGI_MAX_REQS",  sizeof("FCGI_MAX_REQS")-1,  max_workers, len);
	return 0;
}
/* }}} */
#endif

char *fpm_php_script_filename(void) /* {{{ */
{
	return SG(request_info).path_translated;
}
/* }}} */

char *fpm_php_request_uri(void) /* {{{ */
{
	return (char *) SG(request_info).request_uri;
}
/* }}} */

char *fpm_php_request_method(void) /* {{{ */
{
	return (char *) SG(request_info).request_method;
}
/* }}} */

char *fpm_php_query_string(void) /* {{{ */
{
	return SG(request_info).query_string;
}
/* }}} */

char *fpm_php_auth_user(void) /* {{{ */
{
	return SG(request_info).auth_user;
}
/* }}} */

size_t fpm_php_content_length(void) /* {{{ */
{
	return SG(request_info).content_length;
}
/* }}} */

static void fpm_php_cleanup(int which, void *arg) /* {{{ */
{
	php_module_shutdown();
	sapi_shutdown();
}
/* }}} */

void fpm_php_soft_quit() /* {{{ */
{
	fcgi_terminate();
}
/* }}} */

int fpm_php_init_main() /* {{{ */
{
	if (0 > fpm_cleanup_add(FPM_CLEANUP_PARENT, fpm_php_cleanup, 0)) {
		return -1;
	}
	return 0;
}
/* }}} */

int fpm_php_init_child(struct fpm_worker_pool_s *wp) /* {{{ */
{
	if (0 > fpm_php_apply_defines(wp) ||
		0 > fpm_php_set_allowed_clients(wp)) {
		return -1;
	}

	if (wp->limit_extensions) {
		limit_extensions = wp->limit_extensions;
	}
	return 0;
}
/* }}} */

int fpm_php_limit_extensions(char *path) /* {{{ */
{
	char **p;
	size_t path_len;

	if (!path || !limit_extensions) {
		return 0; /* allowed by default */
	}

	p = limit_extensions;
	path_len = strlen(path);
	while (p && *p) {
		size_t ext_len = strlen(*p);
		if (path_len > ext_len) {
			char *path_ext = path + path_len - ext_len;
			if (strcmp(*p, path_ext) == 0) {
				return 0; /* allow as the extension has been found */
			}
		}
		p++;
	}


	zlog(ZLOG_NOTICE, "Access to the script '%s' has been denied (see security.limit_extensions)", path);
	return 1; /* extension not found: not allowed  */
}
/* }}} */

char* fpm_php_get_string_from_table(zend_string *table, char *key) /* {{{ */
{
	zval *data, *tmp;
	zend_string *str;
	if (!table || !key) {
		return NULL;
	}

	/* inspired from ext/standard/info.c */

	zend_is_auto_global(table);

	/* find the table and ensure it's an array */
	data = zend_hash_find(&EG(symbol_table), table);
	if (!data || Z_TYPE_P(data) != IS_ARRAY) {
		return NULL;
	}

	ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(data), str, tmp) {
		if (str && !strncmp(ZSTR_VAL(str), key, ZSTR_LEN(str))) {
			return Z_STRVAL_P(tmp);
		}
	} ZEND_HASH_FOREACH_END();

	return NULL;
}
/* }}} */
