
	/* $Id: fpm_conf.c,v 1.33.2.3 2008/12/13 03:50:29 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#if HAVE_INTTYPES_H
# include <inttypes.h>
#else
# include <stdint.h>
#endif
#ifdef HAVE_GLOB
# ifndef PHP_WIN32
#  include <glob.h>
# else
#  include "win32/glob.h"
# endif
#endif

#include <stdio.h>
#include <unistd.h>

#include "php.h"
#include "zend_ini_scanner.h"
#include "zend_globals.h"
#include "zend_stream.h"

#include "fpm.h"
#include "fpm_conf.h"
#include "fpm_stdio.h"
#include "fpm_worker_pool.h"
#include "fpm_cleanup.h"
#include "fpm_php.h"
#include "fpm_sockets.h"
#include "fpm_shm.h"
#include "fpm_status.h"
#include "zlog.h"

static int fpm_conf_load_ini_file(char *filename TSRMLS_DC);
static char *fpm_conf_set_integer(zval *value, void **config, intptr_t offset);
static char *fpm_conf_set_time(zval *value, void **config, intptr_t offset);
static char *fpm_conf_set_boolean(zval *value, void **config, intptr_t offset);
static char *fpm_conf_set_string(zval *value, void **config, intptr_t offset);
static char *fpm_conf_set_log_level(zval *value, void **config, intptr_t offset);
static char *fpm_conf_set_rlimit_core(zval *value, void **config, intptr_t offset);
static char *fpm_conf_set_pm(zval *value, void **config, intptr_t offset);

struct fpm_global_config_s fpm_global_config = { 0, 0, 0, 1, NULL, NULL};
static struct fpm_worker_pool_s *current_wp = NULL;
static int ini_recursion = 0;
static char *ini_filename = NULL;
static int ini_lineno = 0;
static char *ini_include = NULL;

static struct ini_value_parser_s ini_fpm_global_options[] = {
	{ "emergency_restart_threshold", 	&fpm_conf_set_integer, 	offsetof(struct fpm_global_config_s, emergency_restart_threshold) },
	{ "emergency_restart_interval",		&fpm_conf_set_time,			offsetof(struct fpm_global_config_s, emergency_restart_interval) },
	{ "process_control_timeout",			&fpm_conf_set_time,			offsetof(struct fpm_global_config_s, process_control_timeout) },
	{ "daemonize",										&fpm_conf_set_boolean,	offsetof(struct fpm_global_config_s, daemonize) },
	{ "pid",													&fpm_conf_set_string,		offsetof(struct fpm_global_config_s, pid_file) },
	{ "error_log",										&fpm_conf_set_string,		offsetof(struct fpm_global_config_s, error_log) },
	{ "log_level",										&fpm_conf_set_log_level,	0 },
	{ 0, 0, 0 }
};

static struct ini_value_parser_s ini_fpm_pool_options[] = {
	{ "user", &fpm_conf_set_string, offsetof(struct fpm_worker_pool_config_s, user) },
	{ "group", &fpm_conf_set_string, offsetof(struct fpm_worker_pool_config_s, group) },
	{ "chroot", &fpm_conf_set_string, offsetof(struct fpm_worker_pool_config_s, chroot) },
	{ "chdir", &fpm_conf_set_string, offsetof(struct fpm_worker_pool_config_s, chdir) },
	{ "request_terminate_timeout", &fpm_conf_set_time, offsetof(struct fpm_worker_pool_config_s, request_terminate_timeout) },
	{ "request_slowlog_timeout", &fpm_conf_set_time, offsetof(struct fpm_worker_pool_config_s, request_slowlog_timeout) },
	{ "slowlog", &fpm_conf_set_string, offsetof(struct fpm_worker_pool_config_s, slowlog) },
	{ "rlimit_files", &fpm_conf_set_integer, offsetof(struct fpm_worker_pool_config_s, rlimit_files) },
	{ "rlimit_core", &fpm_conf_set_rlimit_core, offsetof(struct fpm_worker_pool_config_s, rlimit_core) },
	{ "catch_workers_output", &fpm_conf_set_boolean, offsetof(struct fpm_worker_pool_config_s, catch_workers_output) },
	{ "listen", &fpm_conf_set_string, offsetof(struct fpm_worker_pool_config_s, listen_address) },
	{ "listen.owner", &fpm_conf_set_string, offsetof(struct fpm_worker_pool_config_s, listen_owner) },
	{ "listen.group", &fpm_conf_set_string, offsetof(struct fpm_worker_pool_config_s, listen_group) },
	{ "listen.mode", &fpm_conf_set_string, offsetof(struct fpm_worker_pool_config_s, listen_mode) },
	{ "listen.backlog", &fpm_conf_set_integer, offsetof(struct fpm_worker_pool_config_s, listen_backlog) },
	{ "listen.allowed_clients", &fpm_conf_set_string, offsetof(struct fpm_worker_pool_config_s, listen_allowed_clients) },
	{ "pm", &fpm_conf_set_pm, offsetof(struct fpm_worker_pool_config_s, pm) },
	{ "pm.max_requests", &fpm_conf_set_integer, offsetof(struct fpm_worker_pool_config_s, pm_max_requests) },
	{ "pm.max_children", &fpm_conf_set_integer, offsetof(struct fpm_worker_pool_config_s, pm_max_children) },
	{ "pm.start_servers", &fpm_conf_set_integer, offsetof(struct fpm_worker_pool_config_s, pm_start_servers) },
	{ "pm.min_spare_servers", &fpm_conf_set_integer, offsetof(struct fpm_worker_pool_config_s, pm_min_spare_servers) },
	{ "pm.max_spare_servers", &fpm_conf_set_integer, offsetof(struct fpm_worker_pool_config_s, pm_max_spare_servers) },
	{ "pm.status_path", &fpm_conf_set_string, offsetof(struct fpm_worker_pool_config_s, pm_status_path) },
	{ "ping.path", &fpm_conf_set_string, offsetof(struct fpm_worker_pool_config_s, ping_path) },
	{ "ping.response", &fpm_conf_set_string, offsetof(struct fpm_worker_pool_config_s, ping_response) },
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

static char *fpm_conf_set_boolean(zval *value, void **config, intptr_t offset) /* {{{ */
{
	char *val = Z_STRVAL_P(value);
	long value_y = !strcasecmp(val, "yes") || !strcmp(val,  "1") || !strcasecmp(val, "on") || !strcasecmp(val, "true");
	long value_n = !strcasecmp(val, "no")  || !strcmp(val,  "0") || !strcasecmp(val, "off") || !strcasecmp(val, "false");

	if (!value_y && !value_n) {
		return "invalid boolean value";
	}

	* (int *) ((char *) *config + offset) = value_y ? 1 : 0;
	return NULL;
}
/* }}} */

static char *fpm_conf_set_string(zval *value, void **config, intptr_t offset) /* {{{ */
{
	char *new;
	char **old = (char **) ((char *) *config + offset);
	if (*old) {
		return "it's already been defined. Can't do that twice.";
	}

	new = strdup(Z_STRVAL_P(value));
	if (!new) {
		return "fpm_conf_set_string(): strdup() failed";
	}

	*old = new;
	return NULL;
}
/* }}} */

static char *fpm_conf_set_integer(zval *value, void **config, intptr_t offset) /* {{{ */
{
	int i;
	char *val = Z_STRVAL_P(value);

	for (i=0; i<strlen(val); i++) {
		if ( i == 0 && val[i] == '-' ) continue;
		if (val[i] < '0' || val[i] > '9') {
			return("is not a valid number (greater or equal than zero");
		}
	}
	* (int *) ((char *) *config + offset) = atoi(val);
	return(NULL);
}
/* }}} */

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
	char *val = Z_STRVAL_P(value);

	if (!strcmp(val, "debug")) {
		fpm_globals.log_level = ZLOG_DEBUG;
	} else if (!strcasecmp(val, "notice")) {
		fpm_globals.log_level = ZLOG_NOTICE;
	} else if (!strcasecmp(val, "warning") || !strcasecmp(val, "warn")) {
		fpm_globals.log_level = ZLOG_WARNING;
	} else if (!strcasecmp(val, "error")) {
		fpm_globals.log_level = ZLOG_ERROR;
	} else if (!strcasecmp(val, "alert")) {
		fpm_globals.log_level = ZLOG_ALERT;
	} else {
		return "invalid value for 'log_level'";
	}

	return NULL;
}
/* }}} */

static char *fpm_conf_set_rlimit_core(zval *value, void **config, intptr_t offset) /* {{{ */
{
	char *val = Z_STRVAL_P(value);
	struct fpm_worker_pool_config_s *c = *config;

	if (!strcmp(val, "unlimited")) {
		c->rlimit_core = -1;
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

		c->rlimit_core = int_value;
	}

	return NULL;
}
/* }}} */

static char *fpm_conf_set_pm(zval *value, void **config, intptr_t offset) /* {{{ */
{
	char *val = Z_STRVAL_P(value);
	struct fpm_worker_pool_config_s  *c = *config;
	if (!strcmp(val, "static")) {
		c->pm = PM_STYLE_STATIC;
	} else if (!strcmp(val, "dynamic")) {
		c->pm = PM_STYLE_DYNAMIC;
	} else {
		return "invalid process manager (static or dynamic)";
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
	kv->key = strdup(Z_STRVAL_P(key));

	if (!kv->key) {
		return "fpm_conf_set_array: strdup(key) failed";
	}

	if (convert_to_bool) {
		char *err = fpm_conf_set_boolean(value, &subconf, 0);
		if (err) return(err);
		kv->value = strdup(b ? "On" : "Off");
	} else {
		kv->value = strdup(Z_STRVAL_P(value));
	}

	if (!kv->value) {
		free(kv->key);
		return "fpm_conf_set_array: strdup(value) failed";
	}

	kv->next = **parent;
	**parent = kv;
	return NULL;
}
/* }}} */

static void *fpm_worker_pool_config_alloc() /* {{{ */
{
	struct fpm_worker_pool_s *wp;

	wp = fpm_worker_pool_alloc();

	if (!wp) {
		return 0;
	}

	wp->config = malloc(sizeof(struct fpm_worker_pool_config_s));

	if (!wp->config) { 
		return 0;
	}

	memset(wp->config, 0, sizeof(struct fpm_worker_pool_config_s));
	wp->config->listen_backlog = -1;

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
/* }}} */

int fpm_worker_pool_config_free(struct fpm_worker_pool_config_s *wpc) /* {{{ */
{
	struct key_value_s *kv, *kv_next;

	free(wpc->name);
	free(wpc->pm_status_path);
	free(wpc->ping_path);
	free(wpc->ping_response);
	free(wpc->listen_address);
	free(wpc->listen_owner);
	free(wpc->listen_group);
	free(wpc->listen_mode);
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
	free(wpc->listen_allowed_clients);
	free(wpc->user);
	free(wpc->group);
	free(wpc->chroot);
	free(wpc->chdir);
	free(wpc->slowlog);

	return 0;
}
/* }}} */

static int fpm_evaluate_full_path(char **path) /* {{{ */
{
	if (**path != '/') {
		char *full_path;

		full_path = malloc(sizeof(PHP_PREFIX) + strlen(*path) + 1);

		if (!full_path) { 
			return -1;
		}

		sprintf(full_path, "%s/%s", PHP_PREFIX, *path);
		free(*path);
		*path = full_path;
	}

	return 0;
}
/* }}} */

static int fpm_conf_process_all_pools() /* {{{ */
{
	struct fpm_worker_pool_s *wp;

	if (!fpm_worker_all_pools) {
		zlog(ZLOG_STUFF, ZLOG_ERROR, "at least one pool section must be specified in config file");
		return -1;
	}

	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {

		if (wp->config->listen_address && *wp->config->listen_address) {
			wp->listen_address_domain = fpm_sockets_domain_from_address(wp->config->listen_address);

			if (wp->listen_address_domain == FPM_AF_UNIX && *wp->config->listen_address != '/') {
				fpm_evaluate_full_path(&wp->config->listen_address);
			}
		} else {
			zlog(ZLOG_STUFF, ZLOG_ALERT, "[pool %s] no listen address have been defined!", wp->config->name);
			return -1;
		}

		if (!wp->config->user) {
			zlog(ZLOG_STUFF, ZLOG_ALERT, "[pool %s] user has not been defined", wp->config->name);
			return -1;
		}

		if (wp->config->pm != PM_STYLE_STATIC && wp->config->pm != PM_STYLE_DYNAMIC) {
			zlog(ZLOG_STUFF, ZLOG_ALERT, "[pool %s] the process manager is missing (static or dynamic)", wp->config->name);
			return -1;
		}

		if (wp->config->pm_max_children < 1) {
			zlog(ZLOG_STUFF, ZLOG_ALERT, "[pool %s] pm.max_children must be a positive value", wp->config->name);
			return -1;
		}

		if (wp->config->pm == PM_STYLE_DYNAMIC) {
			struct fpm_worker_pool_config_s *config = wp->config;

			if (config->pm_min_spare_servers <= 0) {
				zlog(ZLOG_STUFF, ZLOG_ALERT, "[pool %s] pm.min_spare_servers(%d) must be a positive value", wp->config->name, config->pm_min_spare_servers);
				return -1;
			}

			if (config->pm_max_spare_servers <= 0) {
				zlog(ZLOG_STUFF, ZLOG_ALERT, "[pool %s] pm.max_spare_servers(%d) must be a positive value", wp->config->name, config->pm_max_spare_servers);
				return -1;
			}

			if (config->pm_min_spare_servers > config->pm_max_children ||
					config->pm_max_spare_servers > config->pm_max_children) {
				zlog(ZLOG_STUFF, ZLOG_ALERT, "[pool %s] pm.min_spare_servers(%d) and pm.max_spare_servers(%d) cannot be greater than pm.max_children(%d)",
						wp->config->name, config->pm_min_spare_servers, config->pm_max_spare_servers, config->pm_max_children);
				return -1;
			}

			if (config->pm_max_spare_servers < config->pm_min_spare_servers) {
				zlog(ZLOG_STUFF, ZLOG_ALERT, "[pool %s] pm.max_spare_servers(%d) must not be less than pm.min_spare_servers(%d)", wp->config->name, config->pm_max_spare_servers, config->pm_min_spare_servers);
				return -1;
			}

			if (config->pm_start_servers <= 0) {
				config->pm_start_servers = config->pm_min_spare_servers + ((config->pm_max_spare_servers - config->pm_min_spare_servers) / 2);
				zlog(ZLOG_STUFF, ZLOG_WARNING, "[pool %s] pm.start_servers is not set. It's been set to %d.", wp->config->name, config->pm_start_servers);
			} else if (config->pm_start_servers < config->pm_min_spare_servers || config->pm_start_servers > config->pm_max_spare_servers) {
				zlog(ZLOG_STUFF, ZLOG_ALERT, "[pool %s] pm.start_servers(%d) must not be less than pm.min_spare_servers(%d) and not greater than pm.max_spare_servers(%d)", wp->config->name, config->pm_start_servers, config->pm_min_spare_servers, config->pm_max_spare_servers);
				return -1;
			}

		}


		if (wp->config->request_slowlog_timeout) {
#if HAVE_FPM_TRACE
			if (! (wp->config->slowlog && *wp->config->slowlog)) {
				zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] 'slowlog' must be specified for use with 'request_slowlog_timeout'", wp->config->name);
				return -1;
			}
#else
			static int warned = 0;

			if (!warned) {
				zlog(ZLOG_STUFF, ZLOG_WARNING, "[pool %s] 'request_slowlog_timeout' is not supported on your system",	wp->config->name);
				warned = 1;
			}

			wp->config->request_slowlog_timeout = 0;
#endif
		}

		if (wp->config->request_slowlog_timeout && wp->config->slowlog && *wp->config->slowlog) {
			int fd;

			fpm_evaluate_full_path(&wp->config->slowlog);

			if (wp->config->request_slowlog_timeout) {
				fd = open(wp->config->slowlog, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);

				if (0 > fd) {
					zlog(ZLOG_STUFF, ZLOG_SYSERROR, "open(%s) failed", wp->config->slowlog);
					return -1;
				}
				close(fd);
			}
		}

		if (wp->config->ping_path && *wp->config->ping_path) {
			char *ping = wp->config->ping_path;
			int i;

			if (*ping != '/') {
				zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] the ping path '%s' must start with a '/'", wp->config->name, ping);
				return -1;
			}

			if (strlen(ping) < 2) {
				zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] the ping path '%s' is not long enough", wp->config->name, ping);
				return -1;
			}

			for (i=0; i<strlen(ping); i++) {
				if (!isalnum(ping[i]) && ping[i] != '/' && ping[i] != '-' && ping[i] != '_' && ping[i] != '.') {
					zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] the ping path '%s' must containt only the following characters '[alphanum]/_-.'", wp->config->name, ping);
					return -1;
				}
			}

			if (!wp->config->ping_response) {
				wp->config->ping_response = strdup("pong");
			} else {
				if (strlen(wp->config->ping_response) < 1) {
					zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] the ping response page '%s' is not long enough", wp->config->name, wp->config->ping_response);
					return -1;
				}
			}
		} else {
			if (wp->config->ping_response) {
				free(wp->config->ping_response);
				wp->config->ping_response = NULL;
			}
		}

		if (wp->config->pm_status_path && *wp->config->pm_status_path) {
			int i;
			char *status = wp->config->pm_status_path;
			/* struct fpm_status_s fpm_status; */

			if (*status != '/') {
				zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] the status path '%s' must start with a '/'", wp->config->name, status);
				return -1;
			}

			if (strlen(status) < 2) {
				zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] the status path '%s' is not long enough", wp->config->name, status);
				return -1;
			}

			for (i=0; i<strlen(status); i++) {
				if (!isalnum(status[i]) && status[i] != '/' && status[i] != '-' && status[i] != '_' && status[i] != '.') {
					zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] the status path '%s' must contain only the following characters '[alphanum]/_-.'", wp->config->name, status);
					return -1;
				}
			}
			wp->shm_status = fpm_shm_alloc(sizeof(struct fpm_status_s));
			if (!wp->shm_status) {
				zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] unable to allocate shared memory for status page '%s'", wp->config->name, status);
				return -1;
			}
			fpm_status_update_accepted_conn(wp->shm_status, 0);
			fpm_status_update_activity(wp->shm_status, -1, -1, -1, 1);
			fpm_status_set_pm(wp->shm_status, wp->config->pm);
			/* memset(&fpm_status.last_update, 0, sizeof(fpm_status.last_update)); */
		}

		if (wp->config->chroot && *wp->config->chroot) {
			if (*wp->config->chroot != '/') {
				zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] the chroot path '%s' must start with a '/'", wp->config->name, wp->config->chroot);
				return -1;
			}
			if (!fpm_conf_is_dir(wp->config->chroot)) {
				zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] the chroot path '%s' does not exist or is not a directory", wp->config->name, wp->config->chroot);
				return -1;
			}
		}

		if (wp->config->chdir && *wp->config->chdir) {
			if (*wp->config->chdir != '/') {
				zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] the chdir path '%s' must start with a '/'", wp->config->name, wp->config->chdir);
				return -1;
			}

			if (wp->config->chroot) {
				char *buf;
				size_t len;

				len = strlen(wp->config->chroot) + strlen(wp->config->chdir) + 1;
				buf = malloc(sizeof(char) * len);
				if (!buf) {
					zlog(ZLOG_STUFF, ZLOG_SYSERROR, "[pool %s] malloc() failed", wp->config->name);
					return -1;
				}
				snprintf(buf, len, "%s%s", wp->config->chroot, wp->config->chdir);
				if (!fpm_conf_is_dir(buf)) {
					zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] the chdir path '%s' within the chroot path '%s' ('%s') does not exist or is not a directory", wp->config->name, wp->config->chdir, wp->config->chroot, buf);
					free(buf);
					return -1;
				}
				free(buf);
			} else {
				if (!fpm_conf_is_dir(wp->config->chdir)) {
					zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] the chdir path '%s' does not exist or is not a directory", wp->config->name, wp->config->chdir);
					return -1;
				}
			}
		}
	}
	return 0;
}
/* }}} */

int fpm_conf_unlink_pid() /* {{{ */
{
	if (fpm_global_config.pid_file) {
		if (0 > unlink(fpm_global_config.pid_file)) {
			zlog(ZLOG_STUFF, ZLOG_SYSERROR, "unlink(\"%s\") failed", fpm_global_config.pid_file);
			return -1;
		}
	}
	return 0;
}
/* }}} */

int fpm_conf_write_pid() /* {{{ */
{
	int fd;

	if (fpm_global_config.pid_file) {
		char buf[64];
		int len;

		unlink(fpm_global_config.pid_file);
		fd = creat(fpm_global_config.pid_file, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

		if (fd < 0) {
			zlog(ZLOG_STUFF, ZLOG_SYSERROR, "creat(\"%s\") failed", fpm_global_config.pid_file);
			return -1;
		}

		len = sprintf(buf, "%d", (int) fpm_globals.parent_pid);

		if (len != write(fd, buf, len)) {
			zlog(ZLOG_STUFF, ZLOG_SYSERROR, "write() failed");
			return -1;
		}
		close(fd);
	}
	return 0;
}
/* }}} */

static int fpm_conf_post_process() /* {{{ */
{
	if (fpm_global_config.pid_file) {
		fpm_evaluate_full_path(&fpm_global_config.pid_file);
	}

	if (!fpm_global_config.error_log) {
		char *tmp_log_path;

		spprintf(&tmp_log_path, 0, "%s/log/php-fpm.log", PHP_LOCALSTATEDIR);
		fpm_global_config.error_log = strdup(tmp_log_path);
		efree(tmp_log_path);
	}

	fpm_evaluate_full_path(&fpm_global_config.error_log);

	if (0 > fpm_stdio_open_error_log(0)) {
		return -1;
	}

	return fpm_conf_process_all_pools();
}
/* }}} */

static void fpm_conf_cleanup(int which, void *arg) /* {{{ */
{
	free(fpm_global_config.pid_file);
	free(fpm_global_config.error_log);
	fpm_global_config.pid_file = 0;
	fpm_global_config.error_log = 0;
}
/* }}} */

static void fpm_conf_ini_parser_include(char *inc, void *arg TSRMLS_DC) /* {{{ */
{
	char *filename;
	int *error = (int *)arg;;
	glob_t g;
	int i;

	if (!inc || !arg) return;
	if (*error) return; /* We got already an error. Switch to the end. */
	spprintf(&filename, 0, "%s", ini_filename); 

#ifdef HAVE_GLOB
	{
		g.gl_offs = 0;
		if ((i = glob(inc, GLOB_ERR | GLOB_MARK | GLOB_NOSORT, NULL, &g)) != 0) {
#ifdef GLOB_NOMATCH
			if (i == GLOB_NOMATCH) {
				zlog(ZLOG_STUFF, ZLOG_WARNING, "Nothing matches the include pattern '%s' from %s at line %d.", inc, filename, ini_lineno);
				return;
			} 
#endif /* GLOB_NOMATCH */
			zlog(ZLOG_STUFF, ZLOG_ERROR, "Unable to globalize '%s' (ret=%d) from %s at line %d.", inc, i, filename, ini_lineno);
			*error = 1;
			return;
		}

		for(i=0; i<g.gl_pathc; i++) {
			int len = strlen(g.gl_pathv[i]);
			if (len < 1) continue;
			if (g.gl_pathv[i][len - 1] == '/') continue; /* don't parse directories */
			if (0 > fpm_conf_load_ini_file(g.gl_pathv[i] TSRMLS_CC)) {
				zlog(ZLOG_STUFF, ZLOG_ERROR, "Unable to include %s from %s at line %d", g.gl_pathv[i], filename, ini_lineno);
				*error = 1;
				return;
			}
		}
		globfree(&g);
	}
#else /* HAVE_GLOB */
	if (0 > fpm_conf_load_ini_file(inc TSRMLS_CC)) {
		zlog(ZLOG_STUFF, ZLOG_ERROR, "Unable to include %s from %s at line %d", inc, filename, ini_lineno);
		*error = 1;
		return;
	}
#endif /* HAVE_GLOB */
}
/* }}} */

static void fpm_conf_ini_parser_section(zval *section, void *arg TSRMLS_DC) /* {{{ */
{
	struct fpm_worker_pool_s *wp;
	struct fpm_worker_pool_config_s *config;
	int *error = (int *)arg;

	/* switch to global conf */
	if (!strcasecmp(Z_STRVAL_P(section), "global")) {
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
		zlog(ZLOG_STUFF, ZLOG_ERROR, "[%s:%d] Unable to alloc a new WorkerPool for worker '%s'", ini_filename, ini_lineno, Z_STRVAL_P(section));
		*error = 1;
		return;
	}
	config->name = strdup(Z_STRVAL_P(section));
	if (!config->name) {
		zlog(ZLOG_STUFF, ZLOG_ERROR, "[%s:%d] Unable to alloc memory for configuration name for worker '%s'", ini_filename, ini_lineno, Z_STRVAL_P(section));
		*error = 1;
		return;
	}
}
/* }}} */

static void fpm_conf_ini_parser_entry(zval *name, zval *value, void *arg TSRMLS_DC) /* {{{ */
{
	struct ini_value_parser_s *parser;
	void *config = NULL;

	int *error = (int *)arg;
	if (!value) {
		zlog(ZLOG_STUFF, ZLOG_ERROR, "[%s:%d] value is NULL for a ZEND_INI_PARSER_ENTRY", ini_filename, ini_lineno);
		*error = 1;
		return;
	}

	if (!strcmp(Z_STRVAL_P(name), "include")) {
		if (ini_include) {
			zlog(ZLOG_STUFF, ZLOG_ERROR, "[%s:%d] two includes at the same time !", ini_filename, ini_lineno);
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

	for (;parser->name; parser++) {
		if (!strcasecmp(parser->name, Z_STRVAL_P(name))) {
			char *ret;
			if (!parser->parser) {
				zlog(ZLOG_STUFF, ZLOG_ERROR, "[%s:%d] the parser for entry '%s' is not defined", ini_filename, ini_lineno, parser->name);
				*error = 1;
				return;
			}

			ret = parser->parser(value, &config, parser->offset);
			if (ret) {
				zlog(ZLOG_STUFF, ZLOG_ERROR, "[%s:%d] unable to parse value for entry '%s': %s", ini_filename, ini_lineno, parser->name, ret);
				*error = 1;
				return;
			}

			/* all is good ! */
			return;
		}
	}

	/* nothing has been found if we got here */
	zlog(ZLOG_STUFF, ZLOG_ERROR, "[%s:%d] unknown entry '%s'", ini_filename, ini_lineno, Z_STRVAL_P(name));
	*error = 1;
}
/* }}} */

static void fpm_conf_ini_parser_array(zval *name, zval *key, zval *value, void *arg TSRMLS_DC) /* {{{ */
{
	int *error = (int *)arg;
	char *err = NULL;
	void *config;

	if (!Z_STRVAL_P(key) || !Z_STRVAL_P(value) || !*Z_STRVAL_P(key) || !*Z_STRVAL_P(value)) {
		zlog(ZLOG_STUFF, ZLOG_ERROR, "[%s:%d] Mispell array ?", ini_filename, ini_lineno);
		*error = 1;
		return;
	}
	if (!current_wp || !current_wp->config) {
		zlog(ZLOG_STUFF, ZLOG_ERROR, "[%s:%d] Array are not allowed in the global section", ini_filename, ini_lineno);
		*error = 1;
		return;
	}

	if (!strcmp("env", Z_STRVAL_P(name))) {
		config = (char *)current_wp->config + offsetof(struct fpm_worker_pool_config_s, env);
		err = fpm_conf_set_array(key, value, &config, 0);

	} else if (!strcmp("php_value", Z_STRVAL_P(name))) {
		config = (char *)current_wp->config + offsetof(struct fpm_worker_pool_config_s, php_values);
		err = fpm_conf_set_array(key, value, &config, 0);

	} else if (!strcmp("php_admin_value", Z_STRVAL_P(name))) {
		config = (char *)current_wp->config + offsetof(struct fpm_worker_pool_config_s, php_admin_values);
		err = fpm_conf_set_array(key, value, &config, 0);

	} else if (!strcmp("php_flag", Z_STRVAL_P(name))) {
		config = (char *)current_wp->config + offsetof(struct fpm_worker_pool_config_s, php_values);
		err = fpm_conf_set_array(key, value, &config, 1);

	} else if (!strcmp("php_admin_flag", Z_STRVAL_P(name))) {
		config = (char *)current_wp->config + offsetof(struct fpm_worker_pool_config_s, php_admin_values);
		err = fpm_conf_set_array(key, value, &config, 1);

	} else {
		zlog(ZLOG_STUFF, ZLOG_ERROR, "[%s:%d] unknown directive '%s'", ini_filename, ini_lineno, Z_STRVAL_P(name));
		*error = 1;
		return;
	}

	if (err) {
		zlog(ZLOG_STUFF, ZLOG_ERROR, "[%s:%d] error while parsing '%s[%s]' : %s", ini_filename, ini_lineno, Z_STRVAL_P(name), Z_STRVAL_P(key), err);
		*error = 1;
		return;
	}
}
/* }}} */

static void fpm_conf_ini_parser(zval *arg1, zval *arg2, zval *arg3, int callback_type, void *arg TSRMLS_DC) /* {{{ */
{
	int *error;

	if (!arg1 || !arg) return;
	error = (int *)arg;
	if (*error) return; /* We got already an error. Switch to the end. */

	switch(callback_type) {
		case ZEND_INI_PARSER_ENTRY:
			fpm_conf_ini_parser_entry(arg1, arg2, error TSRMLS_CC);
			break;;
		case ZEND_INI_PARSER_SECTION:
			fpm_conf_ini_parser_section(arg1, error TSRMLS_CC);
			break;;
		case ZEND_INI_PARSER_POP_ENTRY:
			fpm_conf_ini_parser_array(arg1, arg3, arg2, error TSRMLS_CC);
			break;;
		default:
			zlog(ZLOG_STUFF, ZLOG_ERROR, "[%s:%d] Unknown INI syntax", ini_filename, ini_lineno);
			*error = 1;
			break;;
	}
}
/* }}} */

int fpm_conf_load_ini_file(char *filename TSRMLS_DC) /* {{{ */
{
	int error = 0;
	char buf[1024+1];
	int fd, n;
	int nb_read = 1;
	char c = '*';

	int ret = 1;

	if (!filename || !filename[0]) {
		zlog(ZLOG_STUFF, ZLOG_ERROR, "Configuration file is empty");
		return -1;
	}

	fd = open(filename, O_RDONLY, 0);
	if (fd < 0) {
		zlog(ZLOG_STUFF, ZLOG_ERROR, "Unable to open file '%s', errno=%d", filename, errno);
		return -1;
	}

	if (ini_recursion++ > 4) {
		zlog(ZLOG_STUFF, ZLOG_ERROR, "You can include more than 5 files recusively");
		return -1;
	}

	ini_lineno = 0;
	while (nb_read > 0) {
		int tmp;
		memset(buf, 0, sizeof(char) * (1024 + 1));
		for (n=0; n<1024 && (nb_read = read(fd, &c, sizeof(char))) == sizeof(char) && c != '\n'; n++) {
			buf[n] = c;
		}
		buf[n++] = '\n';
		ini_lineno++;
		ini_filename = filename;
		tmp = zend_parse_ini_string(buf, 1, ZEND_INI_SCANNER_RAW, (zend_ini_parser_cb_t)fpm_conf_ini_parser, &error TSRMLS_CC);
		ini_filename = filename;
		if (error || tmp == FAILURE) {
			if (ini_include) free(ini_include);
			ini_recursion--;
			close(fd);
			return -1;
		}
		if (ini_include) {
			char *tmp = ini_include;
			ini_include = NULL;
			fpm_evaluate_full_path(&tmp);
			fpm_conf_ini_parser_include(tmp, &error TSRMLS_CC);
			if (error) {
				free(tmp);
				ini_recursion--;
				close(fd);
				return -1;
			}
			free(tmp);
		}
	}

	ini_recursion--;
	close(fd);
	return ret;

}
/* }}} */

int fpm_conf_init_main() /* {{{ */
{
	char *filename = fpm_globals.config;
	int free = 0;
	int ret;
	TSRMLS_FETCH();

	if (filename == NULL) {
		spprintf(&filename, 0, "%s/php-fpm.conf", PHP_SYSCONFDIR);
		free = 1;
	}

	ret = fpm_conf_load_ini_file(filename TSRMLS_CC);

	if (0 > ret) {
		zlog(ZLOG_STUFF, ZLOG_ERROR, "failed to load configuration file '%s'", filename);
		if (free) efree(filename);
		return -1;
	}

	if (free) efree(filename);

	if (0 > fpm_conf_post_process()) {
		zlog(ZLOG_STUFF, ZLOG_ERROR, "failed to post process the configuration");
		return -1;
	}

	if (0 > fpm_cleanup_add(FPM_CLEANUP_ALL, fpm_conf_cleanup, 0)) {
		return -1;
	}

	return 0;
}
/* }}} */
