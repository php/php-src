/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   | (with helpful hints from Dean Gaudet <dgaudet@arctic.org>            |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifdef THREAD_SAFE
#include "tls.h"
#include "php.h"
#else
#include "httpd.h"
#include "http_config.h"
#if MODULE_MAGIC_NUMBER > 19980712
#include "ap_compat.h"
#else
#if MODULE_MAGIC_NUMBER > 19980324
#include "compat.h"
#endif
#endif
#include "http_core.h"
#include "http_main.h"
#include "http_protocol.h"
#include "http_request.h"
#include "http_log.h"
#endif


/* These are taken out of php_ini.h
 * they must be updated if php_ini.h changes!
 */
#define PHP_INI_USER    (1<<0)
#define PHP_INI_PERDIR  (1<<1)
#define PHP_INI_SYSTEM  (1<<2)

#include "util_script.h"

#include "php_version.h"
#include "mod_php3.h"
#if HAVE_MOD_DAV
# include "mod_dav.h"
#endif

/* ### these should be defined in mod_php3.h or somewhere else */
#define USE_PATH 1
#define IGNORE_URL 2

module MODULE_VAR_EXPORT php3_module;

#ifndef THREAD_SAFE
int saved_umask;
#else
#define GLOBAL(a) php3_globals->a
#define STATIC GLOBAL
#define TLS_VARS \
	php3_globals_struct *php3_globals; \
	php3_globals = TlsGetValue(TlsIndex);
#endif

#ifndef TLS_VARS
#define TLS_VARS
#endif

#ifndef GLOBAL
#define GLOBAL(x) x
#endif

#if WIN32|WINNT
/* popenf isn't working on Windows, use open instead*/
# ifdef popenf
#  undef popenf
# endif
# define popenf(p,n,f,m) open((n),(f),(m))
# ifdef pclosef
#  undef pclosef
# endif
# define pclosef(p,f) close(f)
#else
# define php3i_popenf(p,n,f,m) popenf((p),(n),(f),(m))
#endif

extern php3_ini_structure php3_ini;  /* active config */
extern php3_ini_structure php3_ini_master;  /* master copy of config */

int apache_php3_module_main(request_rec * r, int fd, int display_source_mode);
extern int php3_module_startup();
extern void php3_module_shutdown();
extern void php3_module_shutdown_for_exec();

extern int tls_create(void);
extern int tls_destroy(void);
extern int tls_startup(void);
extern int tls_shutdown(void);

#if WIN32|WINNT

/* 
	we will want to change this to the apache api
	process and thread entry and exit functions
*/
BOOL WINAPI DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
    switch( ul_reason_for_call ) {
    case DLL_PROCESS_ATTACH:
		/* 
		   I should be loading ini vars here 
		   and doing whatever true global inits
		   need to be done
		*/
		if (!tls_startup())
			return 0;
		if (!tls_create())
			return 0;

		break;
    case DLL_THREAD_ATTACH:
		if (!tls_create())
			return 0;
		/*		if (php3_module_startup()==FAILURE) {
			return FAILURE;
		}
*/		break;
    case DLL_THREAD_DETACH:
		if (!tls_destroy())
			return 0;
/*		if (initialized) {
			php3_module_shutdown();
			return SUCCESS;
		} else {
			return FAILURE;
		}
*/		break;
    case DLL_PROCESS_DETACH:
		/*
		    close down anything down in process_attach 
		*/
		if (!tls_destroy())
			return 0;
		if (!tls_shutdown())
			return 0;
		break;
    }
    return 1;
}
#endif

void php3_save_umask()
{
	TLS_VARS;
	GLOBAL(saved_umask) = umask(077);
	umask(GLOBAL(saved_umask));
}

void php3_restore_umask()
{
	TLS_VARS;
	umask(GLOBAL(saved_umask));
}

int send_php3(request_rec *r, int display_source_mode, char *filename)
{
	int fd, retval;
	php3_ini_structure *conf;

	/* We don't accept OPTIONS requests, but take everything else */
	if (r->method_number == M_OPTIONS) {
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}

	/* Make sure file exists */
	if (filename == NULL && r->finfo.st_mode == 0) {
		return NOT_FOUND;
	}

	/* grab configuration settings */
	conf = (php3_ini_structure *) get_module_config(r->per_dir_config,
													&php3_module);
	/* copy to active configuration */
	memcpy(&php3_ini,conf,sizeof(php3_ini_structure));

	/* If PHP parser engine has been turned off with a "php3_engine off"
	 * directive, then decline to handle this request
	 */
	if (!conf->engine) {
		r->content_type = "text/html";
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}
	if (filename == NULL) {
		filename = r->filename;
	}
	/* Open the file */
	if ((fd = popenf(r->pool, filename, O_RDONLY, 0)) == -1) {
		log_reason("file permissions deny server access", filename, r);
		return FORBIDDEN;
	}

	/* Apache 1.2 has a more complex mechanism for reading POST data */
#if MODULE_MAGIC_NUMBER > 19961007
	if ((retval = setup_client_block(r, REQUEST_CHUNKED_ERROR)))
		return retval;
#endif

	if (conf->last_modified) {
#if MODULE_MAGIC_NUMBER < 19970912
		if ((retval = set_last_modified(r, r->finfo.st_mtime))) {
			return retval;
		}
#else
		update_mtime (r, r->finfo.st_mtime);
		set_last_modified(r);
		set_etag(r);
#endif
	}
	/* Assume output will be HTML.  Individual scripts may change this 
	   further down the line */
	r->content_type = "text/html";

	/* Init timeout */
	hard_timeout("send", r);

	php3_save_umask();
	chdir_file(filename);
	add_common_vars(r);
	add_cgi_vars(r);
	apache_php3_module_main(r, fd, display_source_mode);

	/* Done, restore umask, turn off timeout, close file and return */
	php3_restore_umask();
	kill_timeout(r);
	pclosef(r->pool, fd);
	return OK;
}

int send_parsed_php3(request_rec * r)
{
	return send_php3(r, 0, NULL);
}

int send_parsed_php3_source(request_rec * r)
{
	return send_php3(r, 0, NULL);
}

/*
 * Create the per-directory config structure with defaults from php3_ini_master
 */
static void *php3_create_dir(pool * p, char *dummy)
{
	php3_ini_structure *new;

	php3_module_startup();  /* php3_ini_master is set up here */

	new = (php3_ini_structure *) palloc(p, sizeof(php3_ini_structure));
	memcpy(new,&php3_ini_master,sizeof(php3_ini_structure));


	return new;
}

/*
 * Merge in per-directory .conf directives
 */
static void *php3_merge_dir(pool *p, void *basev, void *addv) 
{
	php3_ini_structure *new = (php3_ini_structure *) palloc(p, sizeof(php3_ini_structure));
	php3_ini_structure *base = (php3_ini_structure *) basev;
	php3_ini_structure *add = (php3_ini_structure *) addv;
	php3_ini_structure orig = php3_ini_master;

	/* Start with the base config */
	memcpy(new,base,sizeof(php3_ini_structure));

	/* Now, add any fields that have changed in *add compared to the master config */
	if (add->smtp != orig.smtp) new->smtp = add->smtp;
	if (add->sendmail_path != orig.sendmail_path) new->sendmail_path = add->sendmail_path;
	if (add->sendmail_from != orig.sendmail_from) new->sendmail_from = add->sendmail_from;
	if (add->errors != orig.errors) new->errors = add->errors;
	if (add->track_errors != orig.track_errors) new->track_errors = add->track_errors;
	if (add->display_errors != orig.display_errors) new->display_errors = add->display_errors;
	if (add->log_errors != orig.log_errors) new->log_errors = add->log_errors;
	if (add->doc_root != orig.doc_root) new->doc_root = add->doc_root;
	if (add->user_dir != orig.user_dir) new->user_dir = add->user_dir;
	if (add->track_vars != orig.track_vars) new->track_vars = add->track_vars;
	if (add->cgi_ext != orig.cgi_ext) new->cgi_ext = add->cgi_ext;
	if (add->isapi_ext != orig.isapi_ext) new->isapi_ext = add->isapi_ext;
	if (add->nsapi_ext != orig.nsapi_ext) new->nsapi_ext = add->nsapi_ext;
	if (add->include_path != orig.include_path) new->include_path = add->include_path;
	if (add->auto_prepend_file != orig.auto_prepend_file) new->auto_prepend_file = add->auto_prepend_file;
	if (add->auto_append_file != orig.auto_append_file) new->auto_append_file = add->auto_append_file;
	if (add->upload_tmp_dir != orig.upload_tmp_dir) new->upload_tmp_dir = add->upload_tmp_dir;
	if (add->upload_max_filesize != orig.upload_max_filesize) new->upload_max_filesize = add->upload_max_filesize;
	if (add->extension_dir != orig.extension_dir) new->extension_dir = add->extension_dir;
	if (add->error_log != orig.error_log) new->error_log = add->error_log;
	/* skip the highlight stuff */
	if (add->xbithack != orig.xbithack) new->xbithack = add->xbithack;
	if (add->engine != orig.engine) new->engine = add->engine;
	if (add->last_modified != orig.last_modified) new->last_modified = add->last_modified;
	if (add->max_execution_time != orig.max_execution_time) new->max_execution_time = add->max_execution_time;
	if (add->memory_limit != orig.memory_limit) new->memory_limit = add->memory_limit;
	if (add->browscap != orig.browscap) new->browscap = add->browscap;
	if (add->arg_separator != orig.arg_separator) new->arg_separator = add->arg_separator;
	if (add->gpc_order != orig.gpc_order) new->gpc_order = add->gpc_order;
	if (add->error_prepend_string != orig.error_prepend_string) new->error_prepend_string = add->error_prepend_string;
	if (add->error_append_string != orig.error_append_string) new->error_append_string = add->error_append_string;
	if (add->open_basedir != orig.open_basedir) new->open_basedir = add->open_basedir;
	if (add->enable_dl != orig.enable_dl) new->enable_dl = add->enable_dl;
	if (add->dav_script != orig.dav_script) new->dav_script = add->dav_script;
	
	return new;
}

#if MODULE_MAGIC_NUMBER > 19961007
const char *php3flaghandler(cmd_parms * cmd, php3_ini_structure * conf, int val)
{
#else
char *php3flaghandler(cmd_parms * cmd, php3_ini_structure * conf, int val)
{
#endif
	int c = (int) cmd->info;

	switch (c) {
		case 0:
			conf->track_errors = val;
			break;
		case 5:
			conf->track_vars = val;
			break;
		case 7:
			conf->engine = val;
			break;
		case 8:
			conf->xbithack = val;
			break;
		case 9:
			conf->last_modified = val;
			break;
		case 10:
			conf->log_errors = val;
			break;
		case 11:
			conf->display_errors = val;
			break;
		case 13:
			conf->enable_dl = val;
			break;
	}
	return NULL;
}


#if MODULE_MAGIC_NUMBER > 19961007
#define CONST_PREFIX const
#else
#define CONST_PREFIX
#endif

CONST_PREFIX char *php_apache_value_handler(cmd_parms *cmd, php3_ini_structure *conf, char *arg1, char *arg2)
{
	php_alter_ini_entry(arg1, strlen(arg1)+1, arg2, strlen(arg2)+1, PHP_INI_PERDIR);
	return NULL;
}


CONST_PREFIX char *php_apache_flag_handler(cmd_parms *cmd, php3_ini_structure *conf, char *arg1, char *arg2)
{
	char bool_val[2];

	if (!strcmp(arg2, "On")) {
		bool_val[0] = '1';
	} else {
		bool_val[0] = '0';
	}
	bool_val[1] = 0;
	
	php_alter_ini_entry(arg1, strlen(arg1)+1, bool_val, 2, PHP_INI_PERDIR);
	return NULL;
}


#if MODULE_MAGIC_NUMBER > 19961007
const char *php3take1handler(cmd_parms * cmd, php3_ini_structure * conf, char *arg)
{
#else
char *php3take1handler(cmd_parms * cmd, php3_ini_structure * conf, char *arg)
{
#endif
	int c = (int) cmd->info;

	switch (c) {
		case 0:
			conf->errors = atoi(arg);
			break;
		case 1:
			conf->doc_root = pstrdup(cmd->pool, arg);
			break;
		case 2:
			conf->user_dir = pstrdup(cmd->pool, arg);
			break;
		case 4:
			conf->include_path = pstrdup(cmd->pool, arg);
			break;
		case 5:
			if (strncasecmp(arg, "none", 4)) {
				conf->auto_prepend_file = pstrdup(cmd->pool, arg);
			} else {
				conf->auto_prepend_file = "";
			}
			break;
		case 6:
			if (strncasecmp(arg, "none", 4)) {
				conf->auto_append_file = pstrdup(cmd->pool, arg);
			} else {
				conf->auto_append_file = "";
			}
			break;
		case 7:
			conf->upload_tmp_dir = pstrdup(cmd->pool, arg);
			break;
		case 8:
			conf->extension_dir = pstrdup(cmd->pool, arg);
			break;
		case 9:
			conf->error_log = pstrdup(cmd->pool, arg);
			break;
		case 10:
			conf->arg_separator = pstrdup(cmd->pool, arg);
			break;
		case 11:
			conf->max_execution_time = atoi(arg);
			break;
		case 12:
			conf->memory_limit = atoi(arg);
			break;
		case 13:
			conf->sendmail_path = pstrdup(cmd->pool, arg);
			break;
		case 14:
			conf->browscap = pstrdup(cmd->pool, arg);
			break;
		case 15:
			conf->gpc_order = pstrdup(cmd->pool, arg);
			break;
		case 16:
			conf->error_prepend_string = pstrdup(cmd->pool, arg);
			break;
		case 17:
			conf->error_append_string = pstrdup(cmd->pool, arg);
			break;
		case 18:
			conf->open_basedir = pstrdup(cmd->pool, arg);
			break;
		case 19:
			conf->upload_max_filesize = atol(arg);
			break;
		case 20:
			conf->dav_script = pstrdup(cmd->pool, arg);
			break;
	}
	return NULL;
}

int php3_xbithack_handler(request_rec * r)
{
	php3_ini_structure *conf;

	conf = (php3_ini_structure *) get_module_config(r->per_dir_config, &php3_module);
	if (!(r->finfo.st_mode & S_IXUSR)) {
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}
	if (conf->xbithack == 0) {
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}
	return send_parsed_php3(r);
}

void php3_init_handler(server_rec *s, pool *p)
{
	register_cleanup(p, NULL, php3_module_shutdown, php3_module_shutdown_for_exec);
#if MODULE_MAGIC_NUMBER >= 19980527
	ap_add_version_component("PHP/" PHP_VERSION);
#endif
}


#if HAVE_MOD_DAV

extern int phpdav_mkcol_test_handler(request_rec *r);
extern int phpdav_mkcol_create_handler(request_rec *r);

/* conf is being read twice (both here and in send_php3()) */
int send_parsed_php3_dav_script(request_rec *r)
{
	php3_ini_structure *conf;

	conf = (php3_ini_structure *) get_module_config(r->per_dir_config,
													&php3_module);
	return send_php3(r, 0, 0, conf->dav_script);
}

static int php3_type_checker(request_rec *r)
{
	php3_ini_structure *conf;

	conf = (php3_ini_structure *)get_module_config(r->per_dir_config,
												   &php3_module);

    /* If DAV support is enabled, use mod_dav's type checker. */
    if (conf->dav_script) {
		dav_api_set_request_handler(r, send_parsed_php3_dav_script);
		dav_api_set_mkcol_handlers(r, phpdav_mkcol_test_handler,
								   phpdav_mkcol_create_handler);
		/* leave the rest of the request to mod_dav */
		return dav_api_type_checker(r);
	}

    return DECLINED;
}

#else /* HAVE_MOD_DAV */

# define php3_type_checker NULL

#endif /* HAVE_MOD_DAV */


handler_rec php3_handlers[] =
{
	{"application/x-httpd-php3", send_parsed_php3},
	{"application/x-httpd-php3-source", send_parsed_php3_source},
	{"text/html", php3_xbithack_handler},
	{NULL}
};


command_rec php3_commands[] =
{
	{"php4_directive",		php_apache_value_handler, NULL, OR_OPTIONS, TAKE2, "PHP Value Modifier"},
	{"php4_flag",			php_apache_flag_handler, NULL, OR_OPTIONS, TAKE2, "PHP Flag Modifier"},
	{"php3_error_reporting", php3take1handler, (void *)0, OR_OPTIONS, TAKE1, "error reporting level"},
	{"php3_doc_root", php3take1handler, (void *)1, ACCESS_CONF|RSRC_CONF, TAKE1, "directory"}, /* not used yet */
	{"php3_user_dir", php3take1handler, (void *)2, ACCESS_CONF|RSRC_CONF, TAKE1, "user directory"}, /* not used yet */
	{"php3_include_path", php3take1handler, (void *)4, OR_OPTIONS, TAKE1, "colon-separated path"},
	{"php3_auto_prepend_file", php3take1handler, (void *)5, OR_OPTIONS, TAKE1, "file name"},
	{"php3_auto_append_file", php3take1handler, (void *)6, OR_OPTIONS, TAKE1, "file name"},
	{"php3_upload_tmp_dir", php3take1handler, (void *)7,  ACCESS_CONF|RSRC_CONF, TAKE1, "directory"},
	{"php3_extension_dir", php3take1handler, (void *)8,  ACCESS_CONF|RSRC_CONF, TAKE1, "directory"},
	{"php3_error_log", php3take1handler, (void *)9, OR_OPTIONS, TAKE1, "error log file"},
	{"php3_arg_separator", php3take1handler, (void *)10, OR_OPTIONS, TAKE1, "GET method arg separator"},
	{"php3_max_execution_time", php3take1handler, (void *)11, OR_OPTIONS, TAKE1, "Max script run time in seconds"},
	{"php3_memory_limit", php3take1handler, (void *)12, OR_OPTIONS, TAKE1, "Max memory in bytes a script may use"},
	{"php3_sendmail_path", php3take1handler, (void *)13, OR_OPTIONS, TAKE1, "Full path to sendmail binary"},
	{"php3_browscap", php3take1handler, (void *)14, OR_OPTIONS, TAKE1, "Full path to browscap file"},
	{"php3_gpc_order", php3take1handler, (void *)15, OR_OPTIONS, TAKE1, "Set GET-COOKIE-POST order [default is GPC]"},
	{"php3_error_prepend_string", php3take1handler, (void *)16, OR_OPTIONS, TAKE1, "String to add before an error message from PHP"},
	{"php3_error_append_string", php3take1handler, (void *)17, OR_OPTIONS, TAKE1, "String to add after an error message from PHP"},
	{"php3_open_basedir", php3take1handler, (void *)18, OR_OPTIONS|RSRC_CONF, TAKE1, "Limit opening of files to this directory"},
	{"php3_upload_max_filesize", php3take1handler, (void *)19, OR_OPTIONS|RSRC_CONF, TAKE1, "Limit uploaded files to this many bytes"},
#if HAVE_MOD_DAV
	{"php3_dav_script", php3take1handler, (void *)20, OR_OPTIONS|RSRC_CONF, TAKE1,
	 "Lets PHP handle DAV requests by parsing this script."},
#endif
	{"php3_track_errors", php3flaghandler, (void *)0, OR_OPTIONS, FLAG, "on|off"},
	{"php3_track_vars", php3flaghandler, (void *)5, OR_OPTIONS, FLAG, "on|off"},
	{"php3_engine", php3flaghandler, (void *)7, RSRC_CONF|ACCESS_CONF, FLAG, "on|off"},
	{"php3_xbithack", php3flaghandler, (void *)8, OR_OPTIONS, FLAG, "on|off"},
	{"php3_last_modified", php3flaghandler, (void *)9, OR_OPTIONS, FLAG, "on|off"},
	{"php3_log_errors", php3flaghandler, (void *)10, OR_OPTIONS, FLAG, "on|off"},
	{"php3_display_errors", php3flaghandler, (void *)11, OR_OPTIONS, FLAG, "on|off"},
	{"php3_enable_dl", php3flaghandler, (void *)13, RSRC_CONF|ACCESS_CONF, FLAG, "on|off"},
	{NULL}
};



module MODULE_VAR_EXPORT php3_module =
{
	STANDARD_MODULE_STUFF,
	php3_init_handler,			/* initializer */
	php3_create_dir,			/* per-directory config creator */
	php3_merge_dir,				/* dir merger */
	NULL,						/* per-server config creator */
	NULL, 						/* merge server config */
	php3_commands,				/* command table */
	php3_handlers,				/* handlers */
	NULL,						/* filename translation */
	NULL,						/* check_user_id */
	NULL,						/* check auth */
	NULL,						/* check access */
	php3_type_checker,			/* type_checker */
	NULL,						/* fixups */
	NULL						/* logger */
#if MODULE_MAGIC_NUMBER >= 19970103
	,NULL						/* header parser */
#endif
#if MODULE_MAGIC_NUMBER >= 19970719
	,NULL             			/* child_init */
#endif
#if MODULE_MAGIC_NUMBER >= 19970728
	,NULL						/* child_exit */
#endif
#if MODULE_MAGIC_NUMBER >= 19970902
	,NULL						/* post read-request */
#endif
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
