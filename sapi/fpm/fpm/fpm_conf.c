	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <inttypes.h>
#ifdef HAVE_GLOB
# include <glob.h>
#endif

#include <stdio.h>
#include <unistd.h>

#include "php.h"
#include "zend_ini_scanner.h"
#include "zend_globals.h"
#include "zend_stream.h"
#include "php_syslog.h"

#include "fpm.h"
#include "fpm_conf.h"
#include "fpm_stdio.h"
#include "fpm_worker_pool.h"
#include "fpm_cleanup.h"
#include "fpm_php.h"
#include "fpm_sockets.h"
#include "fpm_shm.h"
#include "fpm_status.h"
#include "fpm_log.h"
#include "fpm_events.h"
#include "zlog.h"
#ifdef HAVE_SYSTEMD
#include "fpm_systemd.h"
#endif


#define STR2STR(a) (a ? a : "undefined")
#define BOOL2STR(a) (a ? "yes" : "no")
#define GO(field) offsetof(struct fpm_global_config_s, field)
#define WPO(field) offsetof(struct fpm_worker_pool_config_s, field)

static int fpm_conf_load_ini_file(char *filename);
static char *fpm_conf_set_integer(zval *value, void **config, intptr_t offset);
#if 0 /* not used for now */
static char *fpm_conf_set_long(zval *value, void **config, intptr_t offset);
#endif
static char *fpm_conf_set_time(zval *value, void **config, intptr_t offset);
static char *fpm_conf_set_boolean(zval *value, void **config, intptr_t offset);
static char *fpm_conf_set_string(zval *value, void **config, intptr_t offset);
static char *fpm_conf_set_log_level(zval *value, void **config, intptr_t offset);
static char *fpm_conf_set_rlimit_core(zval *value, void **config, intptr_t offset);
static char *fpm_conf_set_pm(zval *value, void **config, intptr_t offset);
#ifdef HAVE_SYSLOG_H
static char *fpm_conf_set_syslog_facility(zval *value, void **config, intptr_t offset);
#endif

struct fpm_global_config_s fpm_global_config = {
	.daemonize = 1,
#ifdef HAVE_SYSLOG_H
	.syslog_facility = -1,
#endif
	.process_max = 0,
	.process_priority = 64, /* 64 means unset */
#ifdef HAVE_SYSTEMD
	.systemd_watchdog = 0,
	.systemd_interval = -1, /* -1 means not set */
#endif
	.log_buffering = ZLOG_DEFAULT_BUFFERING,
	.log_limit = ZLOG_DEFAULT_LIMIT
};
static struct fpm_worker_pool_s *current_wp = NULL;
static int ini_recursion = 0;
static char *ini_filename = NULL;
static int ini_lineno = 0;
static char *ini_include = NULL;

/*
 * Please keep the same order as in fpm_conf.h and in php-fpm.conf.in
 */
static struct ini_value_parser_s ini_fpm_global_options[] = {
	{ "pid",                         &fpm_conf_set_string,          GO(pid_file) },
	{ "error_log",                   &fpm_conf_set_string,          GO(error_log) },
#ifdef HAVE_SYSLOG_H
	{ "syslog.ident",                &fpm_conf_set_string,          GO(syslog_ident) },
	{ "syslog.facility",             &fpm_conf_set_syslog_facility, GO(syslog_facility) },
#endif
	{ "log_buffering",               &fpm_conf_set_boolean,         GO(log_buffering) },
	{ "log_level",                   &fpm_conf_set_log_level,       GO(log_level) },
	{ "log_limit",                   &fpm_conf_set_integer,         GO(log_limit) },
	{ "emergency_restart_threshold", &fpm_conf_set_integer,         GO(emergency_restart_threshold) },
	{ "emergency_restart_interval",  &fpm_conf_set_time,            GO(emergency_restart_interval) },
	{ "process_control_timeout",     &fpm_conf_set_time,            GO(process_control_timeout) },
	{ "process.max",                 &fpm_conf_set_integer,         GO(process_max) },
	{ "process.priority",            &fpm_conf_set_integer,         GO(process_priority) },
	{ "daemonize",                   &fpm_conf_set_boolean,         GO(daemonize) },
	{ "rlimit_files",                &fpm_conf_set_integer,         GO(rlimit_files) },
	{ "rlimit_core",                 &fpm_conf_set_rlimit_core,     GO(rlimit_core) },
	{ "events.mechanism",            &fpm_conf_set_string,          GO(events_mechanism) },
#ifdef HAVE_SYSTEMD
	{ "systemd_interval",            &fpm_conf_set_time,            GO(systemd_interval) },
#endif
	{ 0, 0, 0 }
};

/*
 * Please keep the same order as in fpm_conf.h and in php-fpm.conf.in
 */
static struct ini_value_parser_s ini_fpm_pool_options[] = {
	{ "prefix",                    &fpm_conf_set_string,      WPO(prefix) },
	{ "user",                      &fpm_conf_set_string,      WPO(user) },
	{ "group",                     &fpm_conf_set_string,      WPO(group) },
	{ "listen",                    &fpm_conf_set_string,      WPO(listen_address) },
	{ "listen.backlog",            &fpm_conf_set_integer,     WPO(listen_backlog) },
#ifdef HAVE_FPM_ACL
	{ "listen.acl_users",          &fpm_conf_set_string,      WPO(listen_acl_users) },
	{ "listen.acl_groups",         &fpm_conf_set_string,      WPO(listen_acl_groups) },
#endif
	{ "listen.owner",              &fpm_conf_set_string,      WPO(listen_owner) },
	{ "listen.group",              &fpm_conf_set_string,      WPO(listen_group) },
	{ "listen.mode",               &fpm_conf_set_string,      WPO(listen_mode) },
	{ "listen.allowed_clients",    &fpm_conf_set_string,      WPO(listen_allowed_clients) },
#ifdef SO_SETFIB
	{ "listen.setfib",             &fpm_conf_set_integer,     WPO(listen_setfib) },
#endif
	{ "process.priority",          &fpm_conf_set_integer,     WPO(process_priority) },
	{ "process.dumpable",          &fpm_conf_set_boolean,     WPO(process_dumpable) },
	{ "pm",                        &fpm_conf_set_pm,          WPO(pm) },
	{ "pm.max_children",           &fpm_conf_set_integer,     WPO(pm_max_children) },
	{ "pm.start_servers",          &fpm_conf_set_integer,     WPO(pm_start_servers) },
	{ "pm.min_spare_servers",      &fpm_conf_set_integer,     WPO(pm_min_spare_servers) },
	{ "pm.max_spare_servers",      &fpm_conf_set_integer,     WPO(pm_max_spare_servers) },
	{ "pm.max_spawn_rate",         &fpm_conf_set_integer,     WPO(pm_max_spawn_rate) },
	{ "pm.process_idle_timeout",   &fpm_conf_set_time,        WPO(pm_process_idle_timeout) },
	{ "pm.max_requests",           &fpm_conf_set_integer,     WPO(pm_max_requests) },
	{ "pm.status_path",            &fpm_conf_set_string,      WPO(pm_status_path) },
	{ "pm.status_listen",          &fpm_conf_set_string,      WPO(pm_status_listen) },
	{ "ping.path",                 &fpm_conf_set_string,      WPO(ping_path) },
	{ "ping.response",             &fpm_conf_set_string,      WPO(ping_response) },
	{ "access.log",                &fpm_conf_set_string,      WPO(access_log) },
	{ "access.format",             &fpm_conf_set_string,      WPO(access_format) },
	{ "slowlog",                   &fpm_conf_set_string,      WPO(slowlog) },
	{ "request_slowlog_timeout",   &fpm_conf_set_time,        WPO(request_slowlog_timeout) },
	{ "request_slowlog_trace_depth", &fpm_conf_set_integer,     WPO(request_slowlog_trace_depth) },
	{ "request_terminate_timeout", &fpm_conf_set_time,        WPO(request_terminate_timeout) },
	{ "request_terminate_timeout_track_finished", &fpm_conf_set_boolean, WPO(request_terminate_timeout_track_finished) },
	{ "rlimit_files",              &fpm_conf_set_integer,     WPO(rlimit_files) },
	{ "rlimit_core",               &fpm_conf_set_rlimit_core, WPO(rlimit_core) },
	{ "chroot",                    &fpm_conf_set_string,      WPO(chroot) },
	{ "chdir",                     &fpm_conf_set_string,      WPO(chdir) },
	{ "catch_workers_output",      &fpm_conf_set_boolean,     WPO(catch_workers_output) },
	{ "decorate_workers_output",   &fpm_conf_set_boolean,     WPO(decorate_workers_output) },
	{ "clear_env",                 &fpm_conf_set_boolean,     WPO(clear_env) },
	{ "security.limit_extensions", &fpm_conf_set_string,      WPO(security_limit_extensions) },
#ifdef HAVE_APPARMOR
	{ "apparmor_hat",              &fpm_conf_set_string,      WPO(apparmor_hat) },
#endif
	{ 0, 0, 0 }
};

static int fpm_conf_is_dir(char *path) /* {{{ */
{
	struct stat sb;

	if (stat(path, &sb) != 0) {
		return 0;
	}

	return (sb.st_mode & S_IFMT) == S_IFDIR;
}
/* }}} */

/*
 * Expands the '$pool' token in a dynamically allocated string
 */
static int fpm_conf_expand_pool_name(char **value) {
	char *token;

	if (!value || !*value) {
		return 0;
	}

	while (*value && (token = strstr(*value, "$pool"))) {
		char *buf;
		char *p2 = token + strlen("$pool");

		/* If we are not in a pool, we cannot expand this name now */
		if (!current_wp || !current_wp->config  || !current_wp->config->name) {
			return -1;
		}

		/* "aaa$poolbbb" becomes "aaa\0oolbbb" */
		token[0] = '\0';

		/* Build a brand new string with the expanded token */
		spprintf(&buf, 0, "%s%s%s", *value, current_wp->config->name, p2);

		/* Free the previous value and save the new one */
		free(*value);
		*value = strdup(buf);
		efree(buf);
	}

	return 0;
}

static char *fpm_conf_set_boolean(zval *value, void **config, intptr_t offset) /* {{{ */
{
	zend_string *val = Z_STR_P(value);
	bool value_y = zend_string_equals_literal(val, "1");
	bool value_n = ZSTR_LEN(val) == 0; /* Empty string is the only valid false value */

	if (!value_y && !value_n) {
		return "invalid boolean value";
	}

	* (int *) ((char *) *config + offset) = value_y ? 1 : 0;
	return NULL;
}
/* }}} */

static char *fpm_conf_set_string(zval *value, void **config, intptr_t offset) /* {{{ */
{
	char **config_val = (char **) ((char *) *config + offset);

	if (!config_val) {
		return "internal error: NULL value";
	}

	/* Check if there is a previous value to deallocate */
	if (*config_val) {
		free(*config_val);
	}

	*config_val = strdup(Z_STRVAL_P(value));
	if (!*config_val) {
		return "fpm_conf_set_string(): strdup() failed";
	}
	if (fpm_conf_expand_pool_name(config_val) == -1) {
		return "Can't use '$pool' when the pool is not defined";
	}

	return NULL;
}
/* }}} */

static char *fpm_conf_set_integer(zval *value, void **config, intptr_t offset) /* {{{ */
{
	char *val = Z_STRVAL_P(value);
	char *p;

	/* we don't use strtol because we don't want to allow negative values */
	for (p = val; *p; p++) {
		if (p == val && *p == '-') continue;
		if (*p < '0' || *p > '9') {
			return "is not a valid number (greater or equal than zero)";
		}
	}
	* (int *) ((char *) *config + offset) = atoi(val);
	return NULL;
}
/* }}} */

#if 0 /* not used for now */
static char *fpm_conf_set_long(zval *value, void **config, intptr_t offset) /* {{{ */
{
	char *val = Z_STRVAL_P(value);
	char *p;

	for (p = val; *p; p++) {
		if ( p == val && *p == '-' ) continue;
		if (*p < '0' || *p > '9') {
			return "is not a valid number (greater or equal than zero)";
		}
	}
	* (long int *) ((char *) *config + offset) = atol(val);
	return NULL;
}
/* }}} */
#endif

static char *fpm_conf_set_time(zval *value, void **config, intptr_t offset) /* {{{ */
{
	char *val = Z_STRVAL_P(value);
	int len = strlen(val);
	char suffix;
	int seconds;
	if (!len) {
		return "invalid time value";
	}

	suffix = val[len-1];
	switch (suffix) {
		case 'm' :
			val[len-1] = '\0';
			seconds = 60 * atoi(val);
			break;
		case 'h' :
			val[len-1] = '\0';
			seconds = 60 * 60 * atoi(val);
			break;
		case 'd' :
			val[len-1] = '\0';
			seconds = 24 * 60 * 60 * atoi(val);
			break;
		case 's' : /* s is the default suffix */
			val[len-1] = '\0';
			suffix = '0';
			ZEND_FALLTHROUGH;
		default :
			if (suffix < '0' || suffix > '9') {
				return "unknown suffix used in time value";
			}
			seconds = atoi(val);
			break;
	}

	* (int *) ((char *) *config + offset) = seconds;
	return NULL;
}
/* }}} */

static char *fpm_conf_set_log_level(zval *value, void **config, intptr_t offset) /* {{{ */
{
	zend_string *val = Z_STR_P(value);
	int log_level;

	if (zend_string_equals_literal_ci(val, "debug")) {
		log_level = ZLOG_DEBUG;
	} else if (zend_string_equals_literal_ci(val, "notice")) {
		log_level = ZLOG_NOTICE;
	} else if (zend_string_equals_literal_ci(val, "warning") || zend_string_equals_literal_ci(val, "warn")) {
		log_level = ZLOG_WARNING;
	} else if (zend_string_equals_literal_ci(val, "error")) {
		log_level = ZLOG_ERROR;
	} else if (zend_string_equals_literal_ci(val, "alert")) {
		log_level = ZLOG_ALERT;
	} else {
		return "invalid value for 'log_level'";
	}

	* (int *) ((char *) *config + offset) = log_level;
	return NULL;
}
/* }}} */

#ifdef HAVE_SYSLOG_H
static char *fpm_conf_set_syslog_facility(zval *value, void **config, intptr_t offset) /* {{{ */
{
	zend_string *val = Z_STR_P(value);
	int *conf = (int *) ((char *) *config + offset);

#ifdef LOG_AUTH
	if (zend_string_equals_literal_ci(val, "AUTH")) {
		*conf = LOG_AUTH;
		return NULL;
	}
#endif

#ifdef LOG_AUTHPRIV
	if (zend_string_equals_literal_ci(val, "AUTHPRIV")) {
		*conf = LOG_AUTHPRIV;
		return NULL;
	}
#endif

#ifdef LOG_CRON
	if (zend_string_equals_literal_ci(val, "CRON")) {
		*conf = LOG_CRON;
		return NULL;
	}
#endif

#ifdef LOG_DAEMON
	if (zend_string_equals_literal_ci(val, "DAEMON")) {
		*conf = LOG_DAEMON;
		return NULL;
	}
#endif

#ifdef LOG_FTP
	if (zend_string_equals_literal_ci(val, "FTP")) {
		*conf = LOG_FTP;
		return NULL;
	}
#endif

#ifdef LOG_KERN
	if (zend_string_equals_literal_ci(val, "KERN")) {
		*conf = LOG_KERN;
		return NULL;
	}
#endif

#ifdef LOG_LPR
	if (zend_string_equals_literal_ci(val, "LPR")) {
		*conf = LOG_LPR;
		return NULL;
	}
#endif

#ifdef LOG_MAIL
	if (zend_string_equals_literal_ci(val, "MAIL")) {
		*conf = LOG_MAIL;
		return NULL;
	}
#endif

#ifdef LOG_NEWS
	if (zend_string_equals_literal_ci(val, "NEWS")) {
		*conf = LOG_NEWS;
		return NULL;
	}
#endif

#ifdef LOG_SYSLOG
	if (zend_string_equals_literal_ci(val, "SYSLOG")) {
		*conf = LOG_SYSLOG;
		return NULL;
	}
#endif

#ifdef LOG_USER
	if (zend_string_equals_literal_ci(val, "USER")) {
		*conf = LOG_USER;
		return NULL;
	}
#endif

#ifdef LOG_UUCP
	if (zend_string_equals_literal_ci(val, "UUCP")) {
		*conf = LOG_UUCP;
		return NULL;
	}
#endif

#ifdef LOG_LOCAL0
	if (zend_string_equals_literal_ci(val, "LOCAL0")) {
		*conf = LOG_LOCAL0;
		return NULL;
	}
#endif

#ifdef LOG_LOCAL1
	if (zend_string_equals_literal_ci(val, "LOCAL1")) {
		*conf = LOG_LOCAL1;
		return NULL;
	}
#endif

#ifdef LOG_LOCAL2
	if (zend_string_equals_literal_ci(val, "LOCAL2")) {
		*conf = LOG_LOCAL2;
		return NULL;
	}
#endif

#ifdef LOG_LOCAL3
	if (zend_string_equals_literal_ci(val, "LOCAL3")) {
		*conf = LOG_LOCAL3;
		return NULL;
	}
#endif

#ifdef LOG_LOCAL4
	if (zend_string_equals_literal_ci(val, "LOCAL4")) {
		*conf = LOG_LOCAL4;
		return NULL;
	}
#endif

#ifdef LOG_LOCAL5
	if (zend_string_equals_literal_ci(val, "LOCAL5")) {
		*conf = LOG_LOCAL5;
		return NULL;
	}
#endif

#ifdef LOG_LOCAL6
	if (zend_string_equals_literal_ci(val, "LOCAL6")) {
		*conf = LOG_LOCAL6;
		return NULL;
	}
#endif

#ifdef LOG_LOCAL7
	if (zend_string_equals_literal_ci(val, "LOCAL7")) {
		*conf = LOG_LOCAL7;
		return NULL;
	}
#endif

	return "invalid value";
}
/* }}} */
#endif

static char *fpm_conf_set_rlimit_core(zval *value, void **config, intptr_t offset) /* {{{ */
{
	zend_string *val = Z_STR_P(value);
	int *ptr = (int *) ((char *) *config + offset);

	if (zend_string_equals_literal_ci(val, "unlimited")) {
		*ptr = -1;
	} else {
		int int_value;
		void *subconf = &int_value;
		char *error;

		error = fpm_conf_set_integer(value, &subconf, 0);

		if (error) {
			return error;
		}

		if (int_value < 0) {
			return "must be greater than zero or 'unlimited'";
		}

		*ptr = int_value;
	}

	return NULL;
}
/* }}} */

static char *fpm_conf_set_pm(zval *value, void **config, intptr_t offset) /* {{{ */
{
	zend_string *val = Z_STR_P(value);
	struct fpm_worker_pool_config_s  *c = *config;
	if (zend_string_equals_literal_ci(val, "static")) {
		c->pm = PM_STYLE_STATIC;
	} else if (zend_string_equals_literal_ci(val, "dynamic")) {
		c->pm = PM_STYLE_DYNAMIC;
	} else if (zend_string_equals_literal_ci(val, "ondemand")) {
		c->pm = PM_STYLE_ONDEMAND;
	} else {
		return "invalid process manager (static, dynamic or ondemand)";
	}
	return NULL;
}
/* }}} */

static char *fpm_conf_set_array(zval *key, zval *value, void **config, int convert_to_bool) /* {{{ */
{
	struct key_value_s *kv;
	struct key_value_s ***parent = (struct key_value_s ***) config;
	int b;
	void *subconf = &b;

	kv = malloc(sizeof(*kv));

	if (!kv) {
		return "malloc() failed";
	}

	memset(kv, 0, sizeof(*kv));
	if (key) {
		kv->key = strdup(Z_STRVAL_P(key));

		if (!kv->key) {
			free(kv);
			return "fpm_conf_set_array: strdup(key) failed";
		}
	}

	if (convert_to_bool) {
		char *err = fpm_conf_set_boolean(value, &subconf, 0);
		if (err) {
			free(kv->key);
			free(kv);
			return err;
		}
		kv->value = strdup(b ? "1" : "0");
	} else {
		kv->value = strdup(Z_STRVAL_P(value));
		if (fpm_conf_expand_pool_name(&kv->value) == -1) {
			free(kv->key);
			free(kv);
			return "Can't use '$pool' when the pool is not defined";
		}
	}

	if (!kv->value) {
		free(kv->key);
		free(kv);
		return "fpm_conf_set_array: strdup(value) failed";
	}

	kv->next = **parent;
	**parent = kv;
	return NULL;
}
/* }}} */

static void *fpm_worker_pool_config_alloc(void)
{
	struct fpm_worker_pool_s *wp;

	wp = fpm_worker_pool_alloc();

	if (!wp) {
		return 0;
	}

	wp->config = malloc(sizeof(struct fpm_worker_pool_config_s));

	if (!wp->config) {
		fpm_worker_pool_free(wp);
		return 0;
	}

	memset(wp->config, 0, sizeof(struct fpm_worker_pool_config_s));
	wp->config->listen_backlog = FPM_BACKLOG_DEFAULT;
	wp->config->pm_max_spawn_rate = 32; /* 32 by default */
	wp->config->pm_process_idle_timeout = 10; /* 10s by default */
	wp->config->process_priority = 64; /* 64 means unset */
	wp->config->process_dumpable = 0;
	wp->config->clear_env = 1;
	wp->config->decorate_workers_output = 1;
#ifdef SO_SETFIB
	wp->config->listen_setfib = -1;
#endif

	if (!fpm_worker_all_pools) {
		fpm_worker_all_pools = wp;
	} else {
		struct fpm_worker_pool_s *tmp = fpm_worker_all_pools;
		while (tmp) {
			if (!tmp->next) {
				tmp->next = wp;
				break;
			}
			tmp = tmp->next;
		}
	}

	current_wp = wp;
	return wp->config;
}

int fpm_worker_pool_config_free(struct fpm_worker_pool_config_s *wpc) /* {{{ */
{
	struct key_value_s *kv, *kv_next;

	free(wpc->name);
	free(wpc->prefix);
	free(wpc->user);
	free(wpc->group);
	free(wpc->listen_address);
	free(wpc->listen_owner);
	free(wpc->listen_group);
	free(wpc->listen_mode);
	free(wpc->listen_allowed_clients);
	free(wpc->pm_status_path);
	free(wpc->ping_path);
	free(wpc->ping_response);
	free(wpc->access_log);
	free(wpc->access_format);
	free(wpc->slowlog);
	free(wpc->chroot);
	free(wpc->chdir);
	free(wpc->security_limit_extensions);
#ifdef HAVE_APPARMOR
	free(wpc->apparmor_hat);
#endif

	for (kv = wpc->access_suppress_paths; kv; kv = kv_next) {
		kv_next = kv->next;
		free(kv->value);
		free(kv);
	}
	for (kv = wpc->php_values; kv; kv = kv_next) {
		kv_next = kv->next;
		free(kv->key);
		free(kv->value);
		free(kv);
	}
	for (kv = wpc->php_admin_values; kv; kv = kv_next) {
		kv_next = kv->next;
		free(kv->key);
		free(kv->value);
		free(kv);
	}
	for (kv = wpc->env; kv; kv = kv_next) {
		kv_next = kv->next;
		free(kv->key);
		free(kv->value);
		free(kv);
	}

	return 0;
}
/* }}} */

#define FPM_WPC_STR_CP_EX(_cfg, _scfg, _sf, _df) \
	do { \
		if (_scfg->_df && !(_cfg->_sf = strdup(_scfg->_df))) { \
			return -1; \
		} \
	} while (0)
#define FPM_WPC_STR_CP(_cfg, _scfg, _field) FPM_WPC_STR_CP_EX(_cfg, _scfg, _field, _field)

static int fpm_worker_pool_shared_status_alloc(struct fpm_worker_pool_s *shared_wp) { /* {{{ */
	struct fpm_worker_pool_config_s *config, *shared_config;
	config = fpm_worker_pool_config_alloc();
	if (!config) {
		return -1;
	}
	shared_config = shared_wp->config;

	config->name = malloc(strlen(shared_config->name) + sizeof("_status"));
	if (!config->name) {
		return -1;
	}
	strcpy(config->name, shared_config->name);
	strcpy(config->name + strlen(shared_config->name), "_status");

	if (!shared_config->pm_status_path) {
		shared_config->pm_status_path = strdup("/");
	}

	FPM_WPC_STR_CP_EX(config, shared_config, listen_address, pm_status_listen);
#ifdef HAVE_FPM_ACL
	FPM_WPC_STR_CP(config, shared_config, listen_acl_groups);
	FPM_WPC_STR_CP(config, shared_config, listen_acl_users);
#endif
	FPM_WPC_STR_CP(config, shared_config, listen_allowed_clients);
	FPM_WPC_STR_CP(config, shared_config, listen_group);
	FPM_WPC_STR_CP(config, shared_config, listen_owner);
	FPM_WPC_STR_CP(config, shared_config, listen_mode);
	FPM_WPC_STR_CP(config, shared_config, user);
	FPM_WPC_STR_CP(config, shared_config, group);
	FPM_WPC_STR_CP(config, shared_config, pm_status_path);

	config->pm = PM_STYLE_ONDEMAND;
	config->pm_max_children = 2;

	current_wp->shared = shared_wp;

	return 0;
}
/* }}} */

static int fpm_evaluate_full_path(char **path, struct fpm_worker_pool_s *wp, char *default_prefix, int expand) /* {{{ */
{
	char *prefix = NULL;
	char *full_path;

	if (!path || !*path || **path == '/') {
		return 0;
	}

	if (wp && wp->config) {
		prefix = wp->config->prefix;
	}

	/* if the wp prefix is not set */
	if (prefix == NULL) {
		prefix = fpm_globals.prefix;
	}

	/* if the global prefix is not set */
	if (prefix == NULL) {
		prefix = default_prefix ? default_prefix : PHP_PREFIX;
	}

	if (expand) {
		char *tmp;
		tmp = strstr(*path, "$prefix");
		if (tmp != NULL) {

			if (tmp != *path) {
				zlog(ZLOG_ERROR, "'$prefix' must be use at the beginning of the value");
				return -1;
			}

			if (strlen(*path) > strlen("$prefix")) {
				tmp = strdup((*path) + strlen("$prefix"));
				free(*path);
				*path = tmp;
			} else {
				free(*path);
				*path = NULL;
			}
		}
	}

	if (*path) {
		spprintf(&full_path, 0, "%s/%s", prefix, *path);
		free(*path);
		*path = strdup(full_path);
		efree(full_path);
	} else {
		*path = strdup(prefix);
	}

	if (**path != '/' && wp != NULL && wp->config) {
		return fpm_evaluate_full_path(path, NULL, default_prefix, expand);
	}
	return 0;
}
/* }}} */

static int fpm_conf_process_all_pools(void)
{
	struct fpm_worker_pool_s *wp, *wp2;

	if (!fpm_worker_all_pools) {
		zlog(ZLOG_ERROR, "No pool defined. at least one pool section must be specified in config file");
		return -1;
	}

	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {

		/* prefix */
		if (wp->config->prefix && *wp->config->prefix) {
			fpm_evaluate_full_path(&wp->config->prefix, NULL, NULL, 0);

			if (!fpm_conf_is_dir(wp->config->prefix)) {
				zlog(ZLOG_ERROR, "[pool %s] the prefix '%s' does not exist or is not a directory", wp->config->name, wp->config->prefix);
				return -1;
			}
		}

		/* alert if user is not set; only if we are root and fpm is not running with --allow-to-run-as-root */
		if (!wp->config->user && !geteuid() && !fpm_globals.run_as_root) {
			zlog(ZLOG_ALERT, "[pool %s] user has not been defined", wp->config->name);
			return -1;
		}

		/* listen */
		if (wp->config->listen_address && *wp->config->listen_address) {
			wp->listen_address_domain = fpm_sockets_domain_from_address(wp->config->listen_address);

			if (wp->listen_address_domain == FPM_AF_UNIX && *wp->config->listen_address != '/') {
				fpm_evaluate_full_path(&wp->config->listen_address, wp, NULL, 0);
			}
		} else {
			zlog(ZLOG_ALERT, "[pool %s] no listen address have been defined!", wp->config->name);
			return -1;
		}

		if (wp->config->process_priority != 64 && (wp->config->process_priority < -19 || wp->config->process_priority > 20)) {
			zlog(ZLOG_ERROR, "[pool %s] process.priority must be included into [-19,20]", wp->config->name);
			return -1;
		}

		/* pm */
		if (wp->config->pm != PM_STYLE_STATIC && wp->config->pm != PM_STYLE_DYNAMIC && wp->config->pm != PM_STYLE_ONDEMAND) {
			zlog(ZLOG_ALERT, "[pool %s] the process manager is missing (static, dynamic or ondemand)", wp->config->name);
			return -1;
		}

		/* pm.max_children */
		if (wp->config->pm_max_children < 1) {
			zlog(ZLOG_ALERT, "[pool %s] pm.max_children must be a positive value", wp->config->name);
			return -1;
		}

		/* pm.start_servers, pm.min_spare_servers, pm.max_spare_servers, pm.max_spawn_rate */
		if (wp->config->pm == PM_STYLE_DYNAMIC) {
			struct fpm_worker_pool_config_s *config = wp->config;

			if (config->pm_min_spare_servers <= 0) {
				zlog(ZLOG_ALERT, "[pool %s] pm.min_spare_servers(%d) must be a positive value", wp->config->name, config->pm_min_spare_servers);
				return -1;
			}

			if (config->pm_max_spare_servers <= 0) {
				zlog(ZLOG_ALERT, "[pool %s] pm.max_spare_servers(%d) must be a positive value", wp->config->name, config->pm_max_spare_servers);
				return -1;
			}

			if (config->pm_min_spare_servers > config->pm_max_children ||
					config->pm_max_spare_servers > config->pm_max_children) {
				zlog(ZLOG_ALERT, "[pool %s] pm.min_spare_servers(%d) and pm.max_spare_servers(%d) cannot be greater than pm.max_children(%d)", wp->config->name, config->pm_min_spare_servers, config->pm_max_spare_servers, config->pm_max_children);
				return -1;
			}

			if (config->pm_max_spare_servers < config->pm_min_spare_servers) {
				zlog(ZLOG_ALERT, "[pool %s] pm.max_spare_servers(%d) must not be less than pm.min_spare_servers(%d)", wp->config->name, config->pm_max_spare_servers, config->pm_min_spare_servers);
				return -1;
			}

			if (config->pm_start_servers <= 0) {
				config->pm_start_servers = config->pm_min_spare_servers + ((config->pm_max_spare_servers - config->pm_min_spare_servers) / 2);
				zlog(ZLOG_NOTICE, "[pool %s] pm.start_servers is not set. It's been set to %d.", wp->config->name, config->pm_start_servers);

			} else if (config->pm_start_servers < config->pm_min_spare_servers || config->pm_start_servers > config->pm_max_spare_servers) {
				zlog(ZLOG_ALERT, "[pool %s] pm.start_servers(%d) must not be less than pm.min_spare_servers(%d) and not greater than pm.max_spare_servers(%d)", wp->config->name, config->pm_start_servers, config->pm_min_spare_servers, config->pm_max_spare_servers);
				return -1;
			}

			if (config->pm_max_spawn_rate < 1) {
				zlog(ZLOG_ALERT, "[pool %s] pm.max_spawn_rate must be a positive value", wp->config->name);
				return -1;
			}
		} else if (wp->config->pm == PM_STYLE_ONDEMAND) {
			struct fpm_worker_pool_config_s *config = wp->config;

			if (!fpm_event_support_edge_trigger()) {
				zlog(ZLOG_ALERT, "[pool %s] ondemand process manager can ONLY be used when events.mechanism is either epoll (Linux) or kqueue (*BSD).", wp->config->name);
				return -1;
			}

			if (config->pm_process_idle_timeout < 1) {
				zlog(ZLOG_ALERT, "[pool %s] pm.process_idle_timeout(%ds) must be greater than 0s", wp->config->name, config->pm_process_idle_timeout);
				return -1;
			}

			if (config->listen_backlog < FPM_BACKLOG_DEFAULT) {
				zlog(ZLOG_WARNING, "[pool %s] listen.backlog(%d) was too low for the ondemand process manager. I updated it for you to %d.", wp->config->name, config->listen_backlog, FPM_BACKLOG_DEFAULT);
			}

			/* certainly useless but proper */
			config->pm_start_servers = 0;
			config->pm_min_spare_servers = 0;
			config->pm_max_spare_servers = 0;
		}

		/* status */
		if (wp->config->pm_status_listen && fpm_worker_pool_shared_status_alloc(wp)) {
			zlog(ZLOG_ERROR, "[pool %s] failed to initialize a status listener pool", wp->config->name);
		}

		if (wp->config->pm_status_path && *wp->config->pm_status_path) {
			size_t i;
			char *status = wp->config->pm_status_path;

			if (*status != '/') {
				zlog(ZLOG_ERROR, "[pool %s] the status path '%s' must start with a '/'", wp->config->name, status);
				return -1;
			}

			if (!wp->config->pm_status_listen && !wp->shared && strlen(status) < 2) {
				zlog(ZLOG_ERROR, "[pool %s] the status path '%s' is not long enough", wp->config->name, status);
				return -1;
			}

			for (i = 0; i < strlen(status); i++) {
				if (!isalnum(status[i]) && status[i] != '/' && status[i] != '-' && status[i] != '_' && status[i] != '.' && status[i] != '~') {
					zlog(ZLOG_ERROR, "[pool %s] the status path '%s' must contain only the following characters '[alphanum]/_-.~'", wp->config->name, status);
					return -1;
				}
			}
		}

		/* ping */
		if (wp->config->ping_path && *wp->config->ping_path) {
			char *ping = wp->config->ping_path;
			size_t i;

			if (*ping != '/') {
				zlog(ZLOG_ERROR, "[pool %s] the ping path '%s' must start with a '/'", wp->config->name, ping);
				return -1;
			}

			if (strlen(ping) < 2) {
				zlog(ZLOG_ERROR, "[pool %s] the ping path '%s' is not long enough", wp->config->name, ping);
				return -1;
			}

			for (i = 0; i < strlen(ping); i++) {
				if (!isalnum(ping[i]) && ping[i] != '/' && ping[i] != '-' && ping[i] != '_' && ping[i] != '.' && ping[i] != '~') {
					zlog(ZLOG_ERROR, "[pool %s] the ping path '%s' must contain only the following characters '[alphanum]/_-.~'", wp->config->name, ping);
					return -1;
				}
			}

			if (!wp->config->ping_response) {
				wp->config->ping_response = strdup("pong");
			} else {
				if (strlen(wp->config->ping_response) < 1) {
					zlog(ZLOG_ERROR, "[pool %s] the ping response page '%s' is not long enough", wp->config->name, wp->config->ping_response);
					return -1;
				}
			}
		} else {
			if (wp->config->ping_response) {
				free(wp->config->ping_response);
				wp->config->ping_response = NULL;
			}
		}

		/* access.log, access.format */
		if (wp->config->access_log && *wp->config->access_log) {
			fpm_evaluate_full_path(&wp->config->access_log, wp, NULL, 0);
			if (!wp->config->access_format) {
				wp->config->access_format = strdup("%R - %u %t \"%m %r\" %s");
			}
		}

		if (wp->config->request_terminate_timeout) {
			fpm_globals.heartbeat = fpm_globals.heartbeat ? MIN(fpm_globals.heartbeat, (wp->config->request_terminate_timeout * 1000) / 3) : (wp->config->request_terminate_timeout * 1000) / 3;
		}

		/* slowlog */
		if (wp->config->slowlog && *wp->config->slowlog) {
			fpm_evaluate_full_path(&wp->config->slowlog, wp, NULL, 0);
		}

		/* request_slowlog_timeout */
		if (wp->config->request_slowlog_timeout) {
#if HAVE_FPM_TRACE
			if (! (wp->config->slowlog && *wp->config->slowlog)) {
				zlog(ZLOG_ERROR, "[pool %s] 'slowlog' must be specified for use with 'request_slowlog_timeout'", wp->config->name);
				return -1;
			}
#else
			static int warned = 0;

			if (!warned) {
				zlog(ZLOG_WARNING, "[pool %s] 'request_slowlog_timeout' is not supported on your system",	wp->config->name);
				warned = 1;
			}

			wp->config->request_slowlog_timeout = 0;
#endif

			if (wp->config->slowlog && *wp->config->slowlog) {
				int fd;

				fd = open(wp->config->slowlog, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);

				if (0 > fd) {
					zlog(ZLOG_SYSERROR, "Unable to create or open slowlog(%s)", wp->config->slowlog);
					return -1;
				}
				close(fd);
			}

			fpm_globals.heartbeat = fpm_globals.heartbeat ? MIN(fpm_globals.heartbeat, (wp->config->request_slowlog_timeout * 1000) / 3) : (wp->config->request_slowlog_timeout * 1000) / 3;

			if (wp->config->request_terminate_timeout && wp->config->request_slowlog_timeout > wp->config->request_terminate_timeout) {
				zlog(ZLOG_ERROR, "[pool %s] 'request_slowlog_timeout' (%d) can't be greater than 'request_terminate_timeout' (%d)", wp->config->name, wp->config->request_slowlog_timeout, wp->config->request_terminate_timeout);
				return -1;
			}
		}

		/* request_slowlog_trace_depth */
		if (wp->config->request_slowlog_trace_depth) {
#if HAVE_FPM_TRACE
			if (! (wp->config->slowlog && *wp->config->slowlog)) {
				zlog(ZLOG_ERROR, "[pool %s] 'slowlog' must be specified for use with 'request_slowlog_trace_depth'", wp->config->name);
				return -1;
			}
#else
			static int warned = 0;

			if (!warned) {
				zlog(ZLOG_WARNING, "[pool %s] 'request_slowlog_trace_depth' is not supported on your system", wp->config->name);
				warned = 1;
			}
#endif

			if (wp->config->request_slowlog_trace_depth <= 0) {
				zlog(ZLOG_ERROR, "[pool %s] 'request_slowlog_trace_depth' (%d) must be a positive value", wp->config->name, wp->config->request_slowlog_trace_depth);
				return -1;
			}
		} else {
			wp->config->request_slowlog_trace_depth = 20;
		}

		/* chroot */
		if (wp->config->chroot && *wp->config->chroot) {

			fpm_evaluate_full_path(&wp->config->chroot, wp, NULL, 1);

			if (*wp->config->chroot != '/') {
				zlog(ZLOG_ERROR, "[pool %s] the chroot path '%s' must start with a '/'", wp->config->name, wp->config->chroot);
				return -1;
			}

			if (!fpm_conf_is_dir(wp->config->chroot)) {
				zlog(ZLOG_ERROR, "[pool %s] the chroot path '%s' does not exist or is not a directory", wp->config->name, wp->config->chroot);
				return -1;
			}
		}

		/* chdir */
		if (wp->config->chdir && *wp->config->chdir) {

			fpm_evaluate_full_path(&wp->config->chdir, wp, NULL, 0);

			if (*wp->config->chdir != '/') {
				zlog(ZLOG_ERROR, "[pool %s] the chdir path '%s' must start with a '/'", wp->config->name, wp->config->chdir);
				return -1;
			}

			if (wp->config->chroot) {
				char *buf;

				spprintf(&buf, 0, "%s/%s", wp->config->chroot, wp->config->chdir);

				if (!fpm_conf_is_dir(buf)) {
					zlog(ZLOG_ERROR, "[pool %s] the chdir path '%s' within the chroot path '%s' ('%s') does not exist or is not a directory", wp->config->name, wp->config->chdir, wp->config->chroot, buf);
					efree(buf);
					return -1;
				}

				efree(buf);
			} else {
				if (!fpm_conf_is_dir(wp->config->chdir)) {
					zlog(ZLOG_ERROR, "[pool %s] the chdir path '%s' does not exist or is not a directory", wp->config->name, wp->config->chdir);
					return -1;
				}
			}
		}

		/* security.limit_extensions */
		if (!wp->config->security_limit_extensions) {
			wp->config->security_limit_extensions = strdup(".php .phar");
		}

		if (*wp->config->security_limit_extensions) {
			int nb_ext;
			char *ext;
			char *security_limit_extensions;
			char *limit_extensions;


			/* strdup because strtok(3) alters the string it parses */
			security_limit_extensions = strdup(wp->config->security_limit_extensions);
			limit_extensions = security_limit_extensions;
			nb_ext = 0;

			/* find the number of extensions */
			while (strtok(limit_extensions, " \t")) {
				limit_extensions = NULL;
				nb_ext++;
			}
			free(security_limit_extensions);

			/* if something found */
			if (nb_ext > 0) {

				/* malloc the extension array */
				wp->limit_extensions = malloc(sizeof(char *) * (nb_ext + 1));
				if (!wp->limit_extensions) {
					zlog(ZLOG_ERROR, "[pool %s] unable to malloc extensions array", wp->config->name);
					return -1;
				}

				/* strdup because strtok(3) alters the string it parses */
				security_limit_extensions = strdup(wp->config->security_limit_extensions);
				limit_extensions = security_limit_extensions;
				nb_ext = 0;

				/* parse the string and save the extension in the array */
				while ((ext = strtok(limit_extensions, " \t"))) {
					limit_extensions = NULL;
					wp->limit_extensions[nb_ext++] = strdup(ext);
				}

				/* end the array with NULL in order to parse it */
				wp->limit_extensions[nb_ext] = NULL;
				free(security_limit_extensions);
			}
		}

		/* env[], php_value[], php_admin_values[] */
		if (!wp->config->chroot) {
			struct key_value_s *kv;
			char *options[] = FPM_PHP_INI_TO_EXPAND;
			char **p;

			for (kv = wp->config->php_values; kv; kv = kv->next) {
				for (p = options; *p; p++) {
					if (!strcasecmp(kv->key, *p)) {
						fpm_evaluate_full_path(&kv->value, wp, NULL, 0);
					}
				}
			}
			for (kv = wp->config->php_admin_values; kv; kv = kv->next) {
				if (!strcasecmp(kv->key, "error_log") && !strcasecmp(kv->value, "syslog")) {
					continue;
				}
				for (p = options; *p; p++) {
					if (!strcasecmp(kv->key, *p)) {
						fpm_evaluate_full_path(&kv->value, wp, NULL, 0);
					}
				}
			}
		}
	}

	/* ensure 2 pools do not use the same listening address */
	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		for (wp2 = fpm_worker_all_pools; wp2; wp2 = wp2->next) {
			if (wp == wp2) {
				continue;
			}

			if (wp->config->listen_address && *wp->config->listen_address && wp2->config->listen_address && *wp2->config->listen_address && !strcmp(wp->config->listen_address, wp2->config->listen_address)) {
				zlog(ZLOG_ERROR, "[pool %s] unable to set listen address as it's already used in another pool '%s'", wp2->config->name, wp->config->name);
				return -1;
			}
		}
	}
	return 0;
}

int fpm_conf_unlink_pid(void)
{
	if (fpm_global_config.pid_file) {
		if (0 > unlink(fpm_global_config.pid_file)) {
			zlog(ZLOG_SYSERROR, "Unable to remove the PID file (%s).", fpm_global_config.pid_file);
			return -1;
		}
	}
	return 0;
}

int fpm_conf_write_pid(void)
{
	int fd;

	if (fpm_global_config.pid_file) {
		char buf[64];
		int len;

		unlink(fpm_global_config.pid_file);
		fd = creat(fpm_global_config.pid_file, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

		if (fd < 0) {
			zlog(ZLOG_SYSERROR, "Unable to create the PID file (%s).", fpm_global_config.pid_file);
			return -1;
		}

		len = sprintf(buf, "%d", (int) fpm_globals.parent_pid);

		if (len != write(fd, buf, len)) {
			zlog(ZLOG_SYSERROR, "Unable to write to the PID file.");
			close(fd);
			return -1;
		}
		close(fd);
	}
	return 0;
}

static int fpm_conf_post_process(int force_daemon) /* {{{ */
{
	struct fpm_worker_pool_s *wp;

	if (fpm_global_config.pid_file) {
		fpm_evaluate_full_path(&fpm_global_config.pid_file, NULL, PHP_LOCALSTATEDIR, 0);
	}

	if (force_daemon >= 0) {
		/* forced from command line options */
		fpm_global_config.daemonize = force_daemon;
	}

	fpm_globals.log_level = fpm_global_config.log_level;
	zlog_set_level(fpm_globals.log_level);
	if (fpm_global_config.log_limit < ZLOG_MIN_LIMIT) {
		zlog(ZLOG_ERROR, "log_limit must be greater than %d", ZLOG_MIN_LIMIT);
		return -1;
	}
	zlog_set_limit(fpm_global_config.log_limit);
	zlog_set_buffering(fpm_global_config.log_buffering);

	if (fpm_global_config.process_max < 0) {
		zlog(ZLOG_ERROR, "process_max can't be negative");
		return -1;
	}

	if (fpm_global_config.process_priority != 64 && (fpm_global_config.process_priority < -19 || fpm_global_config.process_priority > 20)) {
		zlog(ZLOG_ERROR, "process.priority must be included into [-19,20]");
		return -1;
	}

	if (!fpm_global_config.error_log) {
		fpm_global_config.error_log = strdup("log/php-fpm.log");
	}

#ifdef HAVE_SYSTEMD
	if (0 > fpm_systemd_conf()) {
		return -1;
	}
#endif

#ifdef HAVE_SYSLOG_H
	if (!fpm_global_config.syslog_ident) {
		fpm_global_config.syslog_ident = strdup("php-fpm");
	}

	if (fpm_global_config.syslog_facility < 0) {
		fpm_global_config.syslog_facility = LOG_DAEMON;
	}

	if (strcasecmp(fpm_global_config.error_log, "syslog") != 0)
#endif
	{
		fpm_evaluate_full_path(&fpm_global_config.error_log, NULL, PHP_LOCALSTATEDIR, 0);
	}

	if (0 > fpm_stdio_save_original_stderr()) {
		return -1;
	}

	if (0 > fpm_stdio_open_error_log(0)) {
		return -1;
	}

	if (0 > fpm_event_pre_init(fpm_global_config.events_mechanism)) {
		return -1;
	}

	if (0 > fpm_conf_process_all_pools()) {
		return -1;
	}

	if (0 > fpm_log_open(0)) {
		return -1;
	}

	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		if (!wp->config->access_log || !*wp->config->access_log) {
			continue;
		}
		if (0 > fpm_log_write(wp->config->access_format)) {
			zlog(ZLOG_ERROR, "[pool %s] wrong format for access.format '%s'", wp->config->name, wp->config->access_format);
			return -1;
		}
	}

	return 0;
}
/* }}} */

static void fpm_conf_cleanup(int which, void *arg) /* {{{ */
{
	free(fpm_global_config.pid_file);
	free(fpm_global_config.error_log);
	free(fpm_global_config.events_mechanism);
	fpm_global_config.pid_file = 0;
	fpm_global_config.error_log = 0;
	fpm_global_config.log_limit = ZLOG_DEFAULT_LIMIT;
#ifdef HAVE_SYSLOG_H
	free(fpm_global_config.syslog_ident);
	fpm_global_config.syslog_ident = 0;
#endif
	free(fpm_globals.config);
}
/* }}} */

static void fpm_conf_ini_parser_include(char *inc, void *arg) /* {{{ */
{
	char *filename;
	int *error = (int *)arg;
#ifdef HAVE_GLOB
	glob_t g;
#endif
	size_t i;

	if (!inc || !arg) return;
	if (*error) return; /* We got already an error. Switch to the end. */
	spprintf(&filename, 0, "%s", ini_filename);

#ifdef HAVE_GLOB
	{
		g.gl_offs = 0;
		if ((i = glob(inc, GLOB_ERR | GLOB_MARK, NULL, &g)) != 0) {
#ifdef GLOB_NOMATCH
			if (i == GLOB_NOMATCH) {
				zlog(ZLOG_WARNING, "Nothing matches the include pattern '%s' from %s at line %d.", inc, filename, ini_lineno);
				efree(filename);
				return;
			}
#endif /* GLOB_NOMATCH */
			zlog(ZLOG_ERROR, "Unable to globalize '%s' (ret=%zd) from %s at line %d.", inc, i, filename, ini_lineno);
			*error = 1;
			efree(filename);
			return;
		}

		for (i = 0; i < g.gl_pathc; i++) {
			int len = strlen(g.gl_pathv[i]);
			if (len < 1) continue;
			if (g.gl_pathv[i][len - 1] == '/') continue; /* don't parse directories */
			if (0 > fpm_conf_load_ini_file(g.gl_pathv[i])) {
				zlog(ZLOG_ERROR, "Unable to include %s from %s at line %d", g.gl_pathv[i], filename, ini_lineno);
				*error = 1;
				efree(filename);
				return;
			}
		}
		globfree(&g);
	}
#else /* HAVE_GLOB */
	if (0 > fpm_conf_load_ini_file(inc)) {
		zlog(ZLOG_ERROR, "Unable to include %s from %s at line %d", inc, filename, ini_lineno);
		*error = 1;
		efree(filename);
		return;
	}
#endif /* HAVE_GLOB */

	efree(filename);
}
/* }}} */

static void fpm_conf_ini_parser_section(zval *section, void *arg) /* {{{ */
{
	struct fpm_worker_pool_s *wp;
	struct fpm_worker_pool_config_s *config;
	int *error = (int *)arg;

	/* switch to global conf */
	if (zend_string_equals_literal_ci(Z_STR_P(section), "global")) {
		current_wp = NULL;
		return;
	}

	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		if (!wp->config) continue;
		if (!wp->config->name) continue;
		if (!strcasecmp(wp->config->name, Z_STRVAL_P(section))) {
			/* Found a wp with the same name. Bring it back */
			current_wp = wp;
			return;
		}
	}

	/* it's a new pool */
	config = (struct fpm_worker_pool_config_s *)fpm_worker_pool_config_alloc();
	if (!current_wp || !config) {
		zlog(ZLOG_ERROR, "[%s:%d] Unable to alloc a new WorkerPool for worker '%s'", ini_filename, ini_lineno, Z_STRVAL_P(section));
		*error = 1;
		return;
	}
	config->name = strdup(Z_STRVAL_P(section));
	if (!config->name) {
		zlog(ZLOG_ERROR, "[%s:%d] Unable to alloc memory for configuration name for worker '%s'", ini_filename, ini_lineno, Z_STRVAL_P(section));
		*error = 1;
		return;
	}
}
/* }}} */

static void fpm_conf_ini_parser_entry(zval *name, zval *value, void *arg) /* {{{ */
{
	struct ini_value_parser_s *parser;
	void *config = NULL;

	int *error = (int *)arg;
	if (!value) {
		zlog(ZLOG_ERROR, "[%s:%d] value is NULL for a ZEND_INI_PARSER_ENTRY", ini_filename, ini_lineno);
		*error = 1;
		return;
	}

	if (zend_string_equals_literal(Z_STR_P(name), "include")) {
		if (ini_include) {
			zlog(ZLOG_ERROR, "[%s:%d] two includes at the same time !", ini_filename, ini_lineno);
			*error = 1;
			return;
		}
		ini_include = strdup(Z_STRVAL_P(value));
		return;
	}

	if (!current_wp) { /* we are in the global section */
		parser = ini_fpm_global_options;
		config = &fpm_global_config;
	} else {
		parser = ini_fpm_pool_options;
		config = current_wp->config;
	}

	for (; parser->name; parser++) {
		if (!strcasecmp(parser->name, Z_STRVAL_P(name))) {
			char *ret;
			if (!parser->parser) {
				zlog(ZLOG_ERROR, "[%s:%d] the parser for entry '%s' is not defined", ini_filename, ini_lineno, parser->name);
				*error = 1;
				return;
			}

			ret = parser->parser(value, &config, parser->offset);
			if (ret) {
				zlog(ZLOG_ERROR, "[%s:%d] unable to parse value for entry '%s': %s", ini_filename, ini_lineno, parser->name, ret);
				*error = 1;
				return;
			}

			/* all is good ! */
			return;
		}
	}

	/* nothing has been found if we got here */
	zlog(ZLOG_ERROR, "[%s:%d] unknown entry '%s'", ini_filename, ini_lineno, Z_STRVAL_P(name));
	*error = 1;
}
/* }}} */

static void fpm_conf_ini_parser_array(zval *name, zval *key, zval *value, void *arg) /* {{{ */
{
	int *error = (int *)arg;
	char *err = NULL;
	void *config;

	if (zend_string_equals_literal(Z_STR_P(name), "access.suppress_path")) {
		if (!(*Z_STRVAL_P(key) == '\0')) {
			zlog(ZLOG_ERROR, "[%s:%d] Keys provided to field 'access.suppress_path' are ignored", ini_filename, ini_lineno);
			*error = 1;
		}
		if (!(*Z_STRVAL_P(value)) || (*Z_STRVAL_P(value) != '/')) {
			zlog(ZLOG_ERROR, "[%s:%d] Values provided to field 'access.suppress_path' must begin with '/'", ini_filename, ini_lineno);
			*error = 1;
		}
		if (*error) {
			return;
		}
	} else if (!*Z_STRVAL_P(key)) {
		zlog(ZLOG_ERROR, "[%s:%d] You must provide a key for field '%s'", ini_filename, ini_lineno, Z_STRVAL_P(name));
		*error = 1;
		return;
	}

	if (!current_wp || !current_wp->config) {
		zlog(ZLOG_ERROR, "[%s:%d] Array are not allowed in the global section", ini_filename, ini_lineno);
		*error = 1;
		return;
	}

	if (zend_string_equals_literal(Z_STR_P(name), "env")) {
		if (!*Z_STRVAL_P(value)) {
			zlog(ZLOG_ERROR, "[%s:%d] empty value", ini_filename, ini_lineno);
			*error = 1;
			return;
		}
		config = (char *)current_wp->config + WPO(env);
		err = fpm_conf_set_array(key, value, &config, 0);

	} else if (zend_string_equals_literal(Z_STR_P(name), "php_value")) {
		config = (char *)current_wp->config + WPO(php_values);
		err = fpm_conf_set_array(key, value, &config, 0);

	} else if (zend_string_equals_literal(Z_STR_P(name), "php_admin_value")) {
		config = (char *)current_wp->config + WPO(php_admin_values);
		err = fpm_conf_set_array(key, value, &config, 0);

	} else if (zend_string_equals_literal(Z_STR_P(name), "php_flag")) {
		config = (char *)current_wp->config + WPO(php_values);
		err = fpm_conf_set_array(key, value, &config, 1);

	} else if (zend_string_equals_literal(Z_STR_P(name), "php_admin_flag")) {
		config = (char *)current_wp->config + WPO(php_admin_values);
		err = fpm_conf_set_array(key, value, &config, 1);

	} else if (zend_string_equals_literal(Z_STR_P(name), "access.suppress_path")) {
		config = (char *)current_wp->config + WPO(access_suppress_paths);
		err = fpm_conf_set_array(NULL, value, &config, 0);

	} else {
		zlog(ZLOG_ERROR, "[%s:%d] unknown directive '%s'", ini_filename, ini_lineno, Z_STRVAL_P(name));
		*error = 1;
		return;
	}

	if (err) {
		zlog(ZLOG_ERROR, "[%s:%d] error while parsing '%s[%s]' : %s", ini_filename, ini_lineno, Z_STRVAL_P(name), Z_STRVAL_P(key), err);
		*error = 1;
		return;
	}
}
/* }}} */

static void fpm_conf_ini_parser(zval *arg1, zval *arg2, zval *arg3, int callback_type, void *arg) /* {{{ */
{
	int *error;

	if (!arg1 || !arg) return;
	error = (int *)arg;
	if (*error) return; /* We got already an error. Switch to the end. */

	switch(callback_type) {
		case ZEND_INI_PARSER_ENTRY:
			fpm_conf_ini_parser_entry(arg1, arg2, error);
			break;
		case ZEND_INI_PARSER_SECTION:
			fpm_conf_ini_parser_section(arg1, error);
			break;
		case ZEND_INI_PARSER_POP_ENTRY:
			fpm_conf_ini_parser_array(arg1, arg3, arg2, error);
			break;
		default:
			zlog(ZLOG_ERROR, "[%s:%d] Unknown INI syntax", ini_filename, ini_lineno);
			*error = 1;
			break;
	}
}
/* }}} */

int fpm_conf_load_ini_file(char *filename) /* {{{ */
{
	int error = 0;
	char *buf = NULL, *newbuf = NULL;
	int bufsize = 0;
	int fd, n;
	int nb_read = 1;
	char c = '*';

	int ret = 1;

	if (!filename || !filename[0]) {
		zlog(ZLOG_ERROR, "configuration filename is empty");
		return -1;
	}

	fd = open(filename, O_RDONLY, 0);
	if (fd < 0) {
		zlog(ZLOG_SYSERROR, "failed to open configuration file '%s'", filename);
		return -1;
	}

	if (ini_recursion++ > 4) {
		zlog(ZLOG_ERROR, "failed to include more than 5 files recursively");
		close(fd);
		return -1;
	}

	ini_lineno = 0;
	while (nb_read > 0) {
		int tmp;
		ini_lineno++;
		ini_filename = filename;
		for (n = 0; (nb_read = read(fd, &c, sizeof(char))) == sizeof(char) && c != '\n'; n++) {
			if (n == bufsize) {
				bufsize += 1024;
				newbuf = (char*) realloc(buf, sizeof(char) * (bufsize + 2));
				if (newbuf == NULL) {
					ini_recursion--;
					close(fd);
					free(buf);
					return -1;
				}
				buf = newbuf;
			}

			buf[n] = c;
		}
		if (n == 0) {
			continue;
		}
		/* always append newline and null terminate */
		buf[n++] = '\n';
		buf[n] = '\0';
		tmp = zend_parse_ini_string(buf, 1, ZEND_INI_SCANNER_NORMAL, (zend_ini_parser_cb_t)fpm_conf_ini_parser, &error);
		ini_filename = filename;
		if (error || tmp == FAILURE) {
			if (ini_include) {
				free(ini_include);
				ini_include = NULL;
			}
			ini_recursion--;
			close(fd);
			free(buf);
			return -1;
		}
		if (ini_include) {
			char *tmp = ini_include;
			ini_include = NULL;
			fpm_evaluate_full_path(&tmp, NULL, NULL, 0);
			fpm_conf_ini_parser_include(tmp, &error);
			if (error) {
				free(tmp);
				ini_recursion--;
				close(fd);
				free(buf);
				return -1;
			}
			free(tmp);
		}
	}
	free(buf);

	ini_recursion--;
	close(fd);
	return ret;
}
/* }}} */

static void fpm_conf_dump(void)
{
	struct fpm_worker_pool_s *wp;

	/*
	 * Please keep the same order as in fpm_conf.h and in php-fpm.conf.in
	 */
	zlog(ZLOG_NOTICE, "[global]");
	zlog(ZLOG_NOTICE, "\tpid = %s",                         STR2STR(fpm_global_config.pid_file));
	zlog(ZLOG_NOTICE, "\terror_log = %s",                   STR2STR(fpm_global_config.error_log));
#ifdef HAVE_SYSLOG_H
	zlog(ZLOG_NOTICE, "\tsyslog.ident = %s",                STR2STR(fpm_global_config.syslog_ident));
	zlog(ZLOG_NOTICE, "\tsyslog.facility = %d",             fpm_global_config.syslog_facility); /* FIXME: convert to string */
#endif
	zlog(ZLOG_NOTICE, "\tlog_buffering = %s",               BOOL2STR(fpm_global_config.log_buffering));
	zlog(ZLOG_NOTICE, "\tlog_level = %s",                   zlog_get_level_name(fpm_globals.log_level));
	zlog(ZLOG_NOTICE, "\tlog_limit = %d",                   fpm_global_config.log_limit);
	zlog(ZLOG_NOTICE, "\temergency_restart_interval = %ds", fpm_global_config.emergency_restart_interval);
	zlog(ZLOG_NOTICE, "\temergency_restart_threshold = %d", fpm_global_config.emergency_restart_threshold);
	zlog(ZLOG_NOTICE, "\tprocess_control_timeout = %ds",    fpm_global_config.process_control_timeout);
	zlog(ZLOG_NOTICE, "\tprocess.max = %d",                 fpm_global_config.process_max);
	if (fpm_global_config.process_priority == 64) {
		zlog(ZLOG_NOTICE, "\tprocess.priority = undefined");
	} else {
		zlog(ZLOG_NOTICE, "\tprocess.priority = %d", fpm_global_config.process_priority);
	}
	zlog(ZLOG_NOTICE, "\tdaemonize = %s",                   BOOL2STR(fpm_global_config.daemonize));
	zlog(ZLOG_NOTICE, "\trlimit_files = %d",                fpm_global_config.rlimit_files);
	zlog(ZLOG_NOTICE, "\trlimit_core = %d",                 fpm_global_config.rlimit_core);
	zlog(ZLOG_NOTICE, "\tevents.mechanism = %s",            fpm_event_mechanism_name());
#ifdef HAVE_SYSTEMD
	zlog(ZLOG_NOTICE, "\tsystemd_interval = %ds",           fpm_global_config.systemd_interval/1000);
#endif
	zlog(ZLOG_NOTICE, " ");

	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		struct key_value_s *kv;

		if (!wp->config || wp->shared) {
			continue;
		}

		zlog(ZLOG_NOTICE, "[%s]",                              STR2STR(wp->config->name));
		zlog(ZLOG_NOTICE, "\tprefix = %s",                     STR2STR(wp->config->prefix));
		zlog(ZLOG_NOTICE, "\tuser = %s",                       STR2STR(wp->config->user));
		zlog(ZLOG_NOTICE, "\tgroup = %s",                      STR2STR(wp->config->group));
		zlog(ZLOG_NOTICE, "\tlisten = %s",                     STR2STR(wp->config->listen_address));
		zlog(ZLOG_NOTICE, "\tlisten.backlog = %d",             wp->config->listen_backlog);
#ifdef HAVE_FPM_ACL
		zlog(ZLOG_NOTICE, "\tlisten.acl_users = %s",           STR2STR(wp->config->listen_acl_users));
		zlog(ZLOG_NOTICE, "\tlisten.acl_groups = %s",          STR2STR(wp->config->listen_acl_groups));
#endif
		zlog(ZLOG_NOTICE, "\tlisten.owner = %s",               STR2STR(wp->config->listen_owner));
		zlog(ZLOG_NOTICE, "\tlisten.group = %s",               STR2STR(wp->config->listen_group));
		zlog(ZLOG_NOTICE, "\tlisten.mode = %s",                STR2STR(wp->config->listen_mode));
		zlog(ZLOG_NOTICE, "\tlisten.allowed_clients = %s",     STR2STR(wp->config->listen_allowed_clients));
#ifdef SO_SETFIB
		zlog(ZLOG_NOTICE, "\tlisten.setfib = %d",              wp->config->listen_setfib);
#endif
		if (wp->config->process_priority == 64) {
			zlog(ZLOG_NOTICE, "\tprocess.priority = undefined");
		} else {
			zlog(ZLOG_NOTICE, "\tprocess.priority = %d", wp->config->process_priority);
		}
		zlog(ZLOG_NOTICE, "\tprocess.dumpable = %s",           BOOL2STR(wp->config->process_dumpable));
		zlog(ZLOG_NOTICE, "\tpm = %s",                         PM2STR(wp->config->pm));
		zlog(ZLOG_NOTICE, "\tpm.max_children = %d",            wp->config->pm_max_children);
		zlog(ZLOG_NOTICE, "\tpm.start_servers = %d",           wp->config->pm_start_servers);
		zlog(ZLOG_NOTICE, "\tpm.min_spare_servers = %d",       wp->config->pm_min_spare_servers);
		zlog(ZLOG_NOTICE, "\tpm.max_spare_servers = %d",       wp->config->pm_max_spare_servers);
		zlog(ZLOG_NOTICE, "\tpm.max_spawn_rate = %d",          wp->config->pm_max_spawn_rate);
		zlog(ZLOG_NOTICE, "\tpm.process_idle_timeout = %d",    wp->config->pm_process_idle_timeout);
		zlog(ZLOG_NOTICE, "\tpm.max_requests = %d",            wp->config->pm_max_requests);
		zlog(ZLOG_NOTICE, "\tpm.status_path = %s",             STR2STR(wp->config->pm_status_path));
		zlog(ZLOG_NOTICE, "\tpm.status_listen = %s",           STR2STR(wp->config->pm_status_listen));
		zlog(ZLOG_NOTICE, "\tping.path = %s",                  STR2STR(wp->config->ping_path));
		zlog(ZLOG_NOTICE, "\tping.response = %s",              STR2STR(wp->config->ping_response));
		zlog(ZLOG_NOTICE, "\taccess.log = %s",                 STR2STR(wp->config->access_log));
		zlog(ZLOG_NOTICE, "\taccess.format = %s",              STR2STR(wp->config->access_format));
		for (kv = wp->config->access_suppress_paths; kv; kv = kv->next) {
			zlog(ZLOG_NOTICE, "\taccess.suppress_path[] = %s", kv->value);
		}
		zlog(ZLOG_NOTICE, "\tslowlog = %s",                    STR2STR(wp->config->slowlog));
		zlog(ZLOG_NOTICE, "\trequest_slowlog_timeout = %ds",   wp->config->request_slowlog_timeout);
		zlog(ZLOG_NOTICE, "\trequest_slowlog_trace_depth = %d", wp->config->request_slowlog_trace_depth);
		zlog(ZLOG_NOTICE, "\trequest_terminate_timeout = %ds", wp->config->request_terminate_timeout);
		zlog(ZLOG_NOTICE, "\trequest_terminate_timeout_track_finished = %s", BOOL2STR(wp->config->request_terminate_timeout_track_finished));
		zlog(ZLOG_NOTICE, "\trlimit_files = %d",               wp->config->rlimit_files);
		zlog(ZLOG_NOTICE, "\trlimit_core = %d",                wp->config->rlimit_core);
		zlog(ZLOG_NOTICE, "\tchroot = %s",                     STR2STR(wp->config->chroot));
		zlog(ZLOG_NOTICE, "\tchdir = %s",                      STR2STR(wp->config->chdir));
		zlog(ZLOG_NOTICE, "\tcatch_workers_output = %s",       BOOL2STR(wp->config->catch_workers_output));
		zlog(ZLOG_NOTICE, "\tdecorate_workers_output = %s",    BOOL2STR(wp->config->decorate_workers_output));
		zlog(ZLOG_NOTICE, "\tclear_env = %s",                  BOOL2STR(wp->config->clear_env));
		zlog(ZLOG_NOTICE, "\tsecurity.limit_extensions = %s",  wp->config->security_limit_extensions);

		for (kv = wp->config->env; kv; kv = kv->next) {
			zlog(ZLOG_NOTICE, "\tenv[%s] = %s", kv->key, kv->value);
		}

		for (kv = wp->config->php_values; kv; kv = kv->next) {
			zlog(ZLOG_NOTICE, "\tphp_value[%s] = %s", kv->key, kv->value);
		}

		for (kv = wp->config->php_admin_values; kv; kv = kv->next) {
			zlog(ZLOG_NOTICE, "\tphp_admin_value[%s] = %s", kv->key, kv->value);
		}
		zlog(ZLOG_NOTICE, " ");
	}
}

int fpm_conf_init_main(int test_conf, int force_daemon) /* {{{ */
{
	int ret;

	if (fpm_globals.prefix && *fpm_globals.prefix) {
		if (!fpm_conf_is_dir(fpm_globals.prefix)) {
			zlog(ZLOG_ERROR, "the global prefix '%s' does not exist or is not a directory", fpm_globals.prefix);
			return -1;
		}
	}

	if (fpm_globals.pid && *fpm_globals.pid) {
		fpm_global_config.pid_file = strdup(fpm_globals.pid);
	}

	if (fpm_globals.config == NULL) {
		char *tmp;

		if (fpm_globals.prefix == NULL) {
			spprintf(&tmp, 0, "%s/php-fpm.conf", PHP_SYSCONFDIR);
		} else {
			spprintf(&tmp, 0, "%s/etc/php-fpm.conf", fpm_globals.prefix);
		}

		if (!tmp) {
			zlog(ZLOG_SYSERROR, "spprintf() failed (tmp for fpm_globals.config)");
			return -1;
		}

		fpm_globals.config = strdup(tmp);
		efree(tmp);

		if (!fpm_globals.config) {
			zlog(ZLOG_SYSERROR, "spprintf() failed (fpm_globals.config)");
			return -1;
		}
	}

	ret = fpm_conf_load_ini_file(fpm_globals.config);

	if (0 > ret) {
		zlog(ZLOG_ERROR, "failed to load configuration file '%s'", fpm_globals.config);
		return -1;
	}

	if (0 > fpm_conf_post_process(force_daemon)) {
		zlog(ZLOG_ERROR, "failed to post process the configuration");
		return -1;
	}

	if (test_conf) {
		if (test_conf > 1) {
			fpm_conf_dump();
		}
		zlog(ZLOG_NOTICE, "configuration file %s test is successful\n", fpm_globals.config);
		fpm_globals.test_successful = 1;
		return -1;
	}

	if (0 > fpm_cleanup_add(FPM_CLEANUP_ALL, fpm_conf_cleanup, 0)) {
		return -1;
	}

	return 0;
}
/* }}} */
