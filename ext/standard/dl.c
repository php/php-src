/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Brian Schaffner <brian@tool.net>                            |
   |          Shane Caraveo <shane@caraveo.com>                           |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "dl.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "SAPI.h"

#if !defined(PHP_WIN32) && !defined(NETWARE)
#include "build-defs.h"
#endif

#if defined(HAVE_LIBDL) || HAVE_MACH_O_DYLD_H
#include <stdlib.h>
#include <stdio.h>

#ifdef HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#ifdef PHP_WIN32
#include "win32/param.h"
#include "win32/winutil.h"
#define GET_DL_ERROR()	php_win_err()
#elif defined(NETWARE)
#ifdef NEW_LIBC
#include <sys/param.h>
#else
#include "netware/param.h"
#endif
#define GET_DL_ERROR()	dlerror()
#else
#include <sys/param.h>
#define GET_DL_ERROR()	DL_ERROR()
#endif

#endif /* defined(HAVE_LIBDL) || HAVE_MACH_O_DYLD_H */


/* {{{ proto int dl(string extension_filename)
   Load a PHP extension at runtime */
PHP_FUNCTION(dl)
{
	pval **file;

#ifdef ZTS
	if ((strncmp(sapi_module.name, "cgi", 3)!=0) && (strcmp(sapi_module.name, "cli")!=0)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not supported in multithreaded Web servers - use extension statements in your php.ini");
		RETURN_FALSE;
	}
#endif

	/* obtain arguments */
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &file) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(file);

	if (!PG(enable_dl)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Dynamically loaded extentions aren't enabled");
	} else if (PG(safe_mode)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Dynamically loaded extensions aren't allowed when running in Safe Mode");
	} else {
		php_dl(*file, MODULE_TEMPORARY, return_value TSRMLS_CC);
		EG(full_tables_cleanup) = 1;
	}
}

/* }}} */


#if defined(HAVE_LIBDL) || HAVE_MACH_O_DYLD_H

#ifdef ZTS
#define USING_ZTS 1
#else
#define USING_ZTS 0
#endif

/* {{{ php_dl
 */
void php_dl(pval *file, int type, pval *return_value TSRMLS_DC)
{
	void *handle;
	char *libpath;
	zend_module_entry *module_entry, *tmp;
	zend_module_entry *(*get_module)(void);
	int error_type;
	char *extension_dir;

	if (type==MODULE_PERSISTENT) {
		/* Use the configuration hash directly, the INI mechanism is not yet initialized */
		if (cfg_get_string("extension_dir", &extension_dir)==FAILURE) {
			extension_dir = PHP_EXTENSION_DIR;
		}
	} else {
		extension_dir = PG(extension_dir);
	}

	if (type==MODULE_TEMPORARY) {
		error_type = E_WARNING;
	} else {
		error_type = E_CORE_WARNING;
	}

	if (extension_dir && extension_dir[0]){
		int extension_dir_len = strlen(extension_dir);

		libpath = emalloc(extension_dir_len+Z_STRLEN_P(file)+2);

		if (IS_SLASH(extension_dir[extension_dir_len-1])) {
			sprintf(libpath, "%s%s", extension_dir, Z_STRVAL_P(file)); /* SAFE */
		} else {
			sprintf(libpath, "%s/%s", extension_dir, Z_STRVAL_P(file)); /* SAFE */
		}
	} else {
		libpath = estrndup(Z_STRVAL_P(file), Z_STRLEN_P(file));
	}

	/* load dynamic symbol */
	handle = DL_LOAD(libpath);
	if (!handle) {
		php_error_docref(NULL TSRMLS_CC, error_type, "Unable to load dynamic library '%s' - %s", libpath, GET_DL_ERROR());
		efree(libpath);
		RETURN_FALSE;
	}

	efree(libpath);

#ifndef NETWARE
	get_module = (zend_module_entry *(*)(void)) DL_FETCH_SYMBOL(handle, "get_module");

	/*
	 * some OS prepend _ to symbol names while their dynamic linker
	 * does not do that automatically. Thus we check manually for
	 * _get_module.
	 */

	if (!get_module)
		get_module = (zend_module_entry *(*)(void)) DL_FETCH_SYMBOL(handle, "_get_module");
#else
	/* NetWare doesn't support two NLMs exporting same symbol */
	{
		char symbol_name[64] = "\0";
		int module_name_length = Z_STRLEN_P(file) - 4;  /* '.nlm' is 4 characters; knock it off */

		/* Take the module name (e.g.: 'php_ldap') and append '@get_module' to it */
		strncpy(symbol_name, Z_STRVAL_P(file), module_name_length);
		symbol_name[module_name_length] = '\0';
		strcat(symbol_name, "@");
		strcat(symbol_name, "get_module");

		get_module = (zend_module_entry *(*)(void)) DL_FETCH_SYMBOL(handle, symbol_name);
	}
	/* NetWare doesn't prepend '_' to symbol names; so the corresponding portion of code is also
	   not required for NetWare */
#endif

	if (!get_module) {
		DL_UNLOAD(handle);
		php_error_docref(NULL TSRMLS_CC, error_type, "Invalid library (maybe not a PHP library) '%s' ", Z_STRVAL_P(file));
		RETURN_FALSE;
	}
	module_entry = get_module();
	if ((module_entry->zend_debug != ZEND_DEBUG) || (module_entry->zts != USING_ZTS)
		|| (module_entry->zend_api != ZEND_MODULE_API_NO)) {
		/* Check for pre-4.1.0 module which has a slightly different module_entry structure :( */
			struct pre_4_1_0_module_entry {
				  char *name;
				  zend_function_entry *functions;
				  int (*module_startup_func)(INIT_FUNC_ARGS);
				  int (*module_shutdown_func)(SHUTDOWN_FUNC_ARGS);
				  int (*request_startup_func)(INIT_FUNC_ARGS);
				  int (*request_shutdown_func)(SHUTDOWN_FUNC_ARGS);
				  void (*info_func)(ZEND_MODULE_INFO_FUNC_ARGS);
				  int (*global_startup_func)(void);
				  int (*global_shutdown_func)(void);
				  int globals_id;
				  int module_started;
				  unsigned char type;
				  void *handle;
				  int module_number;
				  unsigned char zend_debug;
				  unsigned char zts;
				  unsigned int zend_api;
			};

			char *name;
			int zend_api;
			unsigned char zend_debug, zts;

			if((  ((struct pre_4_1_0_module_entry *)module_entry)->zend_api > 20000000)
			   &&(((struct pre_4_1_0_module_entry *)module_entry)->zend_api < 20010901)) {
				name       = ((struct pre_4_1_0_module_entry *)module_entry)->name;
				zend_api   = ((struct pre_4_1_0_module_entry *)module_entry)->zend_api;
				zend_debug = ((struct pre_4_1_0_module_entry *)module_entry)->zend_debug;
				zts        = ((struct pre_4_1_0_module_entry *)module_entry)->zts; 
			} else {			
				name       = module_entry->name;
				zend_api   = module_entry->zend_api;
				zend_debug = module_entry->zend_debug;
				zts        = module_entry->zts; 
			}

			php_error(error_type,
					  "%s: Unable to initialize module\n"
					  "Module compiled with module API=%d, debug=%d, thread-safety=%d\n"
					  "PHP    compiled with module API=%d, debug=%d, thread-safety=%d\n"
					  "These options need to match\n",
					  name, zend_api, zend_debug, zts,
					  ZEND_MODULE_API_NO, ZEND_DEBUG, USING_ZTS);
			DL_UNLOAD(handle);
			RETURN_FALSE;
	}
	Z_TYPE_P(module_entry) = type;
	module_entry->module_number = zend_next_free_module();
	if (module_entry->module_startup_func) {
		if (module_entry->module_startup_func(type, module_entry->module_number TSRMLS_CC)==FAILURE) {
			php_error_docref(NULL TSRMLS_CC, error_type, "Unable to initialize module '%s'", module_entry->name);
			DL_UNLOAD(handle);
			RETURN_FALSE;
		}
	}
	zend_register_module(module_entry);

	if ((type == MODULE_TEMPORARY) && module_entry->request_startup_func) {
		if (module_entry->request_startup_func(type, module_entry->module_number TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, error_type, "Unable to initialize module '%s'", module_entry->name);
			DL_UNLOAD(handle);
			RETURN_FALSE;
		}
	}
	
	/* update the .request_started property... */
	if (zend_hash_find(&module_registry, module_entry->name, strlen(module_entry->name)+1, (void **) &tmp)==FAILURE) {
		php_error_docref(NULL TSRMLS_CC, error_type, "Loaded module '%s' got lost", module_entry->name);
		RETURN_FALSE;
	}
	tmp->handle = handle;
	
	RETURN_TRUE;
}
/* }}} */

PHP_MINFO_FUNCTION(dl)
{
	php_info_print_table_row(2, "Dynamic Library Support", "enabled");
}

#else

void php_dl(pval *file, int type, pval *return_value TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot dynamically load %s - dynamic modules are not supported", Z_STRVAL_P(file));
	RETURN_FALSE;
}

PHP_MINFO_FUNCTION(dl)
{
	PUTS("Dynamic Library support not available<br />.\n");
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
