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
   | Author: Edin Kadribasic <edink@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "php_embed.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/dl_arginfo.h"

#ifdef PHP_WIN32
#include <io.h>
#include <fcntl.h>
#endif

#ifndef HAVE_EMBED_MAIN
static const char HARDCODED_INI[] =
	"html_errors=0\n"
	"implicit_flush=1\n"
	"output_buffering=0\n"
	"max_execution_time=0\n"
	"max_input_time=-1\n\0";

#if defined(PHP_WIN32) && defined(ZTS)
ZEND_TSRMLS_CACHE_DEFINE()
#endif

static char* php_embed_read_cookies(void)
{
	return NULL;
}

static int php_embed_deactivate(void)
{
	fflush(stdout);
	return SUCCESS;
}

/* Here we prefer to use write(), which is unbuffered, over fwrite(), which is
 * buffered. Using an unbuffered write operation to stdout will ensure PHP's
 * output buffering feature does not compete with a SAPI output buffer and
 * therefore we avoid situations wherein flushing the output buffer results in
 * nondeterministic behavior.
 */
static inline size_t php_embed_single_write(const char *str, size_t str_length)
{
#ifdef PHP_WRITE_STDOUT
	zend_long ret;

	ret = write(STDOUT_FILENO, str, str_length);
	if (ret <= 0) return 0;
	return ret;
#else
	size_t ret;

	ret = fwrite(str, 1, MIN(str_length, 16384), stdout);
	return ret;
#endif
}

/* SAPIs only have unbuffered write operations. This is because PHP's output
 * buffering feature will handle any buffering of the output and invoke the
 * SAPI unbuffered write operation when it flushes the buffer.
 */
static size_t php_embed_ub_write(const char *str, size_t str_length)
{
	const char *ptr = str;
	size_t remaining = str_length;
	size_t ret;

	while (remaining > 0) {
		ret = php_embed_single_write(ptr, remaining);
		if (!ret) {
			php_handle_aborted_connection();
		}
		ptr += ret;
		remaining -= ret;
	}

	return str_length;
}

static void php_embed_flush(void *server_context)
{
	if (fflush(stdout)==EOF) {
		php_handle_aborted_connection();
	}
}

static void php_embed_send_header(sapi_header_struct *sapi_header, void *server_context)
{
}

/* The SAPI error logger that is called when the 'error_log' INI setting is not
 * set.
 *
 * https://www.php.net/manual/en/errorfunc.configuration.php#ini.error-log
 */
static void php_embed_log_message(const char *message, int syslog_type_int)
{
	fprintf(stderr, "%s\n", message);
}

static void php_embed_register_variables(zval *track_vars_array)
{
	php_import_environment_variables(track_vars_array);

	php_register_variable("PHP_SELF", "-", track_vars_array);
}

/* Module initialization (MINIT) */
static int php_embed_startup(sapi_module_struct *sapi_module)
{
	return php_module_startup(sapi_module, NULL);
}

EMBED_SAPI_API sapi_module_struct php_embed_module = {
	"embed",                       /* name */
	"PHP Embedded Library",        /* pretty name */

	php_embed_startup,             /* startup */
	php_module_shutdown_wrapper,   /* shutdown */

	NULL,                          /* activate */
	php_embed_deactivate,          /* deactivate */

	php_embed_ub_write,            /* unbuffered write */
	php_embed_flush,               /* flush */
	NULL,                          /* get uid */
	NULL,                          /* getenv */

	php_error,                     /* error handler */

	NULL,                          /* header handler */
	NULL,                          /* send headers handler */
	php_embed_send_header,         /* send header handler */

	NULL,                          /* read POST data */
	php_embed_read_cookies,        /* read Cookies */

	php_embed_register_variables,  /* register server variables */
	php_embed_log_message,         /* Log message */
	NULL,                          /* Get request time */
	NULL,                          /* Child terminate */

	STANDARD_SAPI_MODULE_PROPERTIES
};
/* }}} */

static const zend_function_entry additional_functions[] = {
	ZEND_FE(dl, arginfo_dl)
	ZEND_FE_END
};

EMBED_SAPI_API int php_embed_init(int argc, char **argv)
{
#if defined(SIGPIPE) && defined(SIG_IGN)
	signal(SIGPIPE, SIG_IGN); /* ignore SIGPIPE in standalone mode so
								 that sockets created via fsockopen()
								 don't kill PHP if the remote site
								 closes it.  in apache|apxs mode apache
								 does that for us!  thies@thieso.net
								 20000419 */
#endif

#ifdef ZTS
	php_tsrm_startup();
# ifdef PHP_WIN32
	ZEND_TSRMLS_CACHE_UPDATE();
# endif
#endif

	zend_signal_startup();

	/* SAPI initialization (SINIT)
	 *
	 * Initialize the SAPI globals (memset to 0). After this point we can set
	 * SAPI globals via the SG() macro.
	 *
	 * Reentrancy startup.
	 *
	 * This also sets 'php_embed_module.ini_entries = NULL' so we cannot
	 * allocate the INI entries until after this call.
	 */
	sapi_startup(&php_embed_module);

#ifdef PHP_WIN32
	_fmode = _O_BINARY;			/*sets default for file streams to binary */
	_setmode(_fileno(stdin), O_BINARY);		/* make the stdio mode be binary */
	_setmode(_fileno(stdout), O_BINARY);		/* make the stdio mode be binary */
	_setmode(_fileno(stderr), O_BINARY);		/* make the stdio mode be binary */
#endif

	/* This hard-coded string of INI settings is parsed and read into PHP's
	 * configuration hash table at the very end of php_init_config(). This
	 * means these settings will overwrite any INI settings that were set from
	 * an INI file.
	 *
	 * To provide overwritable INI defaults, hook the ini_defaults function
	 * pointer that is part of the sapi_module_struct
	 * (php_embed_module.ini_defaults).
	 *
	 *     void (*ini_defaults)(HashTable *configuration_hash);
	 *
	 * This callback is invoked as soon as the configuration hash table is
	 * allocated so any INI settings added via this callback will have the
	 * lowest precedence and will allow INI files to overwrite them.
	 */
	php_embed_module.ini_entries = HARDCODED_INI;

	/* SAPI-provided functions. */
	php_embed_module.additional_functions = additional_functions;

	if (argv) {
		php_embed_module.executable_location = argv[0];
	}

	/* Module initialization (MINIT) */
	if (php_embed_module.startup(&php_embed_module) == FAILURE) {
		return FAILURE;
	}

	/* Do not chdir to the script's directory. This is akin to calling the CGI
	 * SAPI with '-C'.
	 */
	SG(options) |= SAPI_OPTION_NO_CHDIR;

	SG(request_info).argc=argc;
	SG(request_info).argv=argv;

	/* Request initialization (RINIT) */
	if (php_request_startup() == FAILURE) {
		php_module_shutdown();
		return FAILURE;
	}

	SG(headers_sent) = 1;
	SG(request_info).no_headers = 1;

	return SUCCESS;
}

EMBED_SAPI_API void php_embed_shutdown(void)
{
	/* Request shutdown (RSHUTDOWN) */
	php_request_shutdown((void *) 0);

	/* Module shutdown (MSHUTDOWN) */
	php_module_shutdown();

	/* SAPI shutdown (SSHUTDOWN) */
	sapi_shutdown();

#ifdef ZTS
	tsrm_shutdown();
#endif
}
#endif

#ifdef HAVE_EMBED_MAIN
# include <sys/mman.h>
# include "php_embed_main.h"

static char _php_embed_main_path_[MAXPATHLEN];

static php_stream *s_in_process = NULL;

static int php_embed_main_streams(void) {
	php_stream *s_in, *s_out, *s_err;
	php_stream_context *sc_in=NULL, *sc_out=NULL, *sc_err=NULL;
	zend_constant ic, oc, ec;

	s_in  = php_stream_open_wrapper_ex("php://stdin",  "rb", 0, NULL, sc_in);
	s_out = php_stream_open_wrapper_ex("php://stdout", "wb", 0, NULL, sc_out);
	s_err = php_stream_open_wrapper_ex("php://stderr", "wb", 0, NULL, sc_err);

	if (s_in) s_in->flags |= PHP_STREAM_FLAG_NO_RSCR_DTOR_CLOSE;
	if (s_out) s_out->flags |= PHP_STREAM_FLAG_NO_RSCR_DTOR_CLOSE;
	if (s_err) s_err->flags |= PHP_STREAM_FLAG_NO_RSCR_DTOR_CLOSE;

	if (s_in==NULL || s_out==NULL || s_err==NULL) {
		if (s_in) php_stream_close(s_in);
		if (s_out) php_stream_close(s_out);
		if (s_err) php_stream_close(s_err);
		return FAILURE;
	}

	s_in_process = s_in;

	php_stream_to_zval(s_in,  &ic.value);
	php_stream_to_zval(s_out, &oc.value);
	php_stream_to_zval(s_err, &ec.value);

	Z_CONSTANT_FLAGS(ic.value) = 0;
	ic.name = zend_string_init_interned("STDIN", sizeof("STDIN")-1, 0);
	zend_register_constant(&ic);

	Z_CONSTANT_FLAGS(oc.value) = 0;
	oc.name = zend_string_init_interned("STDOUT", sizeof("STDOUT")-1, 0);
	zend_register_constant(&oc);

	Z_CONSTANT_FLAGS(ec.value) = 0;
	ec.name = zend_string_init_interned("STDERR", sizeof("STDERR")-1, 0);
	zend_register_constant(&ec);

	return SUCCESS;
}

static void php_embed_main_variables(zval *track_vars_array)
{
	php_import_environment_variables(track_vars_array);

	php_register_variable("PHP_SELF",
		_php_embed_main_path_, track_vars_array);
}

static int php_embed_main_enter(void) {
	int fd;
	snprintf(_php_embed_main_path_,
		MAXPATHLEN, "/tmp/php_embed_main.XXXXXX");
	fd = mkstemp(_php_embed_main_path_);
	if (fd == FAILURE) {
		fprintf(stderr,
			"php_embed_main can not be initialized "
				"at /tmp/php_embed_main.*: %d %s\n",
			errno, strerror(errno));
		return FAILURE;
	}

	if (write(fd, php_embed_main, php_embed_main_len) == FAILURE) {
		fprintf(stderr,
			"php_embed_main can not be written "
				"at %s: %d %s\n",
			_php_embed_main_path_, errno, strerror(errno));
		return FAILURE;
	}
	lseek(fd, 0, SEEK_SET);
	close(fd);

#ifdef PHP_EMBED_LINK
	if (symlink(_php_embed_main_path_, PHP_EMBED_LINK) != SUCCESS) {
		fprintf(stderr,
			"php_embed_main link can not be initialized at %s: %d %s\n",
			PHP_EMBED_LINK,
			errno, strerror(errno));
		return FAILURE;
	}
	strcpy(_php_embed_main_path_, PHP_EMBED_LINK);
#endif

	php_embed_module.register_server_variables = php_embed_main_variables;

	return SUCCESS;
}

static int php_embed_main_execute(void) {
	int status = php_embed_main_streams();

	if (status != SUCCESS) {
		return 1;
	}

	zend_file_handle fh;
	zend_stream_init_filename(&fh,
		_php_embed_main_path_);
    if (!php_execute_script(&fh)) {
		if (EG(exit_status) == SUCCESS) {
			status = 1;
		}
	}
	zend_destroy_file_handle(&fh);

	if (status == FAILURE) {
		status = EG(exit_status);
	}

	return status;
}

static void php_embed_main_leave(void) {
	unlink(_php_embed_main_path_);
#ifdef PHP_EMBED_LINK
	unlink(PHP_EMBED_LINK);
#endif
}

int main(int argc, char *argv[])
{
    int  status = FAILURE;

	if (php_embed_main_enter() != SUCCESS) {
		return 1;
	}

    PHP_EMBED_START_BLOCK(argc, argv)

	status = php_embed_main_execute();

    PHP_EMBED_END_BLOCK()

	php_embed_main_leave();

    return status;
}
#endif
