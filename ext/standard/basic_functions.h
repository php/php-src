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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef BASIC_FUNCTIONS_H
#define BASIC_FUNCTIONS_H

#include <sys/stat.h>

#include "zend_highlight.h"

#include "url_scanner.h"
#include "url_scanner_ex.h"

extern zend_module_entry basic_functions_module;
#define basic_functions_module_ptr &basic_functions_module

PHP_MINIT_FUNCTION(basic);
PHP_MSHUTDOWN_FUNCTION(basic);
PHP_RINIT_FUNCTION(basic);
PHP_RSHUTDOWN_FUNCTION(basic);
PHP_MINFO_FUNCTION(basic);

PHP_FUNCTION(constant);
PHP_FUNCTION(toggle_short_open_tag);
PHP_FUNCTION(sleep);
PHP_FUNCTION(usleep);
PHP_FUNCTION(flush);
PHP_FUNCTION(ip2long);
PHP_FUNCTION(long2ip);

/* system functions */
PHP_FUNCTION(getenv);
PHP_FUNCTION(putenv);

PHP_FUNCTION(get_current_user);
PHP_FUNCTION(set_time_limit);

PHP_FUNCTION(get_cfg_var);
PHP_FUNCTION(set_magic_quotes_runtime);
PHP_FUNCTION(get_magic_quotes_runtime);
PHP_FUNCTION(get_magic_quotes_gpc);

PHP_FUNCTION(import_request_variables);

PHP_FUNCTION(error_log);

PHP_FUNCTION(call_user_func);
PHP_FUNCTION(call_user_func_array);
PHP_FUNCTION(call_user_method);
PHP_FUNCTION(call_user_method_array);

PHP_FUNCTION(register_shutdown_function);
PHP_FUNCTION(highlight_file);
PHP_FUNCTION(highlight_string);
ZEND_API void php_get_highlight_struct(zend_syntax_highlighter_ini *syntax_highlighter_ini);

PHP_FUNCTION(ini_get);
PHP_FUNCTION(ini_get_all);
PHP_FUNCTION(ini_set);
PHP_FUNCTION(ini_restore);

PHP_FUNCTION(print_r);

PHP_FUNCTION(connection_aborted);
PHP_FUNCTION(connection_status);
PHP_FUNCTION(ignore_user_abort);

PHP_FUNCTION(getservbyname);
PHP_FUNCTION(getservbyport);
PHP_FUNCTION(getprotobyname);
PHP_FUNCTION(getprotobynumber);

PHP_NAMED_FUNCTION(php_if_crc32);

PHP_FUNCTION(register_tick_function);
PHP_FUNCTION(unregister_tick_function);

PHP_FUNCTION(is_uploaded_file);
PHP_FUNCTION(move_uploaded_file);

/* From the INI parser */
PHP_FUNCTION(parse_ini_file);

PHP_FUNCTION(str_rot13);

#ifdef PHP_WIN32
typedef unsigned int php_stat_len;
#else
typedef int php_stat_len;
#endif

PHPAPI int _php_error_log(int opt_err, char *message, char *opt, char *headers TSRMLS_DC);

#if SIZEOF_INT == 4
/* Most 32-bit and 64-bit systems have 32-bit ints */
typedef unsigned int php_uint32;
typedef signed int php_int32;
#elif SIZEOF_LONG == 4
/* 16-bit systems? */
typedef unsigned long php_uint32;
typedef signed int php_int32;
#else
#error Need type which holds 32 bits
#endif

#define MT_N (624)

typedef struct {
	HashTable *user_shutdown_function_names;
	HashTable putenv_ht;
	zval *strtok_zval;
	char *strtok_string;
	char *locale_string;
	char *strtok_last;
	char strtok_table[256];
	ulong strtok_len;
	char str_ebuf[40];
	zval **array_walk_func_name;
	zval **user_compare_func_name;
	zend_llist *user_tick_functions;

	zval *active_ini_file_section;

	HashTable sm_protected_env_vars;
	char *sm_allowed_env_vars;

	/* file.c */
	char *user_agent;
	
	/* pageinfo.c */
	long page_uid;
	long page_gid;
	long page_inode;
	long page_mtime;

	/* filestat.c */
	char *CurrentStatFile;
	php_stat_len CurrentStatLength;
	struct stat sb;
	struct stat lsb;

	/* rand.c */
	php_uint32   state[MT_N+1];  /* state vector + 1 extra to not violate ANSI C */
	php_uint32   *next;       /* next random value is computed from here */
	int      left;        /* can *next++ this many times before reloading */

	unsigned int rand_seed; /* Seed for rand(), in ts version */

	zend_bool rand_is_seeded; /* Whether rand() has been seeded */
	zend_bool mt_rand_is_seeded; /* Whether mt_rand() has been seeded */
    
	/* syslog.c */
	int syslog_started;
	char *syslog_device;

	/* var.c */
	zend_class_entry *incomplete_class;

	/* url_scanner.c */
	url_adapt_state_t url_adapt_state;
	/* url_scanner_ex.re */
	url_adapt_state_ex_t url_adapt_state_ex;

#ifdef HAVE_MMAP
	void *mmap_file;
	size_t mmap_len;
#endif

	HashTable *aggregation_table;
} php_basic_globals;

#ifdef ZTS
#define BG(v) TSRMG(basic_globals_id, php_basic_globals *, v)
extern int basic_globals_id;
#else
#define BG(v) (basic_globals.v)
extern php_basic_globals basic_globals;
#endif

#if HAVE_PUTENV
typedef struct {
	char *putenv_string;
	char *previous_value;
	char *key;
	int key_len;
} putenv_entry;
#endif

/* Values are comma-delimited
 */
#define SAFE_MODE_PROTECTED_ENV_VARS	"LD_LIBRARY_PATH"
#define SAFE_MODE_ALLOWED_ENV_VARS		"PHP_"

#endif /* BASIC_FUNCTIONS_H */
