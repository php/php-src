/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#include <time.h>

#include "php.h"
#include "ZendAccelerator.h"
#include "zend_API.h"
#include "zend_shared_alloc.h"
#include "zend_accelerator_blacklist.h"
#include "php_ini.h"
#include "SAPI.h"
#include "zend_virtual_cwd.h"
#include "ext/standard/info.h"
#include "ext/standard/php_filestat.h"

#define STRING_NOT_NULL(s) (NULL == (s)?"":s)
#define MIN_ACCEL_FILES 200
#define MAX_ACCEL_FILES 1000000
#define TOKENTOSTR(X) #X

static void (*orig_file_exists)(INTERNAL_FUNCTION_PARAMETERS) = NULL;
static void (*orig_is_file)(INTERNAL_FUNCTION_PARAMETERS) = NULL;
static void (*orig_is_readable)(INTERNAL_FUNCTION_PARAMETERS) = NULL;

ZEND_BEGIN_ARG_INFO(arginfo_opcache_none, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_opcache_get_status, 0, 0, 0)
	ZEND_ARG_INFO(0, fetch_scripts)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_opcache_compile_file, 0, 0, 1)
	ZEND_ARG_INFO(0, file)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_opcache_invalidate, 0, 0, 1)
	ZEND_ARG_INFO(0, script)
	ZEND_ARG_INFO(0, force)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_opcache_is_script_cached, 0, 0, 1)
	ZEND_ARG_INFO(0, script)
ZEND_END_ARG_INFO()

/* User functions */
static ZEND_FUNCTION(opcache_reset);
static ZEND_FUNCTION(opcache_invalidate);
static ZEND_FUNCTION(opcache_is_script_cached);

/* Private functions */
static ZEND_FUNCTION(opcache_get_status);
static ZEND_FUNCTION(opcache_compile_file);
static ZEND_FUNCTION(opcache_get_configuration);

static zend_function_entry accel_functions[] = {
	/* User functions */
	ZEND_FE(opcache_reset,					arginfo_opcache_none)
	ZEND_FE(opcache_invalidate,				arginfo_opcache_invalidate)
	ZEND_FE(opcache_compile_file,			arginfo_opcache_compile_file)
	ZEND_FE(opcache_is_script_cached,		arginfo_opcache_is_script_cached)
	/* Private functions */
	ZEND_FE(opcache_get_configuration,		arginfo_opcache_none)
	ZEND_FE(opcache_get_status,				arginfo_opcache_get_status)
	{ NULL, NULL, NULL, 0, 0 }
};

static int validate_api_restriction(void)
{
	if (ZCG(accel_directives).restrict_api && *ZCG(accel_directives).restrict_api) {
		int len = strlen(ZCG(accel_directives).restrict_api);

		if (!SG(request_info).path_translated ||
		    strlen(SG(request_info).path_translated) < len ||
		    memcmp(SG(request_info).path_translated, ZCG(accel_directives).restrict_api, len) != 0) {
			zend_error(E_WARNING, ACCELERATOR_PRODUCT_NAME " API is restricted by \"restrict_api\" configuration directive");
			return 0;
		}
	}
	return 1;
}

static ZEND_INI_MH(OnUpdateMemoryConsumption)
{
	zend_long *p;
	zend_long memsize;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	/* keep the compiler happy */
	(void)entry; (void)mh_arg2; (void)mh_arg3; (void)stage;

	p = (zend_long *) (base + (size_t)mh_arg1);
	memsize = atoi(ZSTR_VAL(new_value));
	/* sanity check we must use at least 8 MB */
	if (memsize < 8) {
		const char *new_new_value = "8";
		zend_ini_entry *ini_entry;

		memsize = 8;
		zend_accel_error(ACCEL_LOG_WARNING, "opcache.memory_consumption is set below the required 8MB.\n");
		zend_accel_error(ACCEL_LOG_WARNING, ACCELERATOR_PRODUCT_NAME " will use the minimal 8MB configuration.\n");

		if ((ini_entry = zend_hash_str_find_ptr(EG(ini_directives),
					"opcache.memory_consumption",
					sizeof("opcache.memory_consumption")-1)) == NULL) {
			return FAILURE;
		}

		ini_entry->value = zend_string_init(new_new_value, 1, 1);
	}
	*p = memsize * (1024 * 1024);
	return SUCCESS;
}

static ZEND_INI_MH(OnUpdateMaxAcceleratedFiles)
{
	zend_long *p;
	zend_long size;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	/* keep the compiler happy */
	(void)entry; (void)mh_arg2; (void)mh_arg3; (void)stage;

	p = (zend_long *) (base + (size_t)mh_arg1);
	size = atoi(ZSTR_VAL(new_value));
	/* sanity check we must use a value between MIN_ACCEL_FILES and MAX_ACCEL_FILES */

	if (size < MIN_ACCEL_FILES || size > MAX_ACCEL_FILES) {
		const char *new_new_value;
		zend_ini_entry *ini_entry;

		if (size < MIN_ACCEL_FILES) {
			size = MIN_ACCEL_FILES;
			new_new_value = TOKENTOSTR(MIN_ACCEL_FILES);
			zend_accel_error(ACCEL_LOG_WARNING, "opcache.max_accelerated_files is set below the required minimum (%d).\n", MIN_ACCEL_FILES);
			zend_accel_error(ACCEL_LOG_WARNING, ACCELERATOR_PRODUCT_NAME " will use the minimal configuration.\n");
		}
		if (size > MAX_ACCEL_FILES) {
			size = MAX_ACCEL_FILES;
			new_new_value = TOKENTOSTR(MAX_ACCEL_FILES);
			zend_accel_error(ACCEL_LOG_WARNING, "opcache.max_accelerated_files is set above the limit (%d).\n", MAX_ACCEL_FILES);
			zend_accel_error(ACCEL_LOG_WARNING, ACCELERATOR_PRODUCT_NAME " will use the maximal configuration.\n");
		}
		if ((ini_entry = zend_hash_str_find_ptr(EG(ini_directives),
					"opcache.max_accelerated_files",
					sizeof("opcache.max_accelerated_files")-1)) == NULL) {
			return FAILURE;
		}
		ini_entry->value = zend_string_init(new_new_value, strlen(new_new_value), 1);
	}
	*p = size;
	return SUCCESS;
}

static ZEND_INI_MH(OnUpdateMaxWastedPercentage)
{
	double *p;
	zend_long percentage;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	/* keep the compiler happy */
	(void)entry; (void)mh_arg2; (void)mh_arg3; (void)stage;

	p = (double *) (base + (size_t)mh_arg1);
	percentage = atoi(ZSTR_VAL(new_value));

	if (percentage <= 0 || percentage > 50) {
		const char *new_new_value = "5";
		zend_ini_entry *ini_entry;

		percentage = 5;
		zend_accel_error(ACCEL_LOG_WARNING, "opcache.max_wasted_percentage must be set between 1 and 50.\n");
		zend_accel_error(ACCEL_LOG_WARNING, ACCELERATOR_PRODUCT_NAME " will use 5%.\n");
		if ((ini_entry = zend_hash_str_find_ptr(EG(ini_directives),
					"opcache.max_wasted_percentage",
					sizeof("opcache.max_wasted_percentage")-1)) == NULL) {
			return FAILURE;
		}
		ini_entry->value = zend_string_init(new_new_value, strlen(new_new_value), 1);
	}
	*p = (double)percentage / 100.0;
	return SUCCESS;
}

static ZEND_INI_MH(OnEnable)
{
	if (stage == ZEND_INI_STAGE_STARTUP ||
	    stage == ZEND_INI_STAGE_SHUTDOWN ||
	    stage == ZEND_INI_STAGE_DEACTIVATE) {
		return OnUpdateBool(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	} else {
		/* It may be only temporary disabled */
		zend_bool *p;
#ifndef ZTS
		char *base = (char *) mh_arg2;
#else
		char *base = (char *) ts_resource(*((int *) mh_arg2));
#endif

		p = (zend_bool *) (base+(size_t) mh_arg1);
		if ((ZSTR_LEN(new_value) == 2 && strcasecmp("on", ZSTR_VAL(new_value)) == 0) ||
		    (ZSTR_LEN(new_value) == 3 && strcasecmp("yes", ZSTR_VAL(new_value)) == 0) ||
		    (ZSTR_LEN(new_value) == 4 && strcasecmp("true", ZSTR_VAL(new_value)) == 0) ||
			atoi(ZSTR_VAL(new_value)) != 0) {
			zend_error(E_WARNING, ACCELERATOR_PRODUCT_NAME " can't be temporary enabled (it may be only disabled till the end of request)");
			return FAILURE;
		} else {
			*p = 0;
			return SUCCESS;
		}
	}
}

#ifdef HAVE_OPCACHE_FILE_CACHE
static ZEND_INI_MH(OnUpdateFileCache)
{
	if (new_value) {
		if (!ZSTR_LEN(new_value)) {
			new_value = NULL;
		} else {
			zend_stat_t buf;

		    if (!IS_ABSOLUTE_PATH(ZSTR_VAL(new_value), ZSTR_LEN(new_value)) ||
			    zend_stat(ZSTR_VAL(new_value), &buf) != 0 ||
			    !S_ISDIR(buf.st_mode) ||
#ifndef ZEND_WIN32
				access(ZSTR_VAL(new_value), R_OK | W_OK | X_OK) != 0) {
#else
				_access(ZSTR_VAL(new_value), 06) != 0) {
#endif
				zend_accel_error(ACCEL_LOG_WARNING, "opcache.file_cache must be a full path of accessable directory.\n");
				new_value = NULL;
			}
		}
	}
	OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	return SUCCESS;
}
#endif

ZEND_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("opcache.enable"             , "1", PHP_INI_ALL,    OnEnable,     enabled                             , zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.use_cwd"            , "1", PHP_INI_SYSTEM, OnUpdateBool, accel_directives.use_cwd            , zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.validate_timestamps", "1", PHP_INI_ALL   , OnUpdateBool, accel_directives.validate_timestamps, zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.inherited_hack"     , "1", PHP_INI_SYSTEM, OnUpdateBool, accel_directives.inherited_hack     , zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.dups_fix"           , "0", PHP_INI_ALL   , OnUpdateBool, accel_directives.ignore_dups        , zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.revalidate_path"    , "0", PHP_INI_ALL   , OnUpdateBool, accel_directives.revalidate_path    , zend_accel_globals, accel_globals)

	STD_PHP_INI_ENTRY("opcache.log_verbosity_level"   , "1"   , PHP_INI_SYSTEM, OnUpdateLong, accel_directives.log_verbosity_level,       zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.memory_consumption"    , "64"  , PHP_INI_SYSTEM, OnUpdateMemoryConsumption,    accel_directives.memory_consumption,        zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.interned_strings_buffer", "4"  , PHP_INI_SYSTEM, OnUpdateLong,                 accel_directives.interned_strings_buffer,   zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.max_accelerated_files" , "2000", PHP_INI_SYSTEM, OnUpdateMaxAcceleratedFiles,	 accel_directives.max_accelerated_files,     zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.max_wasted_percentage" , "5"   , PHP_INI_SYSTEM, OnUpdateMaxWastedPercentage,	 accel_directives.max_wasted_percentage,     zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.consistency_checks"    , "0"   , PHP_INI_ALL   , OnUpdateLong,	             accel_directives.consistency_checks,        zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.force_restart_timeout" , "180" , PHP_INI_SYSTEM, OnUpdateLong,	             accel_directives.force_restart_timeout,     zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.revalidate_freq"       , "2"   , PHP_INI_ALL   , OnUpdateLong,	             accel_directives.revalidate_freq,           zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.file_update_protection", "2"   , PHP_INI_ALL   , OnUpdateLong,                accel_directives.file_update_protection,    zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.preferred_memory_model", ""    , PHP_INI_SYSTEM, OnUpdateStringUnempty,       accel_directives.memory_model,              zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.blacklist_filename"    , ""    , PHP_INI_SYSTEM, OnUpdateString,	             accel_directives.user_blacklist_filename,   zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.max_file_size"         , "0"   , PHP_INI_SYSTEM, OnUpdateLong,	             accel_directives.max_file_size,             zend_accel_globals, accel_globals)

	STD_PHP_INI_ENTRY("opcache.protect_memory"        , "0"  , PHP_INI_SYSTEM, OnUpdateBool,                  accel_directives.protect_memory,            zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.save_comments"         , "1"  , PHP_INI_SYSTEM, OnUpdateBool,                  accel_directives.save_comments,             zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.fast_shutdown"         , "0"  , PHP_INI_SYSTEM, OnUpdateBool,                  accel_directives.fast_shutdown,             zend_accel_globals, accel_globals)

	STD_PHP_INI_ENTRY("opcache.optimization_level"    , DEFAULT_OPTIMIZATION_LEVEL , PHP_INI_SYSTEM, OnUpdateLong, accel_directives.optimization_level,   zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.enable_file_override"	, "0"   , PHP_INI_SYSTEM, OnUpdateBool,              accel_directives.file_override_enabled,     zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.enable_cli"             , "0"   , PHP_INI_SYSTEM, OnUpdateBool,              accel_directives.enable_cli,                zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.error_log"                , ""    , PHP_INI_SYSTEM, OnUpdateString,	         accel_directives.error_log,                 zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.restrict_api"             , ""    , PHP_INI_SYSTEM, OnUpdateString,	         accel_directives.restrict_api,              zend_accel_globals, accel_globals)

#ifndef ZEND_WIN32
	STD_PHP_INI_ENTRY("opcache.lockfile_path"             , "/tmp"    , PHP_INI_SYSTEM, OnUpdateString,           accel_directives.lockfile_path,              zend_accel_globals, accel_globals)
#else
	STD_PHP_INI_ENTRY("opcache.mmap_base", NULL, PHP_INI_SYSTEM,	OnUpdateString,	                             accel_directives.mmap_base,                 zend_accel_globals, accel_globals)
#endif

#ifdef HAVE_OPCACHE_FILE_CACHE
	STD_PHP_INI_ENTRY("opcache.file_cache"                    , NULL  , PHP_INI_SYSTEM, OnUpdateFileCache, accel_directives.file_cache,                    zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.file_cache_only"               , "0"   , PHP_INI_SYSTEM, OnUpdateBool,	   accel_directives.file_cache_only,               zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.file_cache_consistency_checks" , "1"   , PHP_INI_SYSTEM, OnUpdateBool,	   accel_directives.file_cache_consistency_checks, zend_accel_globals, accel_globals)
#endif
#if ENABLE_FILE_CACHE_FALLBACK
	STD_PHP_INI_ENTRY("opcache.file_cache_fallback"           , "1"   , PHP_INI_SYSTEM, OnUpdateBool,	   accel_directives.file_cache_fallback,           zend_accel_globals, accel_globals)
#endif
#ifdef HAVE_HUGE_CODE_PAGES
	STD_PHP_INI_BOOLEAN("opcache.huge_code_pages"             , "0"   , PHP_INI_SYSTEM, OnUpdateBool,      accel_directives.huge_code_pages,               zend_accel_globals, accel_globals)
#endif
ZEND_INI_END()

static int filename_is_in_cache(zend_string *filename)
{
	char *key;
	int key_length;

	key = accel_make_persistent_key(ZSTR_VAL(filename), ZSTR_LEN(filename), &key_length);
	if (key != NULL) {
		zend_persistent_script *persistent_script = zend_accel_hash_str_find(&ZCSG(hash), key, key_length);
		if (persistent_script && !persistent_script->corrupted) {
			zend_file_handle handle = {{0}, NULL, NULL, 0, 0};

			handle.filename = ZSTR_VAL(filename);
			handle.type = ZEND_HANDLE_FILENAME;

			if (ZCG(accel_directives).validate_timestamps) {
				return validate_timestamp_and_record(persistent_script, &handle) == SUCCESS;
			}

			return 1;
		}
	}

	return 0;
}

static int accel_file_in_cache(INTERNAL_FUNCTION_PARAMETERS)
{
	zval zfilename;

	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_array_ex(1, &zfilename) == FAILURE ||
	    Z_TYPE(zfilename) != IS_STRING ||
	    Z_STRLEN(zfilename) == 0) {
		return 0;
	}
	return filename_is_in_cache(Z_STR(zfilename));
}

static void accel_file_exists(INTERNAL_FUNCTION_PARAMETERS)
{
	if (accel_file_in_cache(INTERNAL_FUNCTION_PARAM_PASSTHRU)) {
		RETURN_TRUE;
	} else {
		orig_file_exists(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	}
}

static void accel_is_file(INTERNAL_FUNCTION_PARAMETERS)
{
	if (accel_file_in_cache(INTERNAL_FUNCTION_PARAM_PASSTHRU)) {
		RETURN_TRUE;
	} else {
		orig_is_file(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	}
}

static void accel_is_readable(INTERNAL_FUNCTION_PARAMETERS)
{
	if (accel_file_in_cache(INTERNAL_FUNCTION_PARAM_PASSTHRU)) {
		RETURN_TRUE;
	} else {
		orig_is_readable(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	}
}

static ZEND_MINIT_FUNCTION(zend_accelerator)
{
	(void)type; /* keep the compiler happy */

	REGISTER_INI_ENTRIES();

	return SUCCESS;
}

void zend_accel_override_file_functions(void)
{
	zend_function *old_function;
	if (ZCG(enabled) && accel_startup_ok && ZCG(accel_directives).file_override_enabled) {
#ifdef HAVE_OPCACHE_FILE_CACHE
		if (ZCG(accel_directives).file_cache_only) {
			zend_accel_error(ACCEL_LOG_WARNING, "file_override_enabled has no effect when file_cache_only is set");
			return;
		}
#endif
		/* override file_exists */
		if ((old_function = zend_hash_str_find_ptr(CG(function_table), "file_exists", sizeof("file_exists")-1)) != NULL) {
			orig_file_exists = old_function->internal_function.handler;
			old_function->internal_function.handler = accel_file_exists;
		}
		if ((old_function = zend_hash_str_find_ptr(CG(function_table), "is_file", sizeof("is_file")-1)) != NULL) {
			orig_is_file = old_function->internal_function.handler;
			old_function->internal_function.handler = accel_is_file;
		}
		if ((old_function = zend_hash_str_find_ptr(CG(function_table), "is_readable", sizeof("is_readable")-1)) != NULL) {
			orig_is_readable = old_function->internal_function.handler;
			old_function->internal_function.handler = accel_is_readable;
		}
	}
}

static ZEND_MSHUTDOWN_FUNCTION(zend_accelerator)
{
	(void)type; /* keep the compiler happy */

	UNREGISTER_INI_ENTRIES();
	accel_shutdown();
	return SUCCESS;
}

void zend_accel_info(ZEND_MODULE_INFO_FUNC_ARGS)
{
	php_info_print_table_start();

	if (ZCG(enabled) && accel_startup_ok &&
#ifdef HAVE_OPCACHE_FILE_CACHE
		((ZCG(counted) || ZCSG(accelerator_enabled)) || ZCG(accel_directives).file_cache_only)
#else
		(ZCG(counted) || ZCSG(accelerator_enabled))
#endif
	) {
		php_info_print_table_row(2, "Opcode Caching", "Up and Running");
	} else {
		php_info_print_table_row(2, "Opcode Caching", "Disabled");
	}
	if (ZCG(enabled) && accel_startup_ok && ZCG(accel_directives).optimization_level) {
		php_info_print_table_row(2, "Optimization", "Enabled");
	} else {
		php_info_print_table_row(2, "Optimization", "Disabled");
	}
#ifdef HAVE_OPCACHE_FILE_CACHE
	if (!ZCG(accel_directives).file_cache_only) {
		php_info_print_table_row(2, "SHM Cache", "Enabled");
	} else {
		php_info_print_table_row(2, "SHM Cache", "Disabled");
	}
	if (ZCG(accel_directives).file_cache) {
		php_info_print_table_row(2, "File Cache", "Enabled");
	} else {
		php_info_print_table_row(2, "File Cache", "Disabled");
	}
	if (ZCG(accel_directives).file_cache_only) {
		if (!accel_startup_ok || zps_api_failure_reason) {
			php_info_print_table_row(2, "Startup Failed", zps_api_failure_reason);
		} else {
			php_info_print_table_row(2, "Startup", "OK");
		}
	} else
#endif
	if (ZCG(enabled)) {
		if (!accel_startup_ok || zps_api_failure_reason) {
			php_info_print_table_row(2, "Startup Failed", zps_api_failure_reason);
		} else {
			char buf[32];
			php_info_print_table_row(2, "Startup", "OK");
			php_info_print_table_row(2, "Shared memory model", zend_accel_get_shared_model());
			snprintf(buf, sizeof(buf), "%pd", (zend_ulong)ZCSG(hits));
			php_info_print_table_row(2, "Cache hits", buf);
			snprintf(buf, sizeof(buf), "%pd", ZSMMG(memory_exhausted)?ZCSG(misses):ZCSG(misses)-ZCSG(blacklist_misses));
			php_info_print_table_row(2, "Cache misses", buf);
			snprintf(buf, sizeof(buf), ZEND_LONG_FMT, ZCG(accel_directives).memory_consumption-zend_shared_alloc_get_free_memory()-ZSMMG(wasted_shared_memory));
			php_info_print_table_row(2, "Used memory", buf);
			snprintf(buf, sizeof(buf), "%pd", zend_shared_alloc_get_free_memory());
			php_info_print_table_row(2, "Free memory", buf);
			snprintf(buf, sizeof(buf), "%pd", ZSMMG(wasted_shared_memory));
			php_info_print_table_row(2, "Wasted memory", buf);
			if (ZCSG(interned_strings_start) && ZCSG(interned_strings_end) && ZCSG(interned_strings_top)) {
				snprintf(buf, sizeof(buf), "%pd", ZCSG(interned_strings_top) - ZCSG(interned_strings_start));
				php_info_print_table_row(2, "Interned Strings Used memory", buf);
				snprintf(buf, sizeof(buf), "%pd", ZCSG(interned_strings_end) - ZCSG(interned_strings_top));
				php_info_print_table_row(2, "Interned Strings Free memory", buf);
			}
			snprintf(buf, sizeof(buf), "%ld", ZCSG(hash).num_direct_entries);
			php_info_print_table_row(2, "Cached scripts", buf);
			snprintf(buf, sizeof(buf), "%ld", ZCSG(hash).num_entries);
			php_info_print_table_row(2, "Cached keys", buf);
			snprintf(buf, sizeof(buf), "%pd", ZCSG(hash).max_num_entries);
			php_info_print_table_row(2, "Max keys", buf);
			snprintf(buf, sizeof(buf), "%pd", ZCSG(oom_restarts));
			php_info_print_table_row(2, "OOM restarts", buf);
			snprintf(buf, sizeof(buf), "%pd", ZCSG(hash_restarts));
			php_info_print_table_row(2, "Hash keys restarts", buf);
			snprintf(buf, sizeof(buf), "%pd", ZCSG(manual_restarts));
			php_info_print_table_row(2, "Manual restarts", buf);
		}
	}

	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}

static zend_module_entry accel_module_entry = {
	STANDARD_MODULE_HEADER,
	ACCELERATOR_PRODUCT_NAME,
	accel_functions,
	ZEND_MINIT(zend_accelerator),
	ZEND_MSHUTDOWN(zend_accelerator),
	NULL,
	NULL,
	zend_accel_info,
	PHP_VERSION,
	NO_MODULE_GLOBALS,
	accel_post_deactivate,
	STANDARD_MODULE_PROPERTIES_EX
};

int start_accel_module(void)
{
	return zend_startup_module(&accel_module_entry);
}

/* {{{ proto array accelerator_get_scripts()
   Get the scripts which are accelerated by ZendAccelerator */
static int accelerator_get_scripts(zval *return_value)
{
	uint i;
	zval persistent_script_report;
	zend_accel_hash_entry *cache_entry;
	struct tm *ta;
	struct timeval exec_time;
	struct timeval fetch_time;

	if (!ZCG(enabled) || !accel_startup_ok || !ZCSG(accelerator_enabled) || accelerator_shm_read_lock() != SUCCESS) {
		return 0;
	}

	array_init(return_value);
	for (i = 0; i<ZCSG(hash).max_num_entries; i++) {
		for (cache_entry = ZCSG(hash).hash_table[i]; cache_entry; cache_entry = cache_entry->next) {
			zend_persistent_script *script;
			char *str;
			size_t len;

			if (cache_entry->indirect) continue;

			script = (zend_persistent_script *)cache_entry->data;

			array_init(&persistent_script_report);
			add_assoc_str(&persistent_script_report, "full_path", zend_string_dup(script->full_path, 0));
			add_assoc_long(&persistent_script_report, "hits", (zend_long)script->dynamic_members.hits);
			add_assoc_long(&persistent_script_report, "memory_consumption", script->dynamic_members.memory_consumption);
			ta = localtime(&script->dynamic_members.last_used);
			str = asctime(ta);
			len = strlen(str);
			if (len > 0 && str[len - 1] == '\n') len--;
			add_assoc_stringl(&persistent_script_report, "last_used", str, len);
			add_assoc_long(&persistent_script_report, "last_used_timestamp", script->dynamic_members.last_used);
			if (ZCG(accel_directives).validate_timestamps) {
				add_assoc_long(&persistent_script_report, "timestamp", (zend_long)script->timestamp);
			}
			timerclear(&exec_time);
			timerclear(&fetch_time);

			zend_hash_str_update(Z_ARRVAL_P(return_value), cache_entry->key, cache_entry->key_length, &persistent_script_report);
		}
	}
	accelerator_shm_read_unlock();

	return 1;
}

/* {{{ proto array accelerator_get_status([bool fetch_scripts])
   Obtain statistics information regarding code acceleration */
static ZEND_FUNCTION(opcache_get_status)
{
	zend_long reqs;
	zval memory_usage, statistics, scripts;
	zend_bool fetch_scripts = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &fetch_scripts) == FAILURE) {
		return;
	}

	if (!validate_api_restriction()) {
		RETURN_FALSE;
	}

	if (!accel_startup_ok) {
		RETURN_FALSE;
	}

	array_init(return_value);

	/* Trivia */
	add_assoc_bool(return_value, "opcache_enabled", ZCG(enabled) && (ZCG(counted) || ZCSG(accelerator_enabled)));

#ifdef HAVE_OPCACHE_FILE_CACHE
	if (ZCG(accel_directives).file_cache) {
		add_assoc_string(return_value, "file_cache", ZCG(accel_directives).file_cache);
	}
	if (ZCG(accel_directives).file_cache_only) {
		add_assoc_bool(return_value, "file_cache_only", 1);
		return;
	}
#endif

	add_assoc_bool(return_value, "cache_full", ZSMMG(memory_exhausted));
	add_assoc_bool(return_value, "restart_pending", ZCSG(restart_pending));
	add_assoc_bool(return_value, "restart_in_progress", ZCSG(restart_in_progress));

	/* Memory usage statistics */
	array_init(&memory_usage);
	add_assoc_long(&memory_usage, "used_memory", ZCG(accel_directives).memory_consumption-zend_shared_alloc_get_free_memory()-ZSMMG(wasted_shared_memory));
	add_assoc_long(&memory_usage, "free_memory", zend_shared_alloc_get_free_memory());
	add_assoc_long(&memory_usage, "wasted_memory", ZSMMG(wasted_shared_memory));
	add_assoc_double(&memory_usage, "current_wasted_percentage", (((double) ZSMMG(wasted_shared_memory))/ZCG(accel_directives).memory_consumption)*100.0);
	add_assoc_zval(return_value, "memory_usage", &memory_usage);

	if (ZCSG(interned_strings_start) && ZCSG(interned_strings_end) && ZCSG(interned_strings_top)) {
		zval interned_strings_usage;

		array_init(&interned_strings_usage);
		add_assoc_long(&interned_strings_usage, "buffer_size", ZCSG(interned_strings_end) - ZCSG(interned_strings_start));
		add_assoc_long(&interned_strings_usage, "used_memory", ZCSG(interned_strings_top) - ZCSG(interned_strings_start));
		add_assoc_long(&interned_strings_usage, "free_memory", ZCSG(interned_strings_end) - ZCSG(interned_strings_top));
		add_assoc_long(&interned_strings_usage, "number_of_strings", ZCSG(interned_strings).nNumOfElements);
		add_assoc_zval(return_value, "interned_strings_usage", &interned_strings_usage);
	}

	/* Accelerator statistics */
	array_init(&statistics);
	add_assoc_long(&statistics, "num_cached_scripts", ZCSG(hash).num_direct_entries);
	add_assoc_long(&statistics, "num_cached_keys",    ZCSG(hash).num_entries);
	add_assoc_long(&statistics, "max_cached_keys",    ZCSG(hash).max_num_entries);
	add_assoc_long(&statistics, "hits", (zend_long)ZCSG(hits));
	add_assoc_long(&statistics, "start_time", ZCSG(start_time));
	add_assoc_long(&statistics, "last_restart_time", ZCSG(last_restart_time));
	add_assoc_long(&statistics, "oom_restarts", ZCSG(oom_restarts));
	add_assoc_long(&statistics, "hash_restarts", ZCSG(hash_restarts));
	add_assoc_long(&statistics, "manual_restarts", ZCSG(manual_restarts));
	add_assoc_long(&statistics, "misses", ZSMMG(memory_exhausted)?ZCSG(misses):ZCSG(misses)-ZCSG(blacklist_misses));
	add_assoc_long(&statistics, "blacklist_misses", ZCSG(blacklist_misses));
	reqs = ZCSG(hits)+ZCSG(misses);
	add_assoc_double(&statistics, "blacklist_miss_ratio", reqs?(((double) ZCSG(blacklist_misses))/reqs)*100.0:0);
	add_assoc_double(&statistics, "opcache_hit_rate", reqs?(((double) ZCSG(hits))/reqs)*100.0:0);
	add_assoc_zval(return_value, "opcache_statistics", &statistics);

	if (fetch_scripts) {
		/* accelerated scripts */
		if (accelerator_get_scripts(&scripts)) {
			add_assoc_zval(return_value, "scripts", &scripts);
		}
	}
}

static int add_blacklist_path(zend_blacklist_entry *p, zval *return_value)
{
	add_next_index_stringl(return_value, p->path, p->path_length);
	return 0;
}

/* {{{ proto array accelerator_get_configuration()
   Obtain configuration information */
static ZEND_FUNCTION(opcache_get_configuration)
{
	zval directives, version, blacklist;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_FALSE;
	}

	if (!validate_api_restriction()) {
		RETURN_FALSE;
	}

	array_init(return_value);

	/* directives */
	array_init(&directives);
	add_assoc_bool(&directives, "opcache.enable",              ZCG(enabled));
	add_assoc_bool(&directives, "opcache.enable_cli",          ZCG(accel_directives).enable_cli);
	add_assoc_bool(&directives, "opcache.use_cwd",             ZCG(accel_directives).use_cwd);
	add_assoc_bool(&directives, "opcache.validate_timestamps", ZCG(accel_directives).validate_timestamps);
	add_assoc_bool(&directives, "opcache.inherited_hack",      ZCG(accel_directives).inherited_hack);
	add_assoc_bool(&directives, "opcache.dups_fix",            ZCG(accel_directives).ignore_dups);
	add_assoc_bool(&directives, "opcache.revalidate_path",     ZCG(accel_directives).revalidate_path);

	add_assoc_long(&directives,   "opcache.log_verbosity_level",    ZCG(accel_directives).log_verbosity_level);
	add_assoc_long(&directives,	 "opcache.memory_consumption",     ZCG(accel_directives).memory_consumption);
	add_assoc_long(&directives,	 "opcache.interned_strings_buffer",ZCG(accel_directives).interned_strings_buffer);
	add_assoc_long(&directives, 	 "opcache.max_accelerated_files",  ZCG(accel_directives).max_accelerated_files);
	add_assoc_double(&directives, "opcache.max_wasted_percentage",  ZCG(accel_directives).max_wasted_percentage);
	add_assoc_long(&directives, 	 "opcache.consistency_checks",     ZCG(accel_directives).consistency_checks);
	add_assoc_long(&directives, 	 "opcache.force_restart_timeout",  ZCG(accel_directives).force_restart_timeout);
	add_assoc_long(&directives, 	 "opcache.revalidate_freq",        ZCG(accel_directives).revalidate_freq);
	add_assoc_string(&directives, "opcache.preferred_memory_model", STRING_NOT_NULL(ZCG(accel_directives).memory_model));
	add_assoc_string(&directives, "opcache.blacklist_filename",     STRING_NOT_NULL(ZCG(accel_directives).user_blacklist_filename));
	add_assoc_long(&directives,   "opcache.max_file_size",          ZCG(accel_directives).max_file_size);
	add_assoc_string(&directives, "opcache.error_log",              STRING_NOT_NULL(ZCG(accel_directives).error_log));

	add_assoc_bool(&directives,   "opcache.protect_memory",         ZCG(accel_directives).protect_memory);
	add_assoc_bool(&directives,   "opcache.save_comments",          ZCG(accel_directives).save_comments);
	add_assoc_bool(&directives,   "opcache.fast_shutdown",          ZCG(accel_directives).fast_shutdown);
	add_assoc_bool(&directives,   "opcache.enable_file_override",   ZCG(accel_directives).file_override_enabled);
	add_assoc_long(&directives, 	 "opcache.optimization_level",     ZCG(accel_directives).optimization_level);

#ifndef ZEND_WIN32
	add_assoc_string(&directives, "opcache.lockfile_path",          STRING_NOT_NULL(ZCG(accel_directives).lockfile_path));
#endif

#ifdef HAVE_OPCACHE_FILE_CACHE
	add_assoc_string(&directives, "opcache.file_cache",                    ZCG(accel_directives).file_cache ? ZCG(accel_directives).file_cache : "");
	add_assoc_bool(&directives,   "opcache.file_cache_only",               ZCG(accel_directives).file_cache_only);
	add_assoc_bool(&directives,   "opcache.file_cache_consistency_checks", ZCG(accel_directives).file_cache_consistency_checks);
#endif

	add_assoc_zval(return_value, "directives", &directives);

	/*version */
	array_init(&version);
	add_assoc_string(&version, "version", PHP_VERSION);
	add_assoc_string(&version, "opcache_product_name", ACCELERATOR_PRODUCT_NAME);
	add_assoc_zval(return_value, "version", &version);

	/* blacklist */
	array_init(&blacklist);
	zend_accel_blacklist_apply(&accel_blacklist, add_blacklist_path, &blacklist);
	add_assoc_zval(return_value, "blacklist", &blacklist);
}

/* {{{ proto void accelerator_reset()
   Request that the contents of the opcode cache to be reset */
static ZEND_FUNCTION(opcache_reset)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_FALSE;
	}

	if (!validate_api_restriction()) {
		RETURN_FALSE;
	}

	if ((!ZCG(enabled) || !accel_startup_ok || !ZCSG(accelerator_enabled))
#if ENABLE_FILE_CACHE_FALLBACK
	&& !fallback_process
#endif
	) {
		RETURN_FALSE;
	}

	zend_accel_schedule_restart(ACCEL_RESTART_USER);
	RETURN_TRUE;
}

/* {{{ proto void opcache_invalidate(string $script [, bool $force = false])
   Invalidates cached script (in necessary or forced) */
static ZEND_FUNCTION(opcache_invalidate)
{
	char *script_name;
	size_t script_name_len;
	zend_bool force = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|b", &script_name, &script_name_len, &force) == FAILURE) {
		return;
	}

	if (!validate_api_restriction()) {
		RETURN_FALSE;
	}

	if (zend_accel_invalidate(script_name, script_name_len, force) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

static ZEND_FUNCTION(opcache_compile_file)
{
	char *script_name;
	size_t script_name_len;
	zend_file_handle handle;
	zend_op_array *op_array = NULL;
	zend_execute_data *orig_execute_data = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &script_name, &script_name_len) == FAILURE) {
		return;
	}

	if (!ZCG(enabled) || !accel_startup_ok || !ZCSG(accelerator_enabled)) {
		zend_error(E_NOTICE, ACCELERATOR_PRODUCT_NAME " seems to be disabled, can't compile file");
		RETURN_FALSE;
	}

	handle.filename = script_name;
	handle.free_filename = 0;
	handle.opened_path = NULL;
	handle.type = ZEND_HANDLE_FILENAME;

	orig_execute_data = EG(current_execute_data);

	zend_try {
		op_array = persistent_compile_file(&handle, ZEND_INCLUDE);
	} zend_catch {
		EG(current_execute_data) = orig_execute_data;
		zend_error(E_WARNING, ACCELERATOR_PRODUCT_NAME " could not compile file %s", handle.filename);
	} zend_end_try();

	if(op_array != NULL) {
		destroy_op_array(op_array);
		efree(op_array);
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
	zend_destroy_file_handle(&handle);
}

/* {{{ proto bool opcache_is_script_cached(string $script)
   Return true if the script is cached in OPCache, false if it is not cached or if OPCache is not running. */
static ZEND_FUNCTION(opcache_is_script_cached)
{
	zend_string *script_name;

	if (!validate_api_restriction()) {
		RETURN_FALSE;
	}

	if (!ZCG(enabled) || !accel_startup_ok || !ZCSG(accelerator_enabled)) {
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &script_name) == FAILURE) {
		return;
	}

	RETURN_BOOL(filename_is_in_cache(script_name));
}
