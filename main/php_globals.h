/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
*/


#ifndef PHP_GLOBALS_H
#define PHP_GLOBALS_H

#include "zend_globals.h"

typedef struct _php_core_globals php_core_globals;

#ifdef ZTS
# define PG(v) TSRMG(core_globals_id, php_core_globals *, v)
extern PHPAPI int core_globals_id;
#else
# define PG(v) (core_globals.v)
extern ZEND_API struct _php_core_globals core_globals;
#endif


#define TRACK_VARS_POST		0
#define TRACK_VARS_GET		1
#define TRACK_VARS_COOKIE	2
#define TRACK_VARS_SERVER	3
#define TRACK_VARS_ENV		4
#define TRACK_VARS_FILES	5

struct _php_tick_function_entry;

typedef struct _arg_separators {
	char *output;
	char *input;
} arg_separators;

struct _php_core_globals {
	zend_bool magic_quotes_gpc;
	zend_bool magic_quotes_runtime;
	zend_bool magic_quotes_sybase;

	zend_bool safe_mode;

	zend_bool allow_call_time_pass_reference;
	zend_bool implicit_flush;

	long output_buffering;

	char *safe_mode_include_dir;
	zend_bool safe_mode_gid;
	zend_bool sql_safe_mode;
	zend_bool enable_dl;

	char *output_handler;

	char *unserialize_callback_func;
	long serialize_precision;

	char *safe_mode_exec_dir;

	long memory_limit;
	long max_input_time;

	zend_bool track_errors;
	zend_bool display_errors;
	zend_bool display_startup_errors;
	zend_bool log_errors;
	long      log_errors_max_len;
	zend_bool ignore_repeated_errors;
	zend_bool ignore_repeated_source;
	zend_bool report_memleaks;
	char *error_log;

	char *doc_root;
	char *user_dir;
	char *include_path;
	char *open_basedir;
	char *extension_dir;

	char *upload_tmp_dir;
	long upload_max_filesize;
	
	char *error_append_string;
	char *error_prepend_string;

	char *auto_prepend_file;
	char *auto_append_file;

	arg_separators arg_separator;

	char *gpc_order;
	char *variables_order;

	HashTable rfc1867_protected_variables;

	short connection_status;
	short ignore_user_abort;

	unsigned char header_is_being_sent;

	zend_llist tick_functions;

	zval *http_globals[6];

	zend_bool expose_php;

	zend_bool register_globals;
	zend_bool register_argc_argv;

	zend_bool y2k_compliance;

	char *docref_root;
	char *docref_ext;

	zend_bool html_errors;
	zend_bool xmlrpc_errors;

	long xmlrpc_error_number;


	zend_bool modules_activated;

	zend_bool file_uploads;

	zend_bool during_request_startup;

	zend_bool allow_url_fopen;

	zend_bool always_populate_raw_post_data;
	
};


#endif /* PHP_GLOBALS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
