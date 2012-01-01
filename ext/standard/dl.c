/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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
#include "php_ini.h"
#include "ext/standard/info.h"

#include "SAPI.h"

#if defined(HAVE_LIBDL)
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
#include <sys/param.h>
#define GET_DL_ERROR()	dlerror()
#else
#include <sys/param.h>
#define GET_DL_ERROR()	DL_ERROR()
#endif
#endif /* defined(HAVE_LIBDL) */

/* {{{ proto int dl(string extension_filename)
   Load a PHP extension at runtime */
PHPAPI PHP_FUNCTION(dl)
{
	char *filename;
	int filename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
		return;
	}

	if (!PG(enable_dl)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Dynamically loaded extensions aren't enabled");
		RETURN_FALSE;
	}

	if (filename_len >= MAXPATHLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "File name exceeds the maximum allowed length of %d characters", MAXPATHLEN);
		RETURN_FALSE;
	}

	if ((strncmp(sapi_module.name, "cgi", 3) != 0) &&
		(strcmp(sapi_module.name, "cli") != 0) &&
		(strncmp(sapi_module.name, "embed", 5) != 0)
	) {
#ifdef ZTS
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not supported in multithreaded Web servers - use extension=%s in your php.ini", filename);
		RETURN_FALSE;
#else
		php_error_docref(NULL TSRMLS_CC, E_DEPRECATED, "dl() is deprecated - use extension=%s in your php.ini", filename);
#endif
	}

	php_dl(filename, MODULE_TEMPORARY, return_value, 0 TSRMLS_CC);
	if (Z_LVAL_P(return_value) == 1) {
		EG(full_tables_cleanup) = 1;
	}
}
/* }}} */

#if defined(HAVE_LIBDL)

#ifdef ZTS
#define USING_ZTS 1
#else
#define USING_ZTS 0
#endif

/* {{{ php_dl
 */
PHPAPI int php_load_extension(char *filename, int type, int start_now TSRMLS_DC) /* {{{ */
{
	void *handle;
	char *libpath;
	zend_module_entry *module_entry;
	zend_module_entry *(*get_module)(void);
	int error_type;
	char *extension_dir;

	if (type == MODULE_PERSISTENT) {
		extension_dir = INI_STR("extension_dir");
	} else {
		extension_dir = PG(extension_dir);
	}

	if (type == MODULE_TEMPORARY) {
		error_type = E_WARNING;
	} else {
		error_type = E_CORE_WARNING;
	}

	/* Check if passed filename contains directory separators */
	if (strchr(filename, '/') != NULL || strchr(filename, DEFAULT_SLASH) != NULL) {
		/* Passing modules with full path is not supported for dynamically loaded extensions */
		if (type == MODULE_TEMPORARY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Temporary module name should contain only filename");
			return FAILURE;
		}
		libpath = estrdup(filename);
	} else if (extension_dir && extension_dir[0]) {
		int extension_dir_len = strlen(extension_dir);

		if (IS_SLASH(extension_dir[extension_dir_len-1])) {
			spprintf(&libpath, 0, "%s%s", extension_dir, filename); /* SAFE */
		} else {
			spprintf(&libpath, 0, "%s%c%s", extension_dir, DEFAULT_SLASH, filename); /* SAFE */
		}
	} else {
		return FAILURE; /* Not full path given or extension_dir is not set */
	}

	/* load dynamic symbol */
	handle = DL_LOAD(libpath);
	if (!handle) {
#if PHP_WIN32
		char *err = GET_DL_ERROR();
		if (err && (*err != "")) {
			php_error_docref(NULL TSRMLS_CC, error_type, "Unable to load dynamic library '%s' - %s", libpath, err);
			LocalFree(err);
		} else {
			php_error_docref(NULL TSRMLS_CC, error_type, "Unable to load dynamic library '%s' - %s", libpath, "Unknown reason");
		}
#else
		php_error_docref(NULL TSRMLS_CC, error_type, "Unable to load dynamic library '%s' - %s", libpath, GET_DL_ERROR());
		GET_DL_ERROR(); /* free the buffer storing the error */
#endif
		efree(libpath);
		return FAILURE;
	}
	efree(libpath);

	get_module = (zend_module_entry *(*)(void)) DL_FETCH_SYMBOL(handle, "get_module");

	/* Some OS prepend _ to symbol names while their dynamic linker
	 * does not do that automatically. Thus we check manually for
	 * _get_module. */

	if (!get_module) {
		get_module = (zend_module_entry *(*)(void)) DL_FETCH_SYMBOL(handle, "_get_module");
	}

	if (!get_module) {
		DL_UNLOAD(handle);
		php_error_docref(NULL TSRMLS_CC, error_type, "Invalid library (maybe not a PHP library) '%s'", filename);
		return FAILURE;
	}
	module_entry = get_module();
	if (module_entry->zend_api != ZEND_MODULE_API_NO) {
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

			const char *name;
			int zend_api;

			if ((((struct pre_4_1_0_module_entry *)module_entry)->zend_api > 20000000) &&
				(((struct pre_4_1_0_module_entry *)module_entry)->zend_api < 20010901)
			) {
				name		= ((struct pre_4_1_0_module_entry *)module_entry)->name;
				zend_api	= ((struct pre_4_1_0_module_entry *)module_entry)->zend_api;
			} else {
				name		= module_entry->name;
				zend_api	= module_entry->zend_api;
			}

			php_error_docref(NULL TSRMLS_CC, error_type,
					"%s: Unable to initialize module\n"
					"Module compiled with module API=%d\n"
					"PHP    compiled with module API=%d\n"
					"These options need to match\n",
					name, zend_api, ZEND_MODULE_API_NO);
			DL_UNLOAD(handle);
			return FAILURE;
	}
	if(strcmp(module_entry->build_id, ZEND_MODULE_BUILD_ID)) {
		php_error_docref(NULL TSRMLS_CC, error_type,
				"%s: Unable to initialize module\n"
				"Module compiled with build ID=%s\n"
				"PHP    compiled with build ID=%s\n"
				"These options need to match\n",
				module_entry->name, module_entry->build_id, ZEND_MODULE_BUILD_ID);
		DL_UNLOAD(handle);
		return FAILURE;
	}
	module_entry->type = type;
	module_entry->module_number = zend_next_free_module();
	module_entry->handle = handle;

	if ((module_entry = zend_register_module_ex(module_entry TSRMLS_CC)) == NULL) {
		DL_UNLOAD(handle);
		return FAILURE;
	}

	if ((type == MODULE_TEMPORARY || start_now) && zend_startup_module_ex(module_entry TSRMLS_CC) == FAILURE) {
		DL_UNLOAD(handle);
		return FAILURE;
	}

	if ((type == MODULE_TEMPORARY || start_now) && module_entry->request_startup_func) {
		if (module_entry->request_startup_func(type, module_entry->module_number TSRMLS_CC) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, error_type, "Unable to initialize module '%s'", module_entry->name);
			DL_UNLOAD(handle);
			return FAILURE;
		}
	}
	return SUCCESS;
}
/* }}} */

/* {{{ php_dl
 */
PHPAPI void php_dl(char *file, int type, zval *return_value, int start_now TSRMLS_DC)
{
	/* Load extension */
	if (php_load_extension(file, type, start_now TSRMLS_CC) == FAILURE) {
		RETVAL_FALSE;
	} else {
		RETVAL_TRUE;
	}
}
/* }}} */

PHP_MINFO_FUNCTION(dl)
{
	php_info_print_table_row(2, "Dynamic Library Support", "enabled");
}

#else

PHPAPI void php_dl(char *file, int type, zval *return_value, int start_now TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot dynamically load %s - dynamic modules are not supported", file);
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
