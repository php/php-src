/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
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
#include "ext/date/php_date.h"
#include "opcache_arginfo.h"

#if HAVE_JIT
#include "jit/zend_jit.h"
#endif

#define STRING_NOT_NULL(s) (NULL == (s)?"":s)
#define MIN_ACCEL_FILES 200
#define MAX_ACCEL_FILES 1000000
/* Max value of opcache.interned_strings_buffer */
#define MAX_INTERNED_STRINGS_BUFFER_SIZE ((zend_long)MIN( \
	MIN( \
		/* STRTAB_STR_TO_POS() must not overflow (zend_string_table_pos_t) */ \
		(ZEND_STRING_TABLE_POS_MAX - sizeof(zend_string_table)) / (1024 * 1024 / ZEND_STRING_TABLE_POS_ALIGNMENT), \
		/* nTableMask must not overflow (uint32_t) */ \
		UINT32_MAX / (32 * 1024 * sizeof(zend_string_table_pos_t)) \
	), \
	/* SHM allocation must not overflow (size_t) */ \
	(SIZE_MAX - sizeof(zend_accel_shared_globals)) / (1024 * 1024) \
))
#define TOKENTOSTR(X) #X

static zif_handler orig_file_exists = NULL;
static zif_handler orig_is_file = NULL;
static zif_handler orig_is_readable = NULL;

static int validate_api_restriction(void)
{
	if (ZCG(accel_directives).restrict_api && *ZCG(accel_directives).restrict_api) {
		size_t len = strlen(ZCG(accel_directives).restrict_api);

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
	zend_long *p = (zend_long *) ZEND_INI_GET_ADDR();
	zend_long memsize = atoi(ZSTR_VAL(new_value));
	/* sanity check we must use at least 8 MB */
	if (memsize < 8) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache.memory_consumption is set below the required 8MB.\n");
		return FAILURE;
	}
	if (UNEXPECTED(memsize > ZEND_LONG_MAX / (1024 * 1024))) {
		*p = ZEND_LONG_MAX & ~(1024 * 1024 - 1);
	} else {
		*p = memsize * (1024 * 1024);
	}
	return SUCCESS;
}

static ZEND_INI_MH(OnUpdateInternedStringsBuffer)
{
	zend_long *p = (zend_long *) ZEND_INI_GET_ADDR();
	zend_long size = zend_ini_parse_quantity_warn(new_value, entry->name);

	if (size < 0) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache.interned_strings_buffer must be greater than or equal to 0, " ZEND_LONG_FMT " given.\n", size);
		return FAILURE;
	}
	if (size > MAX_INTERNED_STRINGS_BUFFER_SIZE) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache.interned_strings_buffer must be less than or equal to " ZEND_LONG_FMT ", " ZEND_LONG_FMT " given.\n", MAX_INTERNED_STRINGS_BUFFER_SIZE, size);
		return FAILURE;
	}

	*p = size;

	return SUCCESS;
}

static ZEND_INI_MH(OnUpdateMaxAcceleratedFiles)
{
	zend_long *p = (zend_long *) ZEND_INI_GET_ADDR();
	zend_long size = atoi(ZSTR_VAL(new_value));
	/* sanity check we must use a value between MIN_ACCEL_FILES and MAX_ACCEL_FILES */
	if (size < MIN_ACCEL_FILES) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache.max_accelerated_files is set below the required minimum (%d).\n", MIN_ACCEL_FILES);
		return FAILURE;
	}
	if (size > MAX_ACCEL_FILES) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache.max_accelerated_files is set above the limit (%d).\n", MAX_ACCEL_FILES);
		return FAILURE;
	}
	*p = size;
	return SUCCESS;
}

static ZEND_INI_MH(OnUpdateMaxWastedPercentage)
{
	double *p = (double *) ZEND_INI_GET_ADDR();
	zend_long percentage = atoi(ZSTR_VAL(new_value));

	if (percentage <= 0 || percentage > 50) {
		zend_accel_error(ACCEL_LOG_WARNING, "opcache.max_wasted_percentage must be set between 1 and 50.\n");
		return FAILURE;
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
		bool *p = (bool *) ZEND_INI_GET_ADDR();
		if (zend_ini_parse_bool(new_value)) {
			zend_error(E_WARNING, ACCELERATOR_PRODUCT_NAME " can't be temporary enabled (it may be only disabled till the end of request)");
			return FAILURE;
		} else {
			*p = 0;
			ZCG(accelerator_enabled) = 0;
			return SUCCESS;
		}
	}
}

static ZEND_INI_MH(OnUpdateFileCache)
{
	if (new_value) {
		if (!ZSTR_LEN(new_value)) {
			new_value = NULL;
		} else {
			zend_stat_t buf = {0};

		    if (!IS_ABSOLUTE_PATH(ZSTR_VAL(new_value), ZSTR_LEN(new_value)) ||
			    zend_stat(ZSTR_VAL(new_value), &buf) != 0 ||
			    !S_ISDIR(buf.st_mode) ||
#ifndef ZEND_WIN32
				access(ZSTR_VAL(new_value), R_OK | W_OK | X_OK) != 0) {
#else
				_access(ZSTR_VAL(new_value), 06) != 0) {
#endif
				zend_accel_error(ACCEL_LOG_WARNING, "opcache.file_cache must be a full path of accessible directory.\n");
				new_value = NULL;
			}
		}
	}
	OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	return SUCCESS;
}

#ifdef HAVE_JIT
static ZEND_INI_MH(OnUpdateJit)
{
	if (zend_jit_config(new_value, stage) == SUCCESS) {
		return OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	}
	return FAILURE;
}

static ZEND_INI_MH(OnUpdateJitDebug)
{
	zend_long *p = (zend_long *) ZEND_INI_GET_ADDR();
	zend_long val = zend_ini_parse_quantity_warn(new_value, entry->name);

	if (zend_jit_debug_config(*p, val, stage) == SUCCESS) {
		*p = val;
		return SUCCESS;
	}
	return FAILURE;
}

static ZEND_INI_MH(OnUpdateCounter)
{
	zend_long val = zend_ini_parse_quantity_warn(new_value, entry->name);
	if (val >= 0 && val < 256) {
		zend_long *p = (zend_long *) ZEND_INI_GET_ADDR();
		*p = val;
		return SUCCESS;
	}
	zend_error(E_WARNING, "Invalid \"%s\" setting; using default value instead. Should be between 0 and 255", ZSTR_VAL(entry->name));
	return FAILURE;
}

static ZEND_INI_MH(OnUpdateUnrollC)
{
	zend_long val = zend_ini_parse_quantity_warn(new_value, entry->name);
	if (val > 0 && val < ZEND_JIT_TRACE_MAX_CALL_DEPTH) {
		zend_long *p = (zend_long *) ZEND_INI_GET_ADDR();
		*p = val;
		return SUCCESS;
	}
	zend_error(E_WARNING, "Invalid \"%s\" setting. Should be between 1 and %d", ZSTR_VAL(entry->name),
		ZEND_JIT_TRACE_MAX_CALL_DEPTH);
	return FAILURE;
}

static ZEND_INI_MH(OnUpdateUnrollR)
{
	zend_long val = zend_ini_parse_quantity_warn(new_value, entry->name);
	if (val >= 0 && val < ZEND_JIT_TRACE_MAX_RET_DEPTH) {
		zend_long *p = (zend_long *) ZEND_INI_GET_ADDR();
		*p = val;
		return SUCCESS;
	}
	zend_error(E_WARNING, "Invalid \"%s\" setting. Should be between 0 and %d", ZSTR_VAL(entry->name),
		ZEND_JIT_TRACE_MAX_RET_DEPTH);
	return FAILURE;
}

static ZEND_INI_MH(OnUpdateUnrollL)
{
	zend_long val = zend_ini_parse_quantity_warn(new_value, entry->name);
	if (val > 0 && val < ZEND_JIT_TRACE_MAX_LOOPS_UNROLL) {
		zend_long *p = (zend_long *) ZEND_INI_GET_ADDR();
		*p = val;
		return SUCCESS;
	}
	zend_error(E_WARNING, "Invalid \"%s\" setting. Should be between 1 and %d", ZSTR_VAL(entry->name),
		ZEND_JIT_TRACE_MAX_LOOPS_UNROLL);
	return FAILURE;
}

static ZEND_INI_MH(OnUpdateMaxTraceLength)
{
	zend_long val = zend_ini_parse_quantity_warn(new_value, entry->name);
	if (val > 3 && val <= ZEND_JIT_TRACE_MAX_LENGTH) {
		zend_long *p = (zend_long *) ZEND_INI_GET_ADDR();
		*p = val;
		return SUCCESS;
	}
	zend_error(E_WARNING, "Invalid \"%s\" setting. Should be between 4 and %d", ZSTR_VAL(entry->name),
		ZEND_JIT_TRACE_MAX_LENGTH);
	return FAILURE;
}
#endif

ZEND_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("opcache.enable"             , "1", PHP_INI_ALL,    OnEnable,     enabled                             , zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.use_cwd"            , "1", PHP_INI_SYSTEM, OnUpdateBool, accel_directives.use_cwd            , zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.validate_timestamps", "1", PHP_INI_ALL   , OnUpdateBool, accel_directives.validate_timestamps, zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.validate_permission", "0", PHP_INI_SYSTEM, OnUpdateBool, accel_directives.validate_permission, zend_accel_globals, accel_globals)
#ifndef ZEND_WIN32
	STD_PHP_INI_BOOLEAN("opcache.validate_root"      , "0", PHP_INI_SYSTEM, OnUpdateBool, accel_directives.validate_root      , zend_accel_globals, accel_globals)
#endif
	STD_PHP_INI_BOOLEAN("opcache.dups_fix"           , "0", PHP_INI_ALL   , OnUpdateBool, accel_directives.ignore_dups        , zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.revalidate_path"    , "0", PHP_INI_ALL   , OnUpdateBool, accel_directives.revalidate_path    , zend_accel_globals, accel_globals)

	STD_PHP_INI_ENTRY("opcache.log_verbosity_level"   , "1"   , PHP_INI_SYSTEM, OnUpdateLong, accel_directives.log_verbosity_level,       zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.memory_consumption"    , "128"  , PHP_INI_SYSTEM, OnUpdateMemoryConsumption,    accel_directives.memory_consumption,        zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.interned_strings_buffer", "8"  , PHP_INI_SYSTEM, OnUpdateInternedStringsBuffer,	 accel_directives.interned_strings_buffer,   zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.max_accelerated_files" , "10000", PHP_INI_SYSTEM, OnUpdateMaxAcceleratedFiles,	 accel_directives.max_accelerated_files,     zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.max_wasted_percentage" , "5"   , PHP_INI_SYSTEM, OnUpdateMaxWastedPercentage,	 accel_directives.max_wasted_percentage,     zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.force_restart_timeout" , "180" , PHP_INI_SYSTEM, OnUpdateLong,	             accel_directives.force_restart_timeout,     zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.revalidate_freq"       , "2"   , PHP_INI_ALL   , OnUpdateLong,	             accel_directives.revalidate_freq,           zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.file_update_protection", "2"   , PHP_INI_ALL   , OnUpdateLong,                accel_directives.file_update_protection,    zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.preferred_memory_model", ""    , PHP_INI_SYSTEM, OnUpdateStringUnempty,       accel_directives.memory_model,              zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.blacklist_filename"    , ""    , PHP_INI_SYSTEM, OnUpdateString,	             accel_directives.user_blacklist_filename,   zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.max_file_size"         , "0"   , PHP_INI_SYSTEM, OnUpdateLong,	             accel_directives.max_file_size,             zend_accel_globals, accel_globals)

	STD_PHP_INI_BOOLEAN("opcache.protect_memory"        , "0"  , PHP_INI_SYSTEM, OnUpdateBool,                  accel_directives.protect_memory,            zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.save_comments"         , "1"  , PHP_INI_SYSTEM, OnUpdateBool,                  accel_directives.save_comments,             zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.record_warnings"       , "0"  , PHP_INI_SYSTEM, OnUpdateBool,                  accel_directives.record_warnings,           zend_accel_globals, accel_globals)

	STD_PHP_INI_ENTRY("opcache.optimization_level"    , DEFAULT_OPTIMIZATION_LEVEL , PHP_INI_SYSTEM, OnUpdateLong, accel_directives.optimization_level,   zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.opt_debug_level"       , "0"      , PHP_INI_SYSTEM, OnUpdateLong,             accel_directives.opt_debug_level,            zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.enable_file_override"	, "0"   , PHP_INI_SYSTEM, OnUpdateBool,              accel_directives.file_override_enabled,     zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.enable_cli"             , "0"   , PHP_INI_SYSTEM, OnUpdateBool,              accel_directives.enable_cli,                zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.error_log"                , ""    , PHP_INI_SYSTEM, OnUpdateString,	         accel_directives.error_log,                 zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("opcache.restrict_api"             , ""    , PHP_INI_SYSTEM, OnUpdateString,	         accel_directives.restrict_api,              zend_accel_globals, accel_globals)

#ifndef ZEND_WIN32
	STD_PHP_INI_ENTRY("opcache.lockfile_path"             , "/tmp"    , PHP_INI_SYSTEM, OnUpdateString,           accel_directives.lockfile_path,              zend_accel_globals, accel_globals)
#else
	STD_PHP_INI_ENTRY("opcache.mmap_base", NULL, PHP_INI_SYSTEM,	OnUpdateString,	                             accel_directives.mmap_base,                 zend_accel_globals, accel_globals)
#endif

	STD_PHP_INI_ENTRY("opcache.file_cache"                    , NULL  , PHP_INI_SYSTEM, OnUpdateFileCache, accel_directives.file_cache,                    zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.file_cache_only"               , "0"   , PHP_INI_SYSTEM, OnUpdateBool,	   accel_directives.file_cache_only,               zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("opcache.file_cache_consistency_checks" , "1"   , PHP_INI_SYSTEM, OnUpdateBool,	   accel_directives.file_cache_consistency_checks, zend_accel_globals, accel_globals)
#if ENABLE_FILE_CACHE_FALLBACK
	STD_PHP_INI_BOOLEAN("opcache.file_cache_fallback"           , "1"   , PHP_INI_SYSTEM, OnUpdateBool,	   accel_directives.file_cache_fallback,           zend_accel_globals, accel_globals)
#endif
#ifdef HAVE_HUGE_CODE_PAGES
	STD_PHP_INI_BOOLEAN("opcache.huge_code_pages"             , "0"   , PHP_INI_SYSTEM, OnUpdateBool,      accel_directives.huge_code_pages,               zend_accel_globals, accel_globals)
#endif
	STD_PHP_INI_ENTRY("opcache.preload"                       , ""    , PHP_INI_SYSTEM, OnUpdateStringUnempty,    accel_directives.preload,                zend_accel_globals, accel_globals)
#ifndef ZEND_WIN32
	STD_PHP_INI_ENTRY("opcache.preload_user"                  , ""    , PHP_INI_SYSTEM, OnUpdateStringUnempty,    accel_directives.preload_user,           zend_accel_globals, accel_globals)
#endif
#if ZEND_WIN32
	STD_PHP_INI_ENTRY("opcache.cache_id"                      , ""    , PHP_INI_SYSTEM, OnUpdateString,           accel_directives.cache_id,               zend_accel_globals, accel_globals)
#endif
#ifdef HAVE_JIT
	STD_PHP_INI_ENTRY("opcache.jit"                           , "disable",                    PHP_INI_ALL,    OnUpdateJit,      options,               zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_buffer_size"               , ZEND_JIT_DEFAULT_BUFFER_SIZE, PHP_INI_SYSTEM, OnUpdateLong,     buffer_size,           zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_debug"                     , "0",                          PHP_INI_ALL,    OnUpdateJitDebug, debug,                 zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_bisect_limit"              , "0",                          PHP_INI_ALL,    OnUpdateLong,     bisect_limit,          zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_prof_threshold"            , "0.005",                      PHP_INI_ALL,    OnUpdateReal,     prof_threshold,        zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_max_root_traces"           , "1024",                       PHP_INI_SYSTEM, OnUpdateLong,     max_root_traces,       zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_max_side_traces"           , "128",                        PHP_INI_SYSTEM, OnUpdateLong,     max_side_traces,       zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_max_exit_counters"         , "8192",                       PHP_INI_SYSTEM, OnUpdateLong,     max_exit_counters,     zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_hot_loop"                  , "64",                         PHP_INI_SYSTEM, OnUpdateCounter,  hot_loop,              zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_hot_func"                  , "127",                        PHP_INI_SYSTEM, OnUpdateCounter,  hot_func,              zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_hot_return"                , "8",                          PHP_INI_SYSTEM, OnUpdateCounter,  hot_return,            zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_hot_side_exit"             , "8",                          PHP_INI_ALL,    OnUpdateCounter,  hot_side_exit,         zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_blacklist_root_trace"      , "16",                         PHP_INI_ALL,    OnUpdateCounter,  blacklist_root_trace,  zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_blacklist_side_trace"      , "8",                          PHP_INI_ALL,    OnUpdateCounter,  blacklist_side_trace,  zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_max_loop_unrolls"          , "8",                          PHP_INI_ALL,    OnUpdateUnrollL,  max_loop_unrolls,      zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_max_recursive_calls"       , "2",                          PHP_INI_ALL,    OnUpdateUnrollC,  max_recursive_calls,   zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_max_recursive_returns"     , "2",                          PHP_INI_ALL,    OnUpdateUnrollR,  max_recursive_returns, zend_jit_globals, jit_globals)
	STD_PHP_INI_ENTRY("opcache.jit_max_polymorphic_calls"     , "2",                          PHP_INI_ALL,    OnUpdateLong,     max_polymorphic_calls, zend_jit_globals, jit_globals)
    STD_PHP_INI_ENTRY("opcache.jit_max_trace_length"          , "1024",                       PHP_INI_ALL,    OnUpdateMaxTraceLength, max_trace_length, zend_jit_globals, jit_globals)
#endif
ZEND_INI_END()

static int filename_is_in_cache(zend_string *filename)
{
	zend_string *key;

	key = accel_make_persistent_key(filename);
	if (key != NULL) {
		zend_persistent_script *persistent_script = zend_accel_hash_find(&ZCSG(hash), key);
		if (persistent_script && !persistent_script->corrupted) {
			if (ZCG(accel_directives).validate_timestamps) {
				zend_file_handle handle;
				int ret;

				zend_stream_init_filename_ex(&handle, filename);
				ret = validate_timestamp_and_record_ex(persistent_script, &handle) == SUCCESS
					? 1 : 0;
				zend_destroy_file_handle(&handle);
				return ret;
			}

			return 1;
		}
	}

	return 0;
}

static int accel_file_in_cache(INTERNAL_FUNCTION_PARAMETERS)
{
	if (ZEND_NUM_ARGS() == 1) {
		zval *zv = ZEND_CALL_ARG(execute_data , 1);

		if (Z_TYPE_P(zv) == IS_STRING && Z_STRLEN_P(zv) != 0) {
			return filename_is_in_cache(Z_STR_P(zv));
		}
	}
	return 0;
}

static ZEND_NAMED_FUNCTION(accel_file_exists)
{
	if (accel_file_in_cache(INTERNAL_FUNCTION_PARAM_PASSTHRU)) {
		RETURN_TRUE;
	} else {
		orig_file_exists(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	}
}

static ZEND_NAMED_FUNCTION(accel_is_file)
{
	if (accel_file_in_cache(INTERNAL_FUNCTION_PARAM_PASSTHRU)) {
		RETURN_TRUE;
	} else {
		orig_is_file(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	}
}

static ZEND_NAMED_FUNCTION(accel_is_readable)
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
		if (file_cache_only) {
			zend_accel_error(ACCEL_LOG_WARNING, "file_override_enabled has no effect when file_cache_only is set");
			return;
		}
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

	if (ZCG(accelerator_enabled) || file_cache_only) {
		php_info_print_table_row(2, "Opcode Caching", "Up and Running");
	} else {
		php_info_print_table_row(2, "Opcode Caching", "Disabled");
	}
	if (ZCG(enabled) && accel_startup_ok && ZCG(accel_directives).optimization_level) {
		php_info_print_table_row(2, "Optimization", "Enabled");
	} else {
		php_info_print_table_row(2, "Optimization", "Disabled");
	}
	if (!file_cache_only) {
		php_info_print_table_row(2, "SHM Cache", "Enabled");
	} else {
		php_info_print_table_row(2, "SHM Cache", "Disabled");
	}
	if (ZCG(accel_directives).file_cache) {
		php_info_print_table_row(2, "File Cache", "Enabled");
	} else {
		php_info_print_table_row(2, "File Cache", "Disabled");
	}
#if HAVE_JIT
	if (JIT_G(enabled)) {
		if (JIT_G(on)) {
			php_info_print_table_row(2, "JIT", "On");
		} else {
			php_info_print_table_row(2, "JIT", "Off");
		}
	} else {
		php_info_print_table_row(2, "JIT", "Disabled");
	}
#else
	php_info_print_table_row(2, "JIT", "Not Available");
#endif
	if (file_cache_only) {
		if (!accel_startup_ok || zps_api_failure_reason) {
			php_info_print_table_row(2, "Startup Failed", zps_api_failure_reason);
		} else {
			php_info_print_table_row(2, "Startup", "OK");
		}
	} else
	if (ZCG(enabled)) {
		if (!accel_startup_ok || zps_api_failure_reason) {
			php_info_print_table_row(2, "Startup Failed", zps_api_failure_reason);
		} else {
			char buf[32];
			zend_string *start_time, *restart_time, *force_restart_time;
			zval *date_ISO8601 = zend_get_constant_str("DATE_ISO8601", sizeof("DATE_ISO8601")-1);

			php_info_print_table_row(2, "Startup", "OK");
			php_info_print_table_row(2, "Shared memory model", zend_accel_get_shared_model());
			snprintf(buf, sizeof(buf), ZEND_ULONG_FMT, ZCSG(hits));
			php_info_print_table_row(2, "Cache hits", buf);
			snprintf(buf, sizeof(buf), ZEND_ULONG_FMT, ZSMMG(memory_exhausted)?ZCSG(misses):ZCSG(misses)-ZCSG(blacklist_misses));
			php_info_print_table_row(2, "Cache misses", buf);
			snprintf(buf, sizeof(buf), ZEND_LONG_FMT, ZCG(accel_directives).memory_consumption-zend_shared_alloc_get_free_memory()-ZSMMG(wasted_shared_memory));
			php_info_print_table_row(2, "Used memory", buf);
			snprintf(buf, sizeof(buf), "%zu", zend_shared_alloc_get_free_memory());
			php_info_print_table_row(2, "Free memory", buf);
			snprintf(buf, sizeof(buf), "%zu", ZSMMG(wasted_shared_memory));
			php_info_print_table_row(2, "Wasted memory", buf);
			if (ZCSG(interned_strings).start && ZCSG(interned_strings).end) {
				snprintf(buf, sizeof(buf), "%zu", (size_t)((char*)ZCSG(interned_strings).top - (char*)(accel_shared_globals + 1)));
				php_info_print_table_row(2, "Interned Strings Used memory", buf);
				snprintf(buf, sizeof(buf), "%zu", (size_t)((char*)ZCSG(interned_strings).end - (char*)ZCSG(interned_strings).top));
				php_info_print_table_row(2, "Interned Strings Free memory", buf);
			}
			snprintf(buf, sizeof(buf), "%" PRIu32, ZCSG(hash).num_direct_entries);
			php_info_print_table_row(2, "Cached scripts", buf);
			snprintf(buf, sizeof(buf), "%" PRIu32, ZCSG(hash).num_entries);
			php_info_print_table_row(2, "Cached keys", buf);
			snprintf(buf, sizeof(buf), "%" PRIu32, ZCSG(hash).max_num_entries);
			php_info_print_table_row(2, "Max keys", buf);
			snprintf(buf, sizeof(buf), ZEND_ULONG_FMT, ZCSG(oom_restarts));
			php_info_print_table_row(2, "OOM restarts", buf);
			snprintf(buf, sizeof(buf), ZEND_ULONG_FMT, ZCSG(hash_restarts));
			php_info_print_table_row(2, "Hash keys restarts", buf);
			snprintf(buf, sizeof(buf), ZEND_ULONG_FMT, ZCSG(manual_restarts));
			php_info_print_table_row(2, "Manual restarts", buf);

			start_time = php_format_date(Z_STRVAL_P(date_ISO8601), Z_STRLEN_P(date_ISO8601), ZCSG(start_time), 1);
			php_info_print_table_row(2, "Start time", ZSTR_VAL(start_time));
			zend_string_release(start_time);

			if (ZCSG(last_restart_time)) {
				restart_time = php_format_date(Z_STRVAL_P(date_ISO8601), Z_STRLEN_P(date_ISO8601), ZCSG(last_restart_time), 1);
				php_info_print_table_row(2, "Last restart time", ZSTR_VAL(restart_time));
				zend_string_release(restart_time);
			} else {
				php_info_print_table_row(2, "Last restart time", "none");
			}

			if (ZCSG(force_restart_time)) {
				force_restart_time = php_format_date(Z_STRVAL_P(date_ISO8601), Z_STRLEN_P(date_ISO8601), ZCSG(force_restart_time), 1);
				php_info_print_table_row(2, "Last force restart time", ZSTR_VAL(force_restart_time));
				zend_string_release(force_restart_time);
			} else {
				php_info_print_table_row(2, "Last force restart time", "none");
			}
		}
	}

	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}

static zend_module_entry accel_module_entry = {
	STANDARD_MODULE_HEADER,
	ACCELERATOR_PRODUCT_NAME,
	ext_functions,
	ZEND_MINIT(zend_accelerator),
	ZEND_MSHUTDOWN(zend_accelerator),
	accel_activate,
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

/* {{{ Get the scripts which are accelerated by ZendAccelerator */
static int accelerator_get_scripts(zval *return_value)
{
	uint32_t i;
	zval persistent_script_report;
	zend_accel_hash_entry *cache_entry;
	struct tm *ta;
	struct timeval exec_time;
	struct timeval fetch_time;

	if (!ZCG(accelerator_enabled) || accelerator_shm_read_lock() != SUCCESS) {
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
			add_assoc_str(&persistent_script_report, "full_path", zend_string_dup(script->script.filename, 0));
			add_assoc_long(&persistent_script_report, "hits", script->dynamic_members.hits);
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

			add_assoc_long(&persistent_script_report, "revalidate", (zend_long)script->dynamic_members.revalidate);

			zend_hash_update(Z_ARRVAL_P(return_value), cache_entry->key, &persistent_script_report);
		}
	}
	accelerator_shm_read_unlock();

	return 1;
}

/* {{{ Obtain statistics information regarding code acceleration */
ZEND_FUNCTION(opcache_get_status)
{
	zend_long reqs;
	zval memory_usage, statistics, scripts;
	bool fetch_scripts = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &fetch_scripts) == FAILURE) {
		RETURN_THROWS();
	}

	if (!validate_api_restriction()) {
		RETURN_FALSE;
	}

	if (!accel_startup_ok) {
		RETURN_FALSE;
	}

	array_init(return_value);

	/* Trivia */
	add_assoc_bool(return_value, "opcache_enabled", ZCG(accelerator_enabled));

	if (ZCG(accel_directives).file_cache) {
		add_assoc_string(return_value, "file_cache", ZCG(accel_directives).file_cache);
	}
	if (file_cache_only) {
		add_assoc_bool(return_value, "file_cache_only", 1);
		return;
	}

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

	if (ZCSG(interned_strings).start && ZCSG(interned_strings).end) {
		zval interned_strings_usage;

		array_init(&interned_strings_usage);
		add_assoc_long(&interned_strings_usage, "buffer_size", (char*)ZCSG(interned_strings).end - (char*)(accel_shared_globals + 1));
		add_assoc_long(&interned_strings_usage, "used_memory", (char*)ZCSG(interned_strings).top - (char*)(accel_shared_globals + 1));
		add_assoc_long(&interned_strings_usage, "free_memory", (char*)ZCSG(interned_strings).end - (char*)ZCSG(interned_strings).top);
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

	if (ZCSG(preload_script)) {
		array_init(&statistics);

		add_assoc_long(&statistics, "memory_consumption", ZCSG(preload_script)->dynamic_members.memory_consumption);

		if (zend_hash_num_elements(&ZCSG(preload_script)->script.function_table)) {
			zend_op_array *op_array;

			array_init(&scripts);
			ZEND_HASH_MAP_FOREACH_PTR(&ZCSG(preload_script)->script.function_table, op_array) {
				add_next_index_str(&scripts, op_array->function_name);
			} ZEND_HASH_FOREACH_END();
			add_assoc_zval(&statistics, "functions", &scripts);
		}

		if (zend_hash_num_elements(&ZCSG(preload_script)->script.class_table)) {
			zval *zv;
			zend_string *key;

			array_init(&scripts);
			ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(&ZCSG(preload_script)->script.class_table, key, zv) {
				if (Z_TYPE_P(zv) == IS_ALIAS_PTR) {
					add_next_index_str(&scripts, key);
				} else {
					add_next_index_str(&scripts, Z_CE_P(zv)->name);
				}
			} ZEND_HASH_FOREACH_END();
			add_assoc_zval(&statistics, "classes", &scripts);
		}

		if (ZCSG(saved_scripts)) {
			zend_persistent_script **p = ZCSG(saved_scripts);

			array_init(&scripts);
			while (*p) {
				add_next_index_str(&scripts, (*p)->script.filename);
				p++;
			}
			add_assoc_zval(&statistics, "scripts", &scripts);
		}
		add_assoc_zval(return_value, "preload_statistics", &statistics);
	}

	if (fetch_scripts) {
		/* accelerated scripts */
		if (accelerator_get_scripts(&scripts)) {
			add_assoc_zval(return_value, "scripts", &scripts);
		}
	}
#if HAVE_JIT
	zend_jit_status(return_value);
#endif
}

static int add_blacklist_path(zend_blacklist_entry *p, zval *return_value)
{
	add_next_index_stringl(return_value, p->path, p->path_length);
	return 0;
}

/* {{{ Obtain configuration information */
ZEND_FUNCTION(opcache_get_configuration)
{
	zval directives, version, blacklist;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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
	add_assoc_bool(&directives, "opcache.validate_permission", ZCG(accel_directives).validate_permission);
#ifndef ZEND_WIN32
	add_assoc_bool(&directives, "opcache.validate_root",       ZCG(accel_directives).validate_root);
#endif
	add_assoc_bool(&directives, "opcache.dups_fix",            ZCG(accel_directives).ignore_dups);
	add_assoc_bool(&directives, "opcache.revalidate_path",     ZCG(accel_directives).revalidate_path);

	add_assoc_long(&directives,   "opcache.log_verbosity_level",    ZCG(accel_directives).log_verbosity_level);
	add_assoc_long(&directives,	 "opcache.memory_consumption",     ZCG(accel_directives).memory_consumption);
	add_assoc_long(&directives,	 "opcache.interned_strings_buffer",ZCG(accel_directives).interned_strings_buffer);
	add_assoc_long(&directives, 	 "opcache.max_accelerated_files",  ZCG(accel_directives).max_accelerated_files);
	add_assoc_double(&directives, "opcache.max_wasted_percentage",  ZCG(accel_directives).max_wasted_percentage);
	add_assoc_long(&directives, 	 "opcache.force_restart_timeout",  ZCG(accel_directives).force_restart_timeout);
	add_assoc_long(&directives, 	 "opcache.revalidate_freq",        ZCG(accel_directives).revalidate_freq);
	add_assoc_string(&directives, "opcache.preferred_memory_model", STRING_NOT_NULL(ZCG(accel_directives).memory_model));
	add_assoc_string(&directives, "opcache.blacklist_filename",     STRING_NOT_NULL(ZCG(accel_directives).user_blacklist_filename));
	add_assoc_long(&directives,   "opcache.max_file_size",          ZCG(accel_directives).max_file_size);
	add_assoc_string(&directives, "opcache.error_log",              STRING_NOT_NULL(ZCG(accel_directives).error_log));

	add_assoc_bool(&directives,   "opcache.protect_memory",         ZCG(accel_directives).protect_memory);
	add_assoc_bool(&directives,   "opcache.save_comments",          ZCG(accel_directives).save_comments);
	add_assoc_bool(&directives,   "opcache.record_warnings",        ZCG(accel_directives).record_warnings);
	add_assoc_bool(&directives,   "opcache.enable_file_override",   ZCG(accel_directives).file_override_enabled);
	add_assoc_long(&directives, 	 "opcache.optimization_level",     ZCG(accel_directives).optimization_level);

#ifndef ZEND_WIN32
	add_assoc_string(&directives, "opcache.lockfile_path",          STRING_NOT_NULL(ZCG(accel_directives).lockfile_path));
#else
	add_assoc_string(&directives, "opcache.mmap_base",              STRING_NOT_NULL(ZCG(accel_directives).mmap_base));
#endif

	add_assoc_string(&directives, "opcache.file_cache",                    ZCG(accel_directives).file_cache ? ZCG(accel_directives).file_cache : "");
	add_assoc_bool(&directives,   "opcache.file_cache_only",               ZCG(accel_directives).file_cache_only);
	add_assoc_bool(&directives,   "opcache.file_cache_consistency_checks", ZCG(accel_directives).file_cache_consistency_checks);
#if ENABLE_FILE_CACHE_FALLBACK
	add_assoc_bool(&directives,   "opcache.file_cache_fallback",           ZCG(accel_directives).file_cache_fallback);
#endif

	add_assoc_long(&directives,   "opcache.file_update_protection",  ZCG(accel_directives).file_update_protection);
	add_assoc_long(&directives,   "opcache.opt_debug_level",         ZCG(accel_directives).opt_debug_level);
	add_assoc_string(&directives, "opcache.restrict_api",            STRING_NOT_NULL(ZCG(accel_directives).restrict_api));
#ifdef HAVE_HUGE_CODE_PAGES
	add_assoc_bool(&directives,   "opcache.huge_code_pages",         ZCG(accel_directives).huge_code_pages);
#endif
	add_assoc_string(&directives, "opcache.preload", STRING_NOT_NULL(ZCG(accel_directives).preload));
#ifndef ZEND_WIN32
	add_assoc_string(&directives, "opcache.preload_user", STRING_NOT_NULL(ZCG(accel_directives).preload_user));
#endif
#if ZEND_WIN32
	add_assoc_string(&directives, "opcache.cache_id", STRING_NOT_NULL(ZCG(accel_directives).cache_id));
#endif
#ifdef HAVE_JIT
	add_assoc_string(&directives, "opcache.jit", JIT_G(options));
	add_assoc_long(&directives,   "opcache.jit_buffer_size", JIT_G(buffer_size));
	add_assoc_long(&directives,   "opcache.jit_debug", JIT_G(debug));
	add_assoc_long(&directives,   "opcache.jit_bisect_limit", JIT_G(bisect_limit));
	add_assoc_long(&directives,   "opcache.jit_blacklist_root_trace", JIT_G(blacklist_root_trace));
	add_assoc_long(&directives,   "opcache.jit_blacklist_side_trace", JIT_G(blacklist_side_trace));
	add_assoc_long(&directives,   "opcache.jit_hot_func", JIT_G(hot_func));
	add_assoc_long(&directives,   "opcache.jit_hot_loop", JIT_G(hot_loop));
	add_assoc_long(&directives,   "opcache.jit_hot_return", JIT_G(hot_return));
	add_assoc_long(&directives,   "opcache.jit_hot_side_exit", JIT_G(hot_side_exit));
	add_assoc_long(&directives,   "opcache.jit_max_exit_counters", JIT_G(max_exit_counters));
	add_assoc_long(&directives,   "opcache.jit_max_loop_unrolls", JIT_G(max_loop_unrolls));
	add_assoc_long(&directives,   "opcache.jit_max_polymorphic_calls", JIT_G(max_polymorphic_calls));
	add_assoc_long(&directives,   "opcache.jit_max_recursive_calls", JIT_G(max_recursive_calls));
	add_assoc_long(&directives,   "opcache.jit_max_recursive_returns", JIT_G(max_recursive_returns));
	add_assoc_long(&directives,   "opcache.jit_max_root_traces", JIT_G(max_root_traces));
	add_assoc_long(&directives,   "opcache.jit_max_side_traces", JIT_G(max_side_traces));
	add_assoc_long(&directives,   "opcache.jit_prof_threshold", JIT_G(prof_threshold));
	add_assoc_long(&directives,   "opcache.jit_max_trace_length", JIT_G(max_trace_length));
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

/* {{{ Request that the contents of the opcode cache to be reset */
ZEND_FUNCTION(opcache_reset)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

	/* exclusive lock */
	zend_shared_alloc_lock();
	zend_accel_schedule_restart(ACCEL_RESTART_USER);
	zend_shared_alloc_unlock();
	RETURN_TRUE;
}

/* {{{ Invalidates cached script (in necessary or forced) */
ZEND_FUNCTION(opcache_invalidate)
{
	zend_string *script_name;
	bool force = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|b", &script_name, &force) == FAILURE) {
		RETURN_THROWS();
	}

	if (!validate_api_restriction()) {
		RETURN_FALSE;
	}

	if (zend_accel_invalidate(script_name, force) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

ZEND_FUNCTION(opcache_compile_file)
{
	zend_string *script_name;
	zend_file_handle handle;
	zend_op_array *op_array = NULL;
	zend_execute_data *orig_execute_data = NULL;
	uint32_t orig_compiler_options;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &script_name) == FAILURE) {
		RETURN_THROWS();
	}

	if (!accel_startup_ok) {
		zend_error(E_NOTICE, ACCELERATOR_PRODUCT_NAME " has not been properly started, can't compile file");
		RETURN_FALSE;
	}

	zend_stream_init_filename_ex(&handle, script_name);

	orig_execute_data = EG(current_execute_data);
	orig_compiler_options = CG(compiler_options);
	CG(compiler_options) |= ZEND_COMPILE_WITHOUT_EXECUTION;

	if (CG(compiler_options) & ZEND_COMPILE_PRELOAD) {
		/* During preloading, a failure in opcache_compile_file() should result in an overall
		 * preloading failure. Otherwise we may include partially compiled files in the preload
		 * state. */
		op_array = persistent_compile_file(&handle, ZEND_INCLUDE);
	} else {
		zend_try {
			op_array = persistent_compile_file(&handle, ZEND_INCLUDE);
		} zend_catch {
			EG(current_execute_data) = orig_execute_data;
			zend_error(E_WARNING, ACCELERATOR_PRODUCT_NAME " could not compile file %s", ZSTR_VAL(handle.filename));
		} zend_end_try();
	}

	CG(compiler_options) = orig_compiler_options;

	if(op_array != NULL) {
		destroy_op_array(op_array);
		efree(op_array);
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
	zend_destroy_file_handle(&handle);
}

/* {{{ Return true if the script is cached in OPCache, false if it is not cached or if OPCache is not running. */
ZEND_FUNCTION(opcache_is_script_cached)
{
	zend_string *script_name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(script_name)
	ZEND_PARSE_PARAMETERS_END();

	if (!validate_api_restriction()) {
		RETURN_FALSE;
	}

	if (!ZCG(accelerator_enabled)) {
		RETURN_FALSE;
	}

	RETURN_BOOL(filename_is_in_cache(script_name));
}
