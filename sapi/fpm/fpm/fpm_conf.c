
	/* $Id: fpm_conf.c,v 1.33.2.3 2008/12/13 03:50:29 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "fpm.h"
#include "fpm_conf.h"
#include "fpm_stdio.h"
#include "fpm_worker_pool.h"
#include "fpm_cleanup.h"
#include "fpm_php.h"
#include "fpm_sockets.h"
#include "xml_config.h"
#include "zlog.h"


struct fpm_global_config_s fpm_global_config;

static void *fpm_global_config_ptr()
{
	return &fpm_global_config;
}

static char *fpm_conf_set_log_level(void **conf, char *name, void *vv, intptr_t offset)
{
	char *value = vv;

	if (!strcmp(value, "debug")) {
		fpm_globals.log_level = ZLOG_DEBUG;
	}
	else if (!strcmp(value, "notice")) {
		fpm_globals.log_level = ZLOG_NOTICE;
	}
	else if (!strcmp(value, "warn")) {
		fpm_globals.log_level = ZLOG_WARNING;
	}
	else if (!strcmp(value, "error")) {
		fpm_globals.log_level = ZLOG_ERROR;
	}
	else if (!strcmp(value, "alert")) {
		fpm_globals.log_level = ZLOG_ALERT;
	}
	else {
		return "invalid value for 'log_level'";
	}

	return NULL;
}

static struct xml_conf_section xml_section_fpm_global_options = {
	.conf = &fpm_global_config_ptr,
	.path = "/configuration/global_options",
	.parsers = (struct xml_value_parser []) {
		{ XML_CONF_SCALAR,	"emergency_restart_threshold",		&xml_conf_set_slot_integer,		offsetof(struct fpm_global_config_s, emergency_restart_threshold) },
		{ XML_CONF_SCALAR,	"emergency_restart_interval",		&xml_conf_set_slot_time,		offsetof(struct fpm_global_config_s, emergency_restart_interval) },
		{ XML_CONF_SCALAR,	"process_control_timeout",			&xml_conf_set_slot_time,		offsetof(struct fpm_global_config_s, process_control_timeout) },
		{ XML_CONF_SCALAR,	"daemonize",						&xml_conf_set_slot_boolean,		offsetof(struct fpm_global_config_s, daemonize) },
		{ XML_CONF_SCALAR,	"pid_file",							&xml_conf_set_slot_string,		offsetof(struct fpm_global_config_s, pid_file) },
		{ XML_CONF_SCALAR,	"error_log",						&xml_conf_set_slot_string,		offsetof(struct fpm_global_config_s, error_log) },
		{ XML_CONF_SCALAR,  "log_level",						&fpm_conf_set_log_level,		0 },
		{ 0, 0, 0, 0 }
	}
};

static char *fpm_conf_set_pm_style(void **conf, char *name, void *vv, intptr_t offset)
{
	char *value = vv;
	struct fpm_pm_s *c = *conf;

	if (!strcmp(value, "static")) {
		c->style = PM_STYLE_STATIC;
	}
	else if (!strcmp(value, "apache-like")) {
		c->style = PM_STYLE_APACHE_LIKE;
	}
	else {
		return "invalid value for 'style'";
	}

	return NULL;
}

static char *fpm_conf_set_rlimit_core(void **conf, char *name, void *vv, intptr_t offset)
{
	char *value = vv;
	struct fpm_worker_pool_config_s *c = *conf;

	if (!strcmp(value, "unlimited")) {
		c->rlimit_core = -1;
	}
	else {
		int int_value;
		void *subconf = &int_value;
		char *error;

		error = xml_conf_set_slot_integer(&subconf, name, vv, 0);

		if (error) return error;

		if (int_value < 0) return "invalid value for 'rlimit_core'";

		c->rlimit_core = int_value;
	}

	return NULL;
}

static char *fpm_conf_set_catch_workers_output(void **conf, char *name, void *vv, intptr_t offset)
{
	struct fpm_worker_pool_config_s *c = *conf;
	int int_value;
	void *subconf = &int_value;
	char *error;

	error = xml_conf_set_slot_boolean(&subconf, name, vv, 0);

	if (error) return error;

	c->catch_workers_output = int_value;

	return NULL;
}

static struct xml_conf_section fpm_conf_set_apache_like_subsection_conf = {
	.path = "apache_like somewhere", /* fixme */
	.parsers = (struct xml_value_parser []) {
		{ XML_CONF_SCALAR, "StartServers",		&xml_conf_set_slot_integer, offsetof(struct fpm_pm_s, options_apache_like.StartServers) },
		{ XML_CONF_SCALAR, "MinSpareServers",	&xml_conf_set_slot_integer, offsetof(struct fpm_pm_s, options_apache_like.MinSpareServers) },
		{ XML_CONF_SCALAR, "MaxSpareServers",	&xml_conf_set_slot_integer, offsetof(struct fpm_pm_s, options_apache_like.MaxSpareServers) },
		{ 0, 0, 0, 0 }
	}
};

static char *fpm_conf_set_apache_like_subsection(void **conf, char *name, void *xml_node, intptr_t offset)
{
	return xml_conf_parse_section(conf, &fpm_conf_set_apache_like_subsection_conf, xml_node);
}

static struct xml_conf_section fpm_conf_set_listen_options_subsection_conf = {
	.path = "listen options somewhere", /* fixme */
	.parsers = (struct xml_value_parser []) {
		{ XML_CONF_SCALAR,		"backlog",		&xml_conf_set_slot_integer,		offsetof(struct fpm_listen_options_s, backlog) },
		{ XML_CONF_SCALAR,		"owner",		&xml_conf_set_slot_string,		offsetof(struct fpm_listen_options_s, owner) },
		{ XML_CONF_SCALAR,		"group",		&xml_conf_set_slot_string,		offsetof(struct fpm_listen_options_s, group) },
		{ XML_CONF_SCALAR,		"mode",			&xml_conf_set_slot_string,		offsetof(struct fpm_listen_options_s, mode) },
		{ 0, 0, 0, 0 }
	}
};

static char *fpm_conf_set_listen_options_subsection(void **conf, char *name, void *xml_node, intptr_t offset)
{
	void *subconf = (char *) *conf + offset;
	struct fpm_listen_options_s *lo;

	lo = malloc(sizeof(*lo));

	if (!lo) {
		return "malloc() failed";
	}

	memset(lo, 0, sizeof(*lo));

	lo->backlog = -1;

	* (struct fpm_listen_options_s **) subconf = lo;

	subconf = lo;

	return xml_conf_parse_section(&subconf, &fpm_conf_set_listen_options_subsection_conf, xml_node);
}

static struct xml_conf_section fpm_conf_set_pm_subsection_conf = {
	.path = "pm settings somewhere", /* fixme */
	.parsers = (struct xml_value_parser []) {
		{ XML_CONF_SCALAR,		"style",				&fpm_conf_set_pm_style,						0 },
		{ XML_CONF_SCALAR,		"max_children",			&xml_conf_set_slot_integer,					offsetof(struct fpm_pm_s, max_children) },
		{ XML_CONF_SUBSECTION,	"apache_like",			&fpm_conf_set_apache_like_subsection,		offsetof(struct fpm_pm_s, options_apache_like) },
		{ 0, 0, 0, 0 }
	}
};

static char *fpm_conf_set_pm_subsection(void **conf, char *name, void *xml_node, intptr_t offset)
{
	void *subconf = (char *) *conf + offset;
	struct fpm_pm_s *pm;

	pm = malloc(sizeof(*pm));

	if (!pm) {
		return "fpm_conf_set_pm_subsection(): malloc failed";
	}

	memset(pm, 0, sizeof(*pm));

	* (struct fpm_pm_s **) subconf = pm;

	subconf = pm;

	return xml_conf_parse_section(&subconf, &fpm_conf_set_pm_subsection_conf, xml_node);
}

static char *xml_conf_set_slot_key_value_pair(void **conf, char *name, void *vv, intptr_t offset)
{
	char *value = vv;
	struct key_value_s *kv;
	struct key_value_s ***parent = (struct key_value_s ***) conf;

	kv = malloc(sizeof(*kv));

	if (!kv) {
		return "malloc() failed";
	}

	memset(kv, 0, sizeof(*kv));

	kv->key = strdup(name);
	kv->value = strdup(value);

	if (!kv->key || !kv->value) {
		return "xml_conf_set_slot_key_value_pair(): strdup() failed";
	}

	**parent = kv;

	*parent = &kv->next;

	return NULL;
}

static struct xml_conf_section fpm_conf_set_key_value_pairs_subsection_conf = {
	.path = "key_value_pairs somewhere", /* fixme */
	.parsers = (struct xml_value_parser []) {
		{ XML_CONF_SCALAR, 0, &xml_conf_set_slot_key_value_pair, 0 },
		{ 0, 0, 0, 0 }
	}
};

static char *fpm_conf_set_key_value_pairs_subsection(void **conf, char *name, void *xml_node, intptr_t offset)
{
	void *next_kv = (char *) *conf + offset;

	return xml_conf_parse_section(&next_kv, &fpm_conf_set_key_value_pairs_subsection_conf, xml_node);
}

static void *fpm_worker_pool_config_alloc()
{
	static struct fpm_worker_pool_s *current_wp = 0;
	struct fpm_worker_pool_s *wp;

	wp = fpm_worker_pool_alloc();

	if (!wp) return 0;

	wp->config = malloc(sizeof(struct fpm_worker_pool_config_s));

	if (!wp->config) return 0;

	memset(wp->config, 0, sizeof(struct fpm_worker_pool_config_s));

	if (current_wp) current_wp->next = wp;

	current_wp = wp;

	return wp->config;
}

int fpm_worker_pool_config_free(struct fpm_worker_pool_config_s *wpc)
{
	struct key_value_s *kv, *kv_next;

	free(wpc->name);
	free(wpc->listen_address);
	if (wpc->listen_options) {
		free(wpc->listen_options->owner);
		free(wpc->listen_options->group);
		free(wpc->listen_options->mode);
		free(wpc->listen_options);
	}
	for (kv = wpc->php_defines; kv; kv = kv_next) {
		kv_next = kv->next;
		free(kv->key);
		free(kv->value);
		free(kv);
	}
	for (kv = wpc->environment; kv; kv = kv_next) {
		kv_next = kv->next;
		free(kv->key);
		free(kv->value);
		free(kv);
	}
	free(wpc->pm);
	free(wpc->user);
	free(wpc->group);
	free(wpc->chroot);
	free(wpc->chdir);
	free(wpc->allowed_clients);
	free(wpc->slowlog);

	return 0;
}

static struct xml_conf_section xml_section_fpm_worker_pool_config = {
	.conf = &fpm_worker_pool_config_alloc,
	.path = "/configuration/workers/pool",
	.parsers = (struct xml_value_parser []) {
		{ XML_CONF_SCALAR,		"name",							&xml_conf_set_slot_string,					offsetof(struct fpm_worker_pool_config_s, name) },
		{ XML_CONF_SCALAR,		"listen_address",				&xml_conf_set_slot_string,					offsetof(struct fpm_worker_pool_config_s, listen_address) },
		{ XML_CONF_SUBSECTION,	"listen_options",				&fpm_conf_set_listen_options_subsection,	offsetof(struct fpm_worker_pool_config_s, listen_options) },
		{ XML_CONF_SUBSECTION,	"php_defines",					&fpm_conf_set_key_value_pairs_subsection,	offsetof(struct fpm_worker_pool_config_s, php_defines) },
		{ XML_CONF_SCALAR,		"user",							&xml_conf_set_slot_string,					offsetof(struct fpm_worker_pool_config_s, user) },
		{ XML_CONF_SCALAR,		"group",						&xml_conf_set_slot_string,					offsetof(struct fpm_worker_pool_config_s, group) },
		{ XML_CONF_SCALAR,		"chroot",						&xml_conf_set_slot_string,					offsetof(struct fpm_worker_pool_config_s, chroot) },
		{ XML_CONF_SCALAR,		"chdir",						&xml_conf_set_slot_string,					offsetof(struct fpm_worker_pool_config_s, chdir) },
		{ XML_CONF_SCALAR,		"allowed_clients",				&xml_conf_set_slot_string,					offsetof(struct fpm_worker_pool_config_s, allowed_clients) },
		{ XML_CONF_SUBSECTION,	"environment",					&fpm_conf_set_key_value_pairs_subsection,	offsetof(struct fpm_worker_pool_config_s, environment) },
		{ XML_CONF_SCALAR,		"request_terminate_timeout",	&xml_conf_set_slot_time,					offsetof(struct fpm_worker_pool_config_s, request_terminate_timeout) },
		{ XML_CONF_SCALAR,		"request_slowlog_timeout",		&xml_conf_set_slot_time,					offsetof(struct fpm_worker_pool_config_s, request_slowlog_timeout) },
		{ XML_CONF_SCALAR,		"slowlog",						&xml_conf_set_slot_string,					offsetof(struct fpm_worker_pool_config_s, slowlog) },
		{ XML_CONF_SCALAR,		"rlimit_files",					&xml_conf_set_slot_integer,					offsetof(struct fpm_worker_pool_config_s, rlimit_files) },
		{ XML_CONF_SCALAR,		"rlimit_core",					&fpm_conf_set_rlimit_core,					0 },
		{ XML_CONF_SCALAR,		"max_requests",					&xml_conf_set_slot_integer,					offsetof(struct fpm_worker_pool_config_s, max_requests) },
		{ XML_CONF_SCALAR,		"catch_workers_output",			&fpm_conf_set_catch_workers_output,			0 },
		{ XML_CONF_SUBSECTION,	"pm",							&fpm_conf_set_pm_subsection,				offsetof(struct fpm_worker_pool_config_s, pm) },
		{ 0, 0, 0, 0 }
	}
};

static struct xml_conf_section *fpm_conf_all_sections[] = {
	&xml_section_fpm_global_options,
	&xml_section_fpm_worker_pool_config,
	0
};

static int fpm_evaluate_full_path(char **path)
{
	if (**path != '/') {
		char *full_path;

		full_path = malloc(sizeof(PHP_PREFIX) + strlen(*path) + 1);

		if (!full_path) return -1;

		sprintf(full_path, "%s/%s", PHP_PREFIX, *path);

		free(*path);

		*path = full_path;
	}

	return 0;
}

static int fpm_conf_process_all_pools()
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

		}
		else {

			wp->is_template = 1;

		}

		if (wp->config->request_slowlog_timeout) {
#if HAVE_FPM_TRACE
			if (! (wp->config->slowlog && *wp->config->slowlog)) {
				zlog(ZLOG_STUFF, ZLOG_ERROR, "pool %s: 'slowlog' must be specified for use with 'request_slowlog_timeout'",
					wp->config->name);
				return -1;
			}
#else
			static int warned = 0;

			if (!warned) {
				zlog(ZLOG_STUFF, ZLOG_WARNING, "pool %s: 'request_slowlog_timeout' is not supported on your system",
					wp->config->name);
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
	}

	return 0;
}

int fpm_conf_unlink_pid()
{
	if (fpm_global_config.pid_file) {

		if (0 > unlink(fpm_global_config.pid_file)) {
			zlog(ZLOG_STUFF, ZLOG_SYSERROR, "unlink(\"%s\") failed", fpm_global_config.pid_file);
			return -1;
		}

	}

	return 0;
}

int fpm_conf_write_pid()
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

static int fpm_conf_post_process()
{
	if (fpm_global_config.pid_file) {
		fpm_evaluate_full_path(&fpm_global_config.pid_file);
	}

	if (!fpm_global_config.error_log) {
		fpm_global_config.error_log = strdup(PHP_FPM_LOG_PATH);
	}

	fpm_evaluate_full_path(&fpm_global_config.error_log);

	if (0 > fpm_stdio_open_error_log(0)) {
		return -1;
	}

	return fpm_conf_process_all_pools();
}

static void fpm_conf_cleanup(int which, void *arg)
{
	free(fpm_global_config.pid_file);
	free(fpm_global_config.error_log);
	fpm_global_config.pid_file = 0;
	fpm_global_config.error_log = 0;
}

int fpm_conf_init_main()
{
	char *filename = fpm_globals.config;
	char *err;

	if (0 > xml_conf_sections_register(fpm_conf_all_sections)) {
		return -1;
	}

	if (filename == NULL) {
		filename = PHP_FPM_CONF_PATH;
	}

	err = xml_conf_load_file(filename);

	if (err) {
		zlog(ZLOG_STUFF, ZLOG_ERROR, "failed to load configuration file: %s", err);
		return -1;
	}

	if (0 > fpm_conf_post_process()) {
		return -1;
	}

	xml_conf_clean();

	if (0 > fpm_cleanup_add(FPM_CLEANUP_ALL, fpm_conf_cleanup, 0)) {
		return -1;
	}

	return 0;
}
