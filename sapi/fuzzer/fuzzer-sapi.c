/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Johannes Schlüter <johanes@php.net>                         |
   |          Stanislav Malyshev <stas@php.net>                           |
   +----------------------------------------------------------------------+
 */

#include <main/php.h>
#include <main/php_main.h>
#include <main/SAPI.h>
#include <ext/standard/info.h>
#include <ext/standard/php_var.h>
#include <main/php_variables.h>

#ifdef __SANITIZE_ADDRESS__
# include "sanitizer/lsan_interface.h"
#endif

#include "fuzzer.h"
#include "fuzzer-sapi.h"

const char HARDCODED_INI[] =
	"html_errors=0\n"
	"implicit_flush=1\n"
	"output_buffering=0\n"
	"error_reporting=0\n"
	/* Reduce oniguruma limits to speed up fuzzing */
	"mbstring.regex_stack_limit=10000\n"
	"mbstring.regex_retry_limit=10000";

static int startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, NULL, 0)==FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}

static size_t ub_write(const char *str, size_t str_length)
{
	/* quiet */
	return str_length;
}

static void fuzzer_flush(void *server_context)
{
	/* quiet */
}

static void send_header(sapi_header_struct *sapi_header, void *server_context)
{
}

static char* read_cookies()
{
	/* TODO: fuzz these! */
	return NULL;
}

static void register_variables(zval *track_vars_array)
{
	php_import_environment_variables(track_vars_array);
}

static void log_message(char *message, int level)
{
}


static sapi_module_struct fuzzer_module = {
	"fuzzer",               /* name */
	"clang fuzzer", /* pretty name */

	startup,             /* startup */
	php_module_shutdown_wrapper,   /* shutdown */

	NULL,                          /* activate */
	NULL,                          /* deactivate */

	ub_write,            /* unbuffered write */
	fuzzer_flush,               /* flush */
	NULL,                          /* get uid */
	NULL,                          /* getenv */

	php_error,                     /* error handler */

	NULL,                          /* header handler */
	NULL,                          /* send headers handler */
	send_header,         /* send header handler */

	NULL,                          /* read POST data */
	read_cookies,        /* read Cookies */

	register_variables,  /* register server variables */
	log_message,         /* Log message */
	NULL,                          /* Get request time */
	NULL,                          /* Child terminate */

	STANDARD_SAPI_MODULE_PROPERTIES
};

int fuzzer_init_php()
{
#ifdef __SANITIZE_ADDRESS__
	/* We're going to leak all the memory allocated during startup,
	 * so disable lsan temporarily. */
	__lsan_disable();
#endif

	sapi_startup(&fuzzer_module);
	fuzzer_module.phpinfo_as_text = 1;

	fuzzer_module.ini_entries = malloc(sizeof(HARDCODED_INI));
	memcpy(fuzzer_module.ini_entries, HARDCODED_INI, sizeof(HARDCODED_INI));

	/*
	 * TODO: we might want to test both Zend and malloc MM, but testing with malloc
	 * is more likely to find bugs, so use that for now.
	 */
	putenv("USE_ZEND_ALLOC=0");

	if (fuzzer_module.startup(&fuzzer_module)==FAILURE) {
		return FAILURE;
	}

#ifdef __SANITIZE_ADDRESS__
	__lsan_enable();
#endif

	return SUCCESS;
}

int fuzzer_request_startup()
{
	if (php_request_startup() == FAILURE) {
		php_module_shutdown();
		return FAILURE;
	}

	return SUCCESS;
}

void fuzzer_set_ini_file(const char *file)
{
	if (fuzzer_module.php_ini_path_override) {
		free(fuzzer_module.php_ini_path_override);
	}
	fuzzer_module.php_ini_path_override = strdup(file);
}


int fuzzer_shutdown_php()
{
	php_module_shutdown();
	sapi_shutdown();

	free(fuzzer_module.ini_entries);
	return SUCCESS;
}

int fuzzer_do_request(zend_file_handle *file_handle, char *filename)
{
	int retval = FAILURE; /* failure by default */

	SG(options) |= SAPI_OPTION_NO_CHDIR;
	SG(request_info).argc=0;
	SG(request_info).argv=NULL;

	if (fuzzer_request_startup() == FAILURE) {
		return FAILURE;
	}

	SG(headers_sent) = 1;
	SG(request_info).no_headers = 1;
	php_register_variable("PHP_SELF", filename, NULL);

	zend_first_try {
		zend_op_array *op_array = zend_compile_file(file_handle, ZEND_REQUIRE);
		if (op_array) {
			destroy_op_array(op_array);
			efree(op_array);
		}
		if (EG(exception)) {
			zend_object_release(EG(exception));
			EG(exception) = NULL;
		}
		/*retval = php_execute_script(file_handle);*/
	} zend_end_try();

	php_request_shutdown((void *) 0);

	return (retval == SUCCESS) ? SUCCESS : FAILURE;
}


int fuzzer_do_request_f(char *filename)
{
	zend_file_handle file_handle;
	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.filename = filename;
	file_handle.handle.fp = NULL;
	file_handle.opened_path = NULL;

	return fuzzer_do_request(&file_handle, filename);
}

int fuzzer_do_request_from_buffer(char *filename, char *data, size_t data_len)
{
	zend_file_handle file_handle;
	file_handle.filename = filename;
	file_handle.free_filename = 0;
	file_handle.opened_path = NULL;
	file_handle.handle.stream.handle = NULL;
	file_handle.handle.stream.reader = (zend_stream_reader_t)_php_stream_read;
	file_handle.handle.stream.fsizer = NULL;
	file_handle.handle.stream.isatty = 0;
	file_handle.handle.stream.closer   = NULL;
	file_handle.buf = data;
	file_handle.len = data_len;
	file_handle.type = ZEND_HANDLE_STREAM;

	return fuzzer_do_request(&file_handle, filename);
}

// Call named PHP function with N zval arguments
void fuzzer_call_php_func_zval(const char *func_name, int nargs, zval *args) {
	zval retval, func;

	ZVAL_STRING(&func, func_name);
	ZVAL_UNDEF(&retval);
	call_user_function(CG(function_table), NULL, &func, &retval, nargs, args);

	// TODO: check result?
	/* to ensure retval is not broken */
	php_var_dump(&retval, 0);

	/* cleanup */
	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&func);
}

// Call named PHP function with N string arguments
void fuzzer_call_php_func(const char *func_name, int nargs, char **params) {
	zval args[nargs];
	int i;

	for(i=0;i<nargs;i++) {
		ZVAL_STRING(&args[i], params[i]);
	}

	fuzzer_call_php_func_zval(func_name, nargs, args);

	for(i=0;i<nargs;i++) {
		zval_ptr_dtor(&args[i]);
		ZVAL_UNDEF(&args[i]);
	}
}
