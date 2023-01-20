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
   | Author: Zeev Suraski <zeev@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_GLOBALS_H
#define PHP_GLOBALS_H

#include "zend_globals.h"

typedef struct _php_core_globals php_core_globals;

#ifdef ZTS
# define PG(v) ZEND_TSRMG_FAST(core_globals_offset, php_core_globals *, v)
extern PHPAPI int core_globals_id;
extern PHPAPI size_t core_globals_offset;
#else
# define PG(v) (core_globals.v)
extern ZEND_API struct _php_core_globals core_globals;
#endif

/* Error display modes */
#define PHP_DISPLAY_ERRORS_STDOUT	1
#define PHP_DISPLAY_ERRORS_STDERR	2

/* Track vars */
#define TRACK_VARS_POST		0
#define TRACK_VARS_GET		1
#define TRACK_VARS_COOKIE	2
#define TRACK_VARS_SERVER	3
#define TRACK_VARS_ENV		4
#define TRACK_VARS_FILES	5
#define TRACK_VARS_REQUEST	6

struct _php_tick_function_entry;

typedef struct _arg_separators {
	char *output;
	char *input;
} arg_separators;

struct _php_core_globals {
	bool implicit_flush;

	zend_long output_buffering;

	bool enable_dl;

	char *output_handler;

	char *unserialize_callback_func;
	zend_long serialize_precision;

	zend_long memory_limit;
	zend_long max_input_time;

	zend_uchar display_errors;
	bool display_startup_errors;
	bool log_errors;
	bool ignore_repeated_errors;
	bool ignore_repeated_source;
	bool report_memleaks;
	char *error_log;

	char *doc_root;
	char *user_dir;
	char *include_path;
	char *open_basedir;
	char *extension_dir;
	char *php_binary;
	char *sys_temp_dir;

	char *upload_tmp_dir;
	zend_long upload_max_filesize;

	char *error_append_string;
	char *error_prepend_string;

	char *auto_prepend_file;
	char *auto_append_file;

	char *input_encoding;
	char *internal_encoding;
	char *output_encoding;

	arg_separators arg_separator;

	char *variables_order;

	HashTable rfc1867_protected_variables;

	short connection_status;
	bool ignore_user_abort;

	unsigned char header_is_being_sent;

	zend_llist tick_functions;

	zval http_globals[6];

	bool expose_php;

	bool register_argc_argv;
	bool auto_globals_jit;

	char *docref_root;
	char *docref_ext;

	bool html_errors;
	bool xmlrpc_errors;

	zend_long xmlrpc_error_number;

	bool activated_auto_globals[8];

	bool modules_activated;
	bool file_uploads;
	bool during_request_startup;
	bool allow_url_fopen;
	bool enable_post_data_reading;
	bool report_zend_debug;

	int last_error_type;
	zend_string *last_error_message;
	zend_string *last_error_file;
	int  last_error_lineno;

	char *php_sys_temp_dir;

	char *disable_classes;
	bool allow_url_include;
#ifdef PHP_WIN32
	bool com_initialized;
#endif
	zend_long max_input_nesting_level;
	zend_long max_input_vars;
	bool in_user_include;

	char *user_ini_filename;
	zend_long user_ini_cache_ttl;

	char *request_order;

	bool mail_x_header;
	bool mail_mixed_lf_and_crlf;
	char *mail_log;

	bool in_error_log;

#ifdef PHP_WIN32
	bool windows_show_crt_warning;
#endif

	zend_long syslog_facility;
	char *syslog_ident;
	bool have_called_openlog;
	zend_long syslog_filter;
	zend_long error_log_mode;
};


#endif /* PHP_GLOBALS_H */
