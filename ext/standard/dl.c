/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Brian Schaffner <brian@tool.net>                            |
   |          Shane Caraveo <shane@caraveo.com>                           |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "dl.h"
#include "php_globals.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "Zend/zend_extensions.h"

#include "SAPI.h"

#ifdef HAVE_LIBDL
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef PHP_WIN32
#include "win32/param.h"
#include "win32/winutil.h"
#define GET_DL_ERROR()	php_win_err()
#else
#include <sys/param.h>
#define GET_DL_ERROR()	DL_ERROR()
#endif
#endif /* defined(HAVE_LIBDL) */

/* {{{ Load a PHP extension at runtime */
PHPAPI PHP_FUNCTION(dl)
{
	char *filename;
	size_t filename_len;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(filename, filename_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!PG(enable_dl)) {
		php_error_docref(NULL, E_WARNING, "Dynamically loaded extensions aren't enabled");
		RETURN_FALSE;
	}

	if (filename_len >= MAXPATHLEN) {
		php_error_docref(NULL, E_WARNING, "Filename exceeds the maximum allowed length of %d characters", MAXPATHLEN);
		RETURN_FALSE;
	}

#if ZEND_RC_DEBUG
	bool orig_rc_debug = zend_rc_debug;
	/* FIXME: Loading extensions during the request breaks some invariants. In
	 * particular, it will create persistent interned strings, which is not
	 * allowed at this stage. */
	zend_rc_debug = false;
#endif

	php_dl(filename, MODULE_TEMPORARY, return_value, 0);
	if (Z_TYPE_P(return_value) == IS_TRUE) {
		EG(full_tables_cleanup) = 1;
	}

#if ZEND_RC_DEBUG
	zend_rc_debug = orig_rc_debug;
#endif
}
/* }}} */

#ifdef HAVE_LIBDL

/* {{{ php_load_shlib */
PHPAPI void *php_load_shlib(const char *path, char **errp)
{
	void *handle;
	char *err;

	handle = DL_LOAD(path);
	if (!handle) {
		err = GET_DL_ERROR();
#ifdef PHP_WIN32
		if (err && (*err)) {
			size_t i = strlen(err);
			(*errp)=estrdup(err);
			php_win32_error_msg_free(err);
			while (i > 0 && isspace((*errp)[i-1])) { (*errp)[i-1] = '\0'; i--; }
		} else {
			(*errp) = estrdup("<No message>");
		}
#else
		(*errp) = estrdup(err);
		GET_DL_ERROR(); /* free the buffer storing the error */
#endif
	}
	return handle;
}
/* }}} */

static zend_result php_register_module_entry(
	zend_module_entry *(*get_module)(void), void *handle, int type, int start_now, int error_type, zend_module_entry **registered_module, bool silent)
{
	zend_module_entry *module_entry = get_module();

	if (registered_module) {
		*registered_module = NULL;
	}

	if (zend_hash_str_exists(&module_registry, module_entry->name, strlen(module_entry->name))) {
		if (!silent) {
			zend_error(E_CORE_WARNING, "Module \"%s\" is already loaded", module_entry->name);
		}
		return FAILURE;
	}
	if (module_entry->zend_api != ZEND_MODULE_API_NO) {
		if (!silent) {
			php_error_docref(NULL, error_type,
					"%s: Unable to initialize module\n"
					"Module compiled with module API=%d\n"
					"PHP    compiled with module API=%d\n"
					"These options need to match\n",
					module_entry->name, module_entry->zend_api, ZEND_MODULE_API_NO);
		}
		return FAILURE;
	}
	if (strcmp(module_entry->build_id, ZEND_MODULE_BUILD_ID)) {
		if (!silent) {
			php_error_docref(NULL, error_type,
					"%s: Unable to initialize module\n"
					"Module compiled with build ID=%s\n"
					"PHP    compiled with build ID=%s\n"
					"These options need to match\n",
					module_entry->name, module_entry->build_id, ZEND_MODULE_BUILD_ID);
		}
		return FAILURE;
	}

	if ((module_entry = zend_register_module_ex(module_entry, type)) == NULL) {
		return FAILURE;
	}

	module_entry->handle = handle;

	if ((type == MODULE_TEMPORARY || start_now) && zend_startup_module_ex(module_entry) == FAILURE) {
		return FAILURE;
	}

	if ((type == MODULE_TEMPORARY || start_now) && module_entry->request_startup_func) {
		if (module_entry->request_startup_func(type, module_entry->module_number) == FAILURE) {
			if (!silent) {
				php_error_docref(NULL, error_type, "Unable to initialize module '%s'", module_entry->name);
			}
			return FAILURE;
		}
	}

	if (registered_module) {
		*registered_module = module_entry;
	}
	return SUCCESS;
}

typedef zend_module_entry *(*get_module_func_t)(void);

static get_module_func_t fetch_get_module_sym(DL_HANDLE handle)
{
	get_module_func_t get_module;

	get_module = (get_module_func_t) DL_FETCH_SYMBOL(handle, "get_module");
	/* Some OS prepend _ to symbol names while their dynamic linker
	 * does not do that automatically. Thus we check manually for
	 * _get_module. */
	if (!get_module) {
		get_module = (get_module_func_t) DL_FETCH_SYMBOL(handle, "_get_module");
	}

	return get_module;
}

static zend_extension *fetch_zend_extension_entry_sym(DL_HANDLE handle)
{
	zend_extension *zend_extension_entry;

	zend_extension_entry = (zend_extension *) DL_FETCH_SYMBOL(handle, "zend_extension_entry");
	if (!zend_extension_entry) {
		zend_extension_entry = (zend_extension *) DL_FETCH_SYMBOL(handle, "_zend_extension_entry");
	}

	return zend_extension_entry;
}

/* {{{ php_load_extension_ex */
PHPAPI int php_load_extension_ex(const char *filename, int type, int start_now, bool primary_as_zend_extension, bool try_additional)
{
	void *handle;
	char *libpath;
	zend_module_entry *module_entry = NULL;
	get_module_func_t get_module;
	zend_extension *zend_extension_entry = NULL;
	int error_type, slash_suffix = 0;
	char *extension_dir;
	char *err1, *err2;

	if (type == MODULE_PERSISTENT) {
		extension_dir = INI_STR("extension_dir");
	} else {
		if (primary_as_zend_extension) {
			php_error_docref(NULL, E_WARNING, "Zend extensions can only be loaded persistently");
			return FAILURE;
		}
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
			php_error_docref(NULL, E_WARNING, "Temporary module name should contain only filename");
			return FAILURE;
		}
		libpath = estrdup(filename);
	} else if (extension_dir && extension_dir[0]) {
		slash_suffix = IS_SLASH(extension_dir[strlen(extension_dir)-1]);
		/* Try as filename first */
		if (slash_suffix) {
			spprintf(&libpath, 0, "%s%s", extension_dir, filename); /* SAFE */
		} else {
			spprintf(&libpath, 0, "%s%c%s", extension_dir, DEFAULT_SLASH, filename); /* SAFE */
		}
	} else {
		return FAILURE; /* Not full path given or extension_dir is not set */
	}

	handle = php_load_shlib(libpath, &err1);
	if (!handle) {
		/* Now, consider 'filename' as extension name and build file name */
		char *orig_libpath = libpath;

		if (slash_suffix) {
			spprintf(&libpath, 0, "%s" PHP_SHLIB_EXT_PREFIX "%s." PHP_SHLIB_SUFFIX, extension_dir, filename); /* SAFE */
		} else {
			spprintf(&libpath, 0, "%s%c" PHP_SHLIB_EXT_PREFIX "%s." PHP_SHLIB_SUFFIX, extension_dir, DEFAULT_SLASH, filename); /* SAFE */
		}

		handle = php_load_shlib(libpath, &err2);
		if (!handle) {
			php_error_docref(NULL, error_type, "Unable to load dynamic library '%s' (tried: %s (%s), %s (%s))",
				filename, orig_libpath, err1, libpath, err2);
			efree(orig_libpath);
			efree(err1);
			efree(libpath);
			efree(err2);
			return FAILURE;
		}
		efree(orig_libpath);
		efree(err1);
	}

#ifdef PHP_WIN32
	if (!php_win32_image_compatible(handle, &err1)) {
			php_error_docref(NULL, error_type, "%s", err1);
			efree(err1);
			DL_UNLOAD(handle);
			efree(libpath);
			return FAILURE;
	}
#endif

	if (primary_as_zend_extension) {
		ZEND_ASSERT(type == MODULE_PERSISTENT);
		zend_extension_entry = fetch_zend_extension_entry_sym(handle);

		if (!zend_extension_entry) {
			DL_UNLOAD(handle);
			php_error_docref(NULL, error_type, "Invalid library (maybe not a PHP library) '%s'", filename);
			efree(libpath);
			return FAILURE;
		}

		if (zend_load_extension_handle_ex(handle, libpath, false /* unload_on_failure */, false /* silent */) != SUCCESS) {
			zend_extension_entry = NULL;
		}
		if (try_additional && zend_extension_entry != NULL) {
			get_module = fetch_get_module_sym(handle);
			if (get_module
					&& php_register_module_entry(get_module, handle, type, start_now, error_type, &module_entry, true) == SUCCESS) {
				/* module_entry is set by php_register_module_entry() */
			}
		}
	} else {
		get_module = fetch_get_module_sym(handle);

		if (!get_module) {

			zend_extension_entry = fetch_zend_extension_entry_sym(handle);
			DL_UNLOAD(handle);
			const char *reason = (zend_extension_entry && type != MODULE_PERSISTENT)
				? "appears to be a Zend Extension and cannot be loaded via dl()"
				: "maybe not a PHP library";
			php_error_docref(NULL, error_type, "Invalid library (%s) '%s'", reason, filename);
			efree(libpath);
			return FAILURE;
		}

		if (php_register_module_entry(get_module, handle, type, start_now, error_type, &module_entry, false) == SUCCESS) {
			/* module_entry is set by php_register_module_entry() */
		}

		if (try_additional && module_entry != NULL && type == MODULE_PERSISTENT) {
			zend_extension_entry = fetch_zend_extension_entry_sym(handle);
			if (zend_extension_entry
					&& zend_load_extension_handle_ex(handle, libpath, false /* unload_on_failure */, true /* silent */) != SUCCESS) {
				zend_extension_entry = NULL;
			}
		}
	}

	if (module_entry != NULL && zend_extension_entry != NULL) {
		/* In dual mode, let Zend extension own this handle. */
		module_entry->handle = NULL;
	}

	efree(libpath);

	if (module_entry != NULL || zend_extension_entry != NULL) {
		return SUCCESS;
	}

	DL_UNLOAD(handle);
	return FAILURE;
}
/* }}} */

/* {{{ php_load_extension */
PHPAPI int php_load_extension(const char *filename, int type, int start_now)
{
	return php_load_extension_ex(filename, type, start_now, false, false);
}
/* }}} */

#else

static void php_dl_error(const char *filename)
{
	php_error_docref(NULL, E_WARNING, "Cannot dynamically load %s - dynamic modules are not supported", filename);
}

PHPAPI void *php_load_shlib(const char *path, char **errp)
{
	php_dl_error(path);
	(*errp) = estrdup("No DL support");
	return NULL;
}

PHPAPI int php_load_extension(const char *filename, int type, int start_now)
{
	return php_load_extension_ex(filename, type, start_now, false, false);
}

PHPAPI int php_load_extension_ex(const char *filename, int type, int start_now, bool primary_as_zend_extension, bool try_additional)
{
	(void) primary_as_zend_extension;
	(void) try_additional;
	php_dl_error(filename);

	return FAILURE;
}

#endif

/* {{{ php_dl */
PHPAPI void php_dl(const char *file, int type, zval *return_value, int start_now)
{
	/* Load extension */
	if (php_load_extension(file, type, start_now) == FAILURE) {
		RETVAL_FALSE;
	} else {
		RETVAL_TRUE;
	}
}
/* }}} */

PHP_MINFO_FUNCTION(dl)
{
#if defined(HAVE_LIBDL)
#define PHP_DL_SUPPORT_STATUS "enabled"
#else
#define PHP_DL_SUPPORT_STATUS "unavailable"
#endif
	php_info_print_table_row(2, "Dynamic Library Support", PHP_DL_SUPPORT_STATUS);
}
