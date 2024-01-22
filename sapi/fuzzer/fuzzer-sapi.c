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
#include <zend_exceptions.h>

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
	/* Let the timeout be enforced by libfuzzer, not PHP. */
	"max_execution_time=0\n"
	/* Reduce oniguruma limits to speed up fuzzing */
	"mbstring.regex_stack_limit=10000\n"
	"mbstring.regex_retry_limit=10000\n"
	/* For the "execute" fuzzer disable some functions that are likely to have
	 * undesirable consequences (shell execution, file system writes). */
	"allow_url_include=0\n"
	"allow_url_fopen=0\n"
	"open_basedir=/tmp\n"
	"disable_functions=dl,mail,mb_send_mail"
	",shell_exec,exec,system,proc_open,popen,passthru,pcntl_exec"
	",chdir,chgrp,chmod,chown,copy,file_put_contents,lchgrp,lchown,link,mkdir"
	",move_uploaded_file,rename,rmdir,symlink,tempname,touch,unlink,fopen"
	/* Networking code likes to wait and wait. */
	",fsockopen,pfsockopen"
	",stream_socket_pair,stream_socket_client,stream_socket_server"
	/* crypt() can be very slow. */
	",crypt"
	/* openlog() has a known memory-management issue. */
	",openlog"
	/* Can cause long loops that bypass the executor step limit. */
	"\ndisable_classes=InfiniteIterator"
;

static int startup(sapi_module_struct *sapi_module)
{
	return php_module_startup(sapi_module, NULL);
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

static char* read_cookies(void)
{
	/* TODO: fuzz these! */
	return NULL;
}

static void register_variables(zval *track_vars_array)
{
	php_import_environment_variables(track_vars_array);
}

static void log_message(const char *message, int level)
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

int fuzzer_init_php(const char *extra_ini)
{
#ifdef __SANITIZE_ADDRESS__
	/* We're going to leak all the memory allocated during startup,
	 * so disable lsan temporarily. */
	__lsan_disable();
#endif

	sapi_startup(&fuzzer_module);
	fuzzer_module.phpinfo_as_text = 1;

	size_t ini_len = sizeof(HARDCODED_INI);
	size_t extra_ini_len = extra_ini ? strlen(extra_ini) : 0;
	if (extra_ini) {
		ini_len += extra_ini_len + 1;
	}
	char *p = malloc(ini_len + 1);
	fuzzer_module.ini_entries = p;
	p = zend_mempcpy(p, HARDCODED_INI, sizeof(HARDCODED_INI) - 1);
	if (extra_ini) {
		*p++ = '\n';
		p = zend_mempcpy(p, extra_ini, extra_ini_len);
	}
	*p = '\0';

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

int fuzzer_request_startup(void)
{
	if (php_request_startup() == FAILURE) {
		php_module_shutdown();
		return FAILURE;
	}

#ifdef ZEND_SIGNALS
	/* Some signal handlers will be overridden,
	 * don't complain about them during shutdown. */
	SIGG(check) = 0;
#endif

	return SUCCESS;
}

void fuzzer_request_shutdown(void)
{
	zend_try {
		/* Destroy thrown exceptions. This does not happen as part of request shutdown. */
		if (EG(exception)) {
			zend_object_release(EG(exception));
			EG(exception) = NULL;
		}

		/* Some fuzzers (like unserialize) may create circular structures. Make sure we free them.
		 * Two calls are performed to handle objects with destructors. */
		zend_gc_collect_cycles();
		zend_gc_collect_cycles();
	} zend_end_try();

	php_request_shutdown(NULL);
}

/* Set up a dummy stack frame so that exceptions may be thrown. */
void fuzzer_setup_dummy_frame(void)
{
	static zend_execute_data execute_data;
	static zend_function func;

	memset(&execute_data, 0, sizeof(zend_execute_data));
	memset(&func, 0, sizeof(zend_function));

	func.type = ZEND_INTERNAL_FUNCTION;
	func.common.function_name = ZSTR_EMPTY_ALLOC();
	execute_data.func = &func;
	EG(current_execute_data) = &execute_data;
}

void fuzzer_set_ini_file(const char *file)
{
	if (fuzzer_module.php_ini_path_override) {
		free(fuzzer_module.php_ini_path_override);
	}
	fuzzer_module.php_ini_path_override = strdup(file);
}


int fuzzer_shutdown_php(void)
{
	php_module_shutdown();
	sapi_shutdown();

	free((void *)fuzzer_module.ini_entries);
	return SUCCESS;
}

int fuzzer_do_request_from_buffer(
		char *filename, const char *data, size_t data_len, bool execute,
		void (*before_shutdown)(void))
{
	int retval = FAILURE; /* failure by default */

	SG(options) |= SAPI_OPTION_NO_CHDIR;
	SG(request_info).argc=0;
	SG(request_info).argv=NULL;

	if (fuzzer_request_startup() == FAILURE) {
		return FAILURE;
	}

	// Commented out to avoid leaking the header callback.
	//SG(headers_sent) = 1;
	//SG(request_info).no_headers = 1;
	php_register_variable("PHP_SELF", filename, NULL);

	zend_first_try {
		zend_file_handle file_handle;
		zend_stream_init_filename(&file_handle, filename);
		file_handle.primary_script = 1;
		file_handle.buf = emalloc(data_len + ZEND_MMAP_AHEAD);
		memcpy(file_handle.buf, data, data_len);
		memset(file_handle.buf + data_len, 0, ZEND_MMAP_AHEAD);
		file_handle.len = data_len;
		/* Avoid ZEND_HANDLE_FILENAME for opcache. */
		file_handle.type = ZEND_HANDLE_STREAM;

		zend_op_array *op_array = zend_compile_file(&file_handle, ZEND_REQUIRE);
		zend_destroy_file_handle(&file_handle);
		if (op_array) {
			if (execute) {
				zend_execute(op_array, NULL);
			}
			zend_destroy_static_vars(op_array);
			destroy_op_array(op_array);
			efree(op_array);
		}
	} zend_end_try();

	CG(compiled_filename) = NULL; /* ??? */
	if (before_shutdown) {
		zend_try {
			before_shutdown();
		} zend_end_try();
	}
	fuzzer_request_shutdown();

	return (retval == SUCCESS) ? SUCCESS : FAILURE;
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
