
	/* $Id: fpm_conf.h,v 1.12.2.2 2008/12/13 03:46:49 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_CONF_H
#define FPM_CONF_H 1

#include <stdint.h>
#include "php.h"

#define FPM_CONF_MAX_PONG_LENGTH 64

struct key_value_s;

struct key_value_s {
	struct key_value_s *next;
	char *key;
	char *value;
};

struct fpm_global_config_s {
	int emergency_restart_threshold;
	int emergency_restart_interval;
	int process_control_timeout;
	int daemonize;
	char *pid_file;
	char *error_log;
};

extern struct fpm_global_config_s fpm_global_config;

struct fpm_worker_pool_config_s {
	char *name;
	char *user;
	char *group;
	char *chroot;
	char *chdir;
	int request_terminate_timeout;
	int request_slowlog_timeout;
	char *slowlog;
	int rlimit_files;
	int rlimit_core;
	int catch_workers_output;
	int pm;
	int pm_max_children;
	char *pm_status_path;
	int pm_max_requests;
	int pm_start_servers;
	int pm_min_spare_servers;
	int pm_max_spare_servers;
	char *ping_path;
	char *ping_response;
	char *listen_address;
	int listen_backlog;
	char *listen_owner;
	char *listen_group;
	char *listen_mode;
	char *listen_allowed_clients;
	struct key_value_s *env;
	struct key_value_s *php_admin_values;
	struct key_value_s *php_values;
};

struct ini_value_parser_s {
	char *name;
	char *(*parser)(zval *, void **, intptr_t);
	intptr_t offset;
};

enum { PM_STYLE_STATIC = 1, PM_STYLE_DYNAMIC = 2 };

int fpm_conf_init_main();
int fpm_worker_pool_config_free(struct fpm_worker_pool_config_s *wpc);
int fpm_conf_write_pid();
int fpm_conf_unlink_pid();

#endif

