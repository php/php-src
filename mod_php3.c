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
   | PHP4 patches by Zeev Suraski <zeev@zend.com>                         |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

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

php_apache_info_struct php_apache_info;		/* active config */

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

	/* We don't accept OPTIONS requests, but take everything else */
	if (r->method_number == M_OPTIONS) {
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}

	/* Make sure file exists */
	if (filename == NULL && r->finfo.st_mode == 0) {
		return NOT_FOUND;
	}

	/* If PHP parser engine has been turned off with a "php3_engine off"
	 * directive, then decline to handle this request
	 */
	if (!php_apache_info.engine) {
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

	if (php_apache_info.last_modified) {
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
 * Create the per-directory config structure with defaults
 */
static void *php3_create_dir(pool * p, char *dummy)
{
	php_apache_info_struct *new;

	new = (php_apache_info_struct *) palloc(p, sizeof(php_apache_info_struct));
	memcpy(new, &php_apache_info, sizeof(php_apache_info_struct));

	return new;
}


#if MODULE_MAGIC_NUMBER > 19961007
#define CONST_PREFIX const
#else
#define CONST_PREFIX
#endif

CONST_PREFIX char *php_apache_value_handler(cmd_parms *cmd, php_apache_info_struct *conf, char *arg1, char *arg2)
{
	php_alter_ini_entry(arg1, strlen(arg1)+1, arg2, strlen(arg2)+1, PHP_INI_PERDIR);
	return NULL;
}


CONST_PREFIX char *php_apache_flag_handler(cmd_parms *cmd, php_apache_info_struct *conf, char *arg1, char *arg2)
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


int php3_xbithack_handler(request_rec * r)
{
	php_apache_info_struct *conf;

	conf = (php_apache_info_struct *) get_module_config(r->per_dir_config, &php3_module);
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
	php3_module_startup();
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
	php_apache_info_struct *conf;

	conf = (php_apache_info_struct *) get_module_config(r->per_dir_config,
													&php3_module);
	return send_php3(r, 0, 0, conf->dav_script);
}

static int php3_type_checker(request_rec *r)
{
	php_apache_info_struct *conf;

	conf = (php_apache_info_struct *)get_module_config(r->per_dir_config,
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
	{"php4_value",		php_apache_value_handler, NULL, OR_OPTIONS, TAKE2, "PHP Value Modifier"},
	{"php4_flag",			php_apache_flag_handler, NULL, OR_OPTIONS, TAKE2, "PHP Flag Modifier"},
	{NULL}
};



module MODULE_VAR_EXPORT php3_module =
{
	STANDARD_MODULE_STUFF,
	php3_init_handler,			/* initializer */
	php3_create_dir,			/* per-directory config creator */
	NULL,						/* dir merger */
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
