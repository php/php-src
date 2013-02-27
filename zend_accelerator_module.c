/*
   +----------------------------------------------------------------------+
   | Zend Optimizer+                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 The PHP Group                                |
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
#include "TSRM/tsrm_virtual_cwd.h"
#include "ext/standard/info.h"
#include "ext/standard/php_filestat.h"

#define STRING_NOT_NULL(s) (NULL == (s)?"":s)
#define MIN_ACCEL_FILES 200
#define MAX_ACCEL_FILES 100000
#define TOKENTOSTR(X) #X

/* User functions */
static ZEND_FUNCTION(accelerator_reset);

/* Private functions */
static ZEND_FUNCTION(accelerator_get_status);
static ZEND_FUNCTION(accelerator_get_configuration);

static zend_function_entry accel_functions[] = {
	/* User functions */
	ZEND_FE(accelerator_reset,					NULL)
	/* Private functions */
	ZEND_FE(accelerator_get_configuration,		NULL)
	ZEND_FE(accelerator_get_status,				NULL)
	{ NULL, NULL, NULL, 0, 0 }
};

static ZEND_INI_MH(OnUpdateMemoryConsumption)
{
	long *p;
	long memsize;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	/* keep the compiler happy */
	(void)entry; (void)new_value_length; (void)mh_arg2; (void)mh_arg3; (void)stage;

	p = (long *) (base + (size_t)mh_arg1);
	memsize = atoi(new_value);
	/* sanity check we must use at least 8 MB */
	if (memsize < 8) {
		const char *new_new_value = "8";
		zend_ini_entry *ini_entry;

		memsize = 8;
		zend_accel_error(ACCEL_LOG_WARNING, "zend_optimizerplus.memory_consumption is set below the required 8MB.\n");
		zend_accel_error(ACCEL_LOG_WARNING, ACCELERATOR_PRODUCT_NAME " will use the minimal 8MB cofiguration.\n");

		if (zend_hash_find(EG(ini_directives),
					"zend_optimizerplus.memory_consumption",
					sizeof("zend_optimizerplus.memory_consumption"),
					(void *) &ini_entry) == FAILURE) {
			return FAILURE;
		}

		ini_entry->value = strdup(new_new_value);
		ini_entry->value_length = strlen(new_new_value);
	}
	*p = memsize * (1024 * 1024);
	return SUCCESS;
}

static ZEND_INI_MH(OnUpdateMaxAcceleratedFiles)
{
	long *p;
	long size;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	/* keep the compiler happy */
	(void)entry; (void)new_value_length; (void)mh_arg2; (void)mh_arg3; (void)stage;

	p = (long *) (base + (size_t)mh_arg1);
	size = atoi(new_value);
	/* sanity check we must use a value between MIN_ACCEL_FILES and MAX_ACCEL_FILES */

	if (size < MIN_ACCEL_FILES || size > MAX_ACCEL_FILES) {
		const char *new_new_value;
		zend_ini_entry *ini_entry;

		if (size < MIN_ACCEL_FILES) {
			size = MIN_ACCEL_FILES;
			new_new_value = TOKENTOSTR(MIN_ACCEL_FILES);
			zend_accel_error(ACCEL_LOG_WARNING, "zend_optimizerplus.max_accelerated_files is set below the required minimum (%d).\n", MIN_ACCEL_FILES);
			zend_accel_error(ACCEL_LOG_WARNING, ACCELERATOR_PRODUCT_NAME " will use the minimal cofiguration.\n");
		}
		if (size > MAX_ACCEL_FILES) {
			size = MAX_ACCEL_FILES;
			new_new_value = TOKENTOSTR(MAX_ACCEL_FILES);
			zend_accel_error(ACCEL_LOG_WARNING, "zend_optimizerplus.max_accelerated_files is set above the limit (%d).\n", MAX_ACCEL_FILES);
			zend_accel_error(ACCEL_LOG_WARNING, ACCELERATOR_PRODUCT_NAME " will use the maximal cofiguration.\n");
		}
		if (zend_hash_find(EG(ini_directives),
					"zend_optimizerplus.max_accelerated_files",
					sizeof("zend_optimizerplus.max_accelerated_files"),
					(void *) &ini_entry) == FAILURE) {
			return FAILURE;
		}
		ini_entry->value = strdup(new_new_value);
		ini_entry->value_length = strlen(new_new_value);
	}
	*p = size;
	return SUCCESS;
}

static ZEND_INI_MH(OnUpdateMaxWastedPercentage)
{
	double *p;
	long percentage;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	/* keep the compiler happy */
	(void)entry; (void)new_value_length; (void)mh_arg2; (void)mh_arg3; (void)stage;

	p = (double *) (base + (size_t)mh_arg1);
	percentage = atoi(new_value);

	if (percentage <= 0 || percentage > 50) {
		const char *new_new_value = "5";
		zend_ini_entry *ini_entry;

		percentage = 5;
		zend_accel_error(ACCEL_LOG_WARNING, "zend_optimizerplus.max_wasted_percentage must be ser netweeb 1 and 50.\n");
		zend_accel_error(ACCEL_LOG_WARNING, ACCELERATOR_PRODUCT_NAME " will use 5%.\n");
		if (zend_hash_find(EG(ini_directives),
					"zend_optimizerplus.max_wasted_percentage",
					sizeof("zend_optimizerplus.max_wasted_percentage"),
					(void *) &ini_entry) == FAILURE) {
			return FAILURE;
		}
		ini_entry->value = strdup(new_new_value);
		ini_entry->value_length = strlen(new_new_value);
	}
	*p = (double)percentage / 100.0;
	return SUCCESS;
}

static ZEND_INI_MH(OnUpdateAccelBlacklist)
{
	char **p;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	/* keep the compiler happy */
	(void)entry; (void)new_value_length; (void)mh_arg2; (void)mh_arg3; (void)stage;

	if (new_value && !new_value[0]) {
		return FAILURE;
	}

	p = (char **) (base + (size_t)mh_arg1);
	*p = new_value;

	zend_accel_blacklist_init(&accel_blacklist);
	zend_accel_blacklist_load(&accel_blacklist, *p);

	return SUCCESS;
}

ZEND_INI_BEGIN()
    STD_PHP_INI_BOOLEAN("zend_optimizerplus.enable"             , "1", PHP_INI_SYSTEM, OnUpdateBool, enabled                             , zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("zend_optimizerplus.use_cwd"            , "1", PHP_INI_SYSTEM, OnUpdateBool, accel_directives.use_cwd            , zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("zend_optimizerplus.validate_timestamps", "1", PHP_INI_ALL   , OnUpdateBool, accel_directives.validate_timestamps, zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("zend_optimizerplus.inherited_hack"     , "1", PHP_INI_SYSTEM, OnUpdateBool, accel_directives.inherited_hack     , zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("zend_optimizerplus.dups_fix"           , "0", PHP_INI_ALL   , OnUpdateBool, accel_directives.ignore_dups        , zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("zend_optimizerplus.revalidate_path"    , "0", PHP_INI_ALL   , OnUpdateBool, accel_directives.revalidate_path    , zend_accel_globals, accel_globals)

	STD_PHP_INI_ENTRY("zend_optimizerplus.log_verbosity_level"   , "1"   , PHP_INI_SYSTEM, OnUpdateLong, accel_directives.log_verbosity_level,       zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("zend_optimizerplus.memory_consumption"    , "64"  , PHP_INI_SYSTEM, OnUpdateMemoryConsumption,    accel_directives.memory_consumption,        zend_accel_globals, accel_globals)
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
	STD_PHP_INI_ENTRY("zend_optimizerplus.interned_strings_buffer", "4"  , PHP_INI_SYSTEM, OnUpdateLong,                 accel_directives.interned_strings_buffer,   zend_accel_globals, accel_globals)
#endif
	STD_PHP_INI_ENTRY("zend_optimizerplus.max_accelerated_files" , "2000", PHP_INI_SYSTEM, OnUpdateMaxAcceleratedFiles,	 accel_directives.max_accelerated_files,     zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("zend_optimizerplus.max_wasted_percentage" , "5"   , PHP_INI_SYSTEM, OnUpdateMaxWastedPercentage,	 accel_directives.max_wasted_percentage,     zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("zend_optimizerplus.consistency_checks"    , "0"   , PHP_INI_ALL   , OnUpdateLong,	             accel_directives.consistency_checks,        zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("zend_optimizerplus.force_restart_timeout" , "180" , PHP_INI_SYSTEM, OnUpdateLong,	             accel_directives.force_restart_timeout,     zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("zend_optimizerplus.revalidate_freq"       , "2"   , PHP_INI_ALL   , OnUpdateLong,	             accel_directives.revalidate_freq,           zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("zend_optimizerplus.preferred_memory_model", ""    , PHP_INI_SYSTEM, OnUpdateStringUnempty,        accel_directives.memory_model,              zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("zend_optimizerplus.blacklist_filename"    , ""    , PHP_INI_SYSTEM, OnUpdateAccelBlacklist,	     accel_directives.user_blacklist_filename,   zend_accel_globals, accel_globals)

	STD_PHP_INI_ENTRY("zend_optimizerplus.protect_memory"        , "0"  , PHP_INI_SYSTEM, OnUpdateBool,                  accel_directives.protect_memory,            zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("zend_optimizerplus.save_comments"         , "1"  , PHP_INI_SYSTEM, OnUpdateBool,                  accel_directives.save_comments,             zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("zend_optimizerplus.load_comments"         , "1"  , PHP_INI_ALL,    OnUpdateBool,                  accel_directives.load_comments,             zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("zend_optimizerplus.fast_shutdown"         , "0"  , PHP_INI_SYSTEM, OnUpdateBool,                  accel_directives.fast_shutdown,             zend_accel_globals, accel_globals)

	STD_PHP_INI_ENTRY("zend_optimizerplus.optimization_level"    , DEFAULT_OPTIMIZATION_LEVEL , PHP_INI_SYSTEM, OnUpdateLong, accel_directives.optimization_level,   zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("zend_optimizerplus.enable_file_override"	, "0"   , PHP_INI_SYSTEM, OnUpdateBool,              accel_directives.file_override_enabled,     zend_accel_globals, accel_globals)
	STD_PHP_INI_BOOLEAN("zend_optimizerplus.enable_cli"             , "0"   , PHP_INI_SYSTEM, OnUpdateBool,              accel_directives.enable_cli,                zend_accel_globals, accel_globals)
	STD_PHP_INI_ENTRY("zend_optimizerplus.error_log"                , ""    , PHP_INI_SYSTEM, OnUpdateString,	         accel_directives.error_log,                 zend_accel_globals, accel_globals)

#ifdef ZEND_WIN32
	STD_PHP_INI_ENTRY("zend_optimizerplus.mmap_base", NULL, PHP_INI_SYSTEM,	OnUpdateString,	                             accel_directives.mmap_base,                 zend_accel_globals, accel_globals)
#endif
ZEND_INI_END()

#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO

#undef  EX
#define EX(element) execute_data->element
#define EX_T(offset) (*(temp_variable *)((char *) EX(Ts) + offset))

static int ZEND_DECLARE_INHERITED_CLASS_DELAYED_HANDLER(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_class_entry **pce, **pce_orig;

	if (zend_hash_find(EG(class_table), Z_STRVAL(EX(opline)->op2.u.constant), Z_STRLEN(EX(opline)->op2.u.constant) + 1, (void **)&pce) == FAILURE ||
	    (zend_hash_find(EG(class_table), Z_STRVAL(EX(opline)->op1.u.constant), Z_STRLEN(EX(opline)->op1.u.constant), (void**)&pce_orig) == SUCCESS &&
	     *pce != *pce_orig)) {
		do_bind_inherited_class(EX(opline), EG(class_table), EX_T(EX(opline)->extended_value).class_entry, 0 TSRMLS_CC);
	}
	EX(opline)++;
	return ZEND_USER_OPCODE_CONTINUE;
}
#endif

static int filename_is_in_cache(char *filename, int filename_len TSRMLS_DC)
{
	char *key;
	int key_length;
	zend_file_handle handle = {0};
	zend_persistent_script *persistent_script;

	handle.filename = filename;
	handle.type = ZEND_HANDLE_FILENAME;

	if (IS_ABSOLUTE_PATH(filename, filename_len)) {
		persistent_script = zend_accel_hash_find(&ZCSG(hash), filename, filename_len + 1);
		if (persistent_script) {
			return !persistent_script->corrupted;
		}
	}

	if ((key = accel_make_persistent_key_ex(&handle, filename_len, &key_length TSRMLS_CC)) != NULL) {
		persistent_script = zend_accel_hash_find(&ZCSG(hash), key, key_length + 1);
		return persistent_script && !persistent_script->corrupted;
	}

	return 0;
}

static void accel_file_in_cache(int type, INTERNAL_FUNCTION_PARAMETERS)
{
	char *filename;
	int filename_len;
#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
	zval **zfilename;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zfilename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(zfilename);
	filename = Z_STRVAL_PP(zfilename);
	filename_len = Z_STRLEN_PP(zfilename);
#elif ZEND_EXTENSION_API_NO == PHP_5_3_X_API_NO
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
		return;
	}
#else
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p", &filename, &filename_len) == FAILURE) {
		return;
	}
#endif
	if (filename_len > 0) {
		if (filename_is_in_cache(filename, filename_len TSRMLS_CC)) {
			RETURN_TRUE;
		}
	}

	php_stat(filename, filename_len, type, return_value TSRMLS_CC);
}

static void accel_file_exists(INTERNAL_FUNCTION_PARAMETERS)
{
	accel_file_in_cache(FS_EXISTS, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static void accel_is_file(INTERNAL_FUNCTION_PARAMETERS)
{
	accel_file_in_cache(FS_IS_FILE, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static void accel_is_readable(INTERNAL_FUNCTION_PARAMETERS)
{
	accel_file_in_cache(FS_IS_R, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static ZEND_MINIT_FUNCTION(zend_accelerator)
{
	(void)type; /* keep the compiler happy */

	/* must be 0 before the ini entry OnUpdate function is called */
	accel_blacklist.entries = NULL;

	REGISTER_INI_ENTRIES();
#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
	zend_set_user_opcode_handler(ZEND_DECLARE_INHERITED_CLASS_DELAYED, ZEND_DECLARE_INHERITED_CLASS_DELAYED_HANDLER);
#endif
	return SUCCESS;
}

void zend_accel_override_file_functions(TSRMLS_D)
{
	zend_function *old_function;
	if (ZCG(enabled) && accel_startup_ok && ZCG(accel_directives).file_override_enabled) {
		/* override file_exists */
		if (zend_hash_find(CG(function_table), "file_exists", sizeof("file_exists"), (void **)&old_function) == SUCCESS) {
			old_function->internal_function.handler = accel_file_exists;
		}
		if (zend_hash_find(CG(function_table), "is_file", sizeof("is_file"), (void **)&old_function) == SUCCESS) {
			old_function->internal_function.handler = accel_is_file;
		}
		if (zend_hash_find(CG(function_table), "is_readable", sizeof("is_readable"), (void **)&old_function) == SUCCESS) {
			old_function->internal_function.handler = accel_is_readable;
		}
	}
}

static ZEND_MSHUTDOWN_FUNCTION(zend_accelerator)
{
	(void)type; /* keep the compiler happy */

	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

void zend_accel_info(ZEND_MODULE_INFO_FUNC_ARGS)
{
	php_info_print_table_start();

	if (ZCG(enabled) && accel_startup_ok && ZCSG(accelerator_enabled)) {
		php_info_print_table_row(2, "Opcode Caching", "Up and Running");
	} else {
		php_info_print_table_row(2, "Opcode Caching", "Disabled");
	}
	if (ZCG(enabled) && accel_startup_ok && ZCSG(accelerator_enabled) && ZCG(accel_directives).optimization_level) {
		php_info_print_table_row(2, "Optimization", "Enabled");
	} else {
		php_info_print_table_row(2, "Optimization", "Disabled");
	}
	if (ZCG(enabled)) {
		if (!accel_startup_ok || zps_api_failure_reason) {
			php_info_print_table_row(2, "Startup Failed", zps_api_failure_reason);
		} else {
			php_info_print_table_row(2, "Startup", "OK");
			php_info_print_table_row(2, "Shared memory model", zend_accel_get_shared_model());
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
    ACCELERATOR_VERSION "FE",
	STANDARD_MODULE_PROPERTIES
};

int start_accel_module(void)
{
	return zend_startup_module(&accel_module_entry);
}

/* {{{ proto array accelerator_get_scripts()
   Get the scripts which are accelerated by ZendAccelerator */
static zval* accelerator_get_scripts(TSRMLS_D)
{
	uint i;
	zval *return_value,*persistent_script_report;
	zend_accel_hash_entry *cache_entry;
	struct tm *ta;
	struct timeval exec_time;
	struct timeval fetch_time;

	if (!ZCG(enabled) || !accel_startup_ok || !ZCSG(accelerator_enabled) || accelerator_shm_read_lock(TSRMLS_C) != SUCCESS) {
		return 0;
	}

	MAKE_STD_ZVAL(return_value);
	array_init(return_value);
	for (i = 0; i<ZCSG(hash).max_num_entries; i++) {
		for (cache_entry = ZCSG(hash).hash_table[i]; cache_entry; cache_entry = cache_entry->next) {
			zend_persistent_script *script;

			if (cache_entry->indirect) continue;

			script = (zend_persistent_script *)cache_entry->data;

			MAKE_STD_ZVAL(persistent_script_report);
			array_init(persistent_script_report);
			add_assoc_stringl(persistent_script_report, "full_path", script->full_path, script->full_path_len, 1);
			add_assoc_long(persistent_script_report, "hits", script->dynamic_members.hits);
			add_assoc_long(persistent_script_report, "memory_consumption", script->dynamic_members.memory_consumption);
			ta = localtime(&script->dynamic_members.last_used);
			add_assoc_string(persistent_script_report, "last_used", asctime(ta), 1);
			add_assoc_long(persistent_script_report, "last_used_timestamp", script->dynamic_members.last_used);
			if (ZCG(accel_directives).validate_timestamps) {
				add_assoc_long(persistent_script_report, "timestamp", (long)script->timestamp);
			}
			timerclear(&exec_time);
			timerclear(&fetch_time);

			zend_hash_update(return_value->value.ht, cache_entry->key, cache_entry->key_length, &persistent_script_report, sizeof(zval *), NULL);
		}
	}
	accelerator_shm_read_unlock(TSRMLS_C);

	return return_value;
}

/* {{{ proto array accelerator_get_status()
   Obtain statistics information regarding code acceleration in the Zend Performance Suite */
static ZEND_FUNCTION(accelerator_get_status)
{
	long reqs;
	zval *memory_usage,*statistics,*scripts;

	/* keep the compiler happy */
	(void)ht; (void)return_value_ptr; (void)this_ptr; (void)return_value_used;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_FALSE;
	}
	
	if (!ZCG(enabled) || !accel_startup_ok || !ZCSG(accelerator_enabled)) {
		RETURN_FALSE;
	}

	array_init(return_value);

	/* Trivia */
	add_assoc_bool(return_value, "accelerator_enabled", 1 /*ZCG(enabled) && accel_startup_ok && ZCSG(accelerator_enabled)*/);
	add_assoc_bool(return_value, "cache_full", ZSMMG(memory_exhausted));

	/* Memory usage statistics */
	MAKE_STD_ZVAL(memory_usage);
	array_init(memory_usage);
	add_assoc_long(memory_usage, "used_memory", ZCG(accel_directives).memory_consumption-zend_shared_alloc_get_free_memory()-ZSMMG(wasted_shared_memory));
	add_assoc_long(memory_usage, "free_memory", zend_shared_alloc_get_free_memory());
	add_assoc_long(memory_usage, "wasted_memory", ZSMMG(wasted_shared_memory));
	add_assoc_double(memory_usage, "current_wasted_percentage", (((double) ZSMMG(wasted_shared_memory))/ZCG(accel_directives).memory_consumption)*100.0);
	add_assoc_zval(return_value, "memory_usage", memory_usage);

	/* Accelerator statistics */
	MAKE_STD_ZVAL(statistics);
	array_init(statistics);
	add_assoc_long(statistics, "num_cached_scripts", ZCSG(hash).num_direct_entries);
	add_assoc_long(statistics, "max_cached_scripts", ZCSG(hash).max_num_entries);
	add_assoc_long(statistics, "hits", ZCSG(hits));
	add_assoc_long(statistics, "last_restart_time", ZCSG(last_restart_time));
	add_assoc_long(statistics, "misses", ZSMMG(memory_exhausted)?ZCSG(misses):ZCSG(misses)-ZCSG(blacklist_misses));
	add_assoc_long(statistics, "blacklist_misses", ZCSG(blacklist_misses));
	reqs = ZCSG(hits)+ZCSG(misses);
	add_assoc_double(statistics, "blacklist_miss_ratio", reqs?(((double) ZCSG(blacklist_misses))/reqs)*100.0:0);
	add_assoc_double(statistics, "accelerator_hit_rate", reqs?(((double) ZCSG(hits))/reqs)*100.0:0);
	add_assoc_zval(return_value, "accelerator_statistics", statistics);

	/* acceleratred scripts */
	scripts = accelerator_get_scripts(TSRMLS_C);
	if (scripts) {
		add_assoc_zval(return_value, "scripts", scripts);
	}
}

static int add_blacklist_path(zend_blacklist_entry *p, zval *return_value TSRMLS_DC)
{
	add_next_index_stringl(return_value, p->path, p->path_length, 1);
	return 0;
}

/* {{{ proto array accelerator_get_configuration()
   Obtain configuration information for the Zend Performance Suite */
static ZEND_FUNCTION(accelerator_get_configuration)
{
	zval *directives,*version,*blacklist;

	/* keep the compiler happy */
	(void)ht; (void)return_value_ptr; (void)this_ptr; (void)return_value_used;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_FALSE;
	}

	array_init(return_value);

	/* directives */
	MAKE_STD_ZVAL(directives);
	array_init(directives);
	add_assoc_bool(directives, "zend_optimizerplus.enable",              ZCG(enabled));
	add_assoc_bool(directives, "zend_optimizerplus.enable_cli",          ZCG(accel_directives).enable_cli);
	add_assoc_bool(directives, "zend_optimizerplus.use_cwd",             ZCG(accel_directives).use_cwd);
	add_assoc_bool(directives, "zend_optimizerplus.validate_timestamps", ZCG(accel_directives).validate_timestamps);
	add_assoc_bool(directives, "zend_optimizerplus.inherited_hack",      ZCG(accel_directives).inherited_hack);
	add_assoc_bool(directives, "zend_optimizerplus.dups_fix",            ZCG(accel_directives).ignore_dups);
	add_assoc_bool(directives, "zend_optimizerplus.revalidate_path",     ZCG(accel_directives).revalidate_path);

	add_assoc_long(directives,   "zend_optimizerplus.log_verbosity_level",    ZCG(accel_directives).log_verbosity_level);
	add_assoc_long(directives,	 "zend_optimizerplus.memory_consumption",     ZCG(accel_directives).memory_consumption);
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
	add_assoc_long(directives,	 "zend_optimizerplus.interned_strings_buffer",ZCG(accel_directives).interned_strings_buffer);
#endif
	add_assoc_long(directives, 	 "zend_optimizerplus.max_accelerated_files",  ZCG(accel_directives).max_accelerated_files);
	add_assoc_double(directives, "zend_optimizerplus.max_wasted_percentage",  ZCG(accel_directives).max_wasted_percentage);
	add_assoc_long(directives, 	 "zend_optimizerplus.consistency_checks",     ZCG(accel_directives).consistency_checks);
	add_assoc_long(directives, 	 "zend_optimizerplus.force_restart_timeout",  ZCG(accel_directives).force_restart_timeout);
	add_assoc_long(directives, 	 "zend_optimizerplus.revalidate_freq",        ZCG(accel_directives).revalidate_freq);
	add_assoc_string(directives, "zend_optimizerplus.preferred_memory_model", STRING_NOT_NULL(ZCG(accel_directives).memory_model), 1);
	add_assoc_string(directives, "zend_optimizerplus.blacklist_filename",     STRING_NOT_NULL(ZCG(accel_directives).user_blacklist_filename), 1);
	add_assoc_string(directives, "zend_optimizerplus.error_log",              STRING_NOT_NULL(ZCG(accel_directives).error_log), 1);

	add_assoc_bool(directives,   "zend_optimizerplus.protect_memory",         ZCG(accel_directives).protect_memory);
	add_assoc_bool(directives,   "zend_optimizerplus.save_comments",          ZCG(accel_directives).save_comments);
	add_assoc_bool(directives,   "zend_optimizerplus.load_comments",          ZCG(accel_directives).load_comments);
	add_assoc_bool(directives,   "zend_optimizerplus.fast_shutdown",          ZCG(accel_directives).fast_shutdown);
	add_assoc_bool(directives,   "zend_optimizerplus.enable_file_override",   ZCG(accel_directives).file_override_enabled);
	add_assoc_long(directives, 	 "zend_optimizerplus.optimization_level",         ZCG(accel_directives).optimization_level);

	add_assoc_zval(return_value, "directives", directives);

	/*version */
	MAKE_STD_ZVAL(version);
	array_init(version);
	add_assoc_string(version, "version", ACCELERATOR_VERSION, 1);
	add_assoc_string(version, "accelerator_product_name", ACCELERATOR_PRODUCT_NAME, 1);
	add_assoc_zval(return_value, "version", version);

	/* blacklist */
	MAKE_STD_ZVAL(blacklist);
	array_init(blacklist);
	zend_accel_blacklist_apply(&accel_blacklist, (apply_func_arg_t) add_blacklist_path, blacklist TSRMLS_CC);
	add_assoc_zval(return_value, "blacklist", blacklist);
}

/* {{{ proto void accelerator_reset()
   Request that the contents of the Accelerator module in the ZPS be reset */
static ZEND_FUNCTION(accelerator_reset)
{
	/* keep the compiler happy */
	(void)ht; (void)return_value_ptr; (void)this_ptr; (void)return_value_used;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_FALSE;
	}

	if (!ZCG(enabled) || !accel_startup_ok || !ZCSG(accelerator_enabled)) {
		RETURN_FALSE;
	}

	zend_accel_schedule_restart(TSRMLS_C);
	RETURN_TRUE;
}
