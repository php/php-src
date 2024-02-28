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
   |         Marcus Boerger <helly@php.net>                               |
   |         Johannes Schlueter <johannes@php.net>                        |
   |         Parts based on CGI SAPI Module by                            |
   |         Rasmus Lerdorf, Stig Bakken and Zeev Suraski                 |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_globals.h"
#include "php_variables.h"
#include "php_ini_builder.h"
#include "zend_hash.h"
#include "zend_modules.h"
#include "zend_interfaces.h"

#include "ext/reflection/php_reflection.h"

#include "SAPI.h"

#include <stdio.h>
#include "php.h"
#ifdef PHP_WIN32
#include "win32/time.h"
#include "win32/signal.h"
#include "win32/console.h"
#include <process.h>
#include <shellapi.h>
#endif
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <signal.h>
#include <locale.h>
#include "zend.h"
#include "zend_extensions.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_main.h"
#include "fopen_wrappers.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/dl_arginfo.h"
#include "cli.h"
#ifdef PHP_WIN32
#include <io.h>
#include <fcntl.h>
#include "win32/php_registry.h"
#endif

#ifdef __riscos__
#include <unixlib/local.h>
#endif

#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_highlight.h"
#include "zend_exceptions.h"

#include "php_getopt.h"

#ifndef PHP_CLI_WIN32_NO_CONSOLE
#include "php_cli_server.h"
#endif

#include "ps_title.h"
#include "php_cli_process_title.h"
#include "php_cli_process_title_arginfo.h"

#ifndef PHP_WIN32
# define php_select(m, r, w, e, t)	select(m, r, w, e, t)
#else
# include "win32/select.h"
#endif

#if defined(PHP_WIN32) && defined(HAVE_OPENSSL)
# include "openssl/applink.c"
#endif

PHPAPI extern char *php_ini_opened_path;
PHPAPI extern char *php_ini_scanned_path;
PHPAPI extern char *php_ini_scanned_files;

#if defined(PHP_WIN32)
#if defined(ZTS)
ZEND_TSRMLS_CACHE_DEFINE()
#endif
static DWORD orig_cp = 0;
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#define PHP_MODE_STANDARD      1
#define PHP_MODE_HIGHLIGHT     2
#define PHP_MODE_LINT          4
#define PHP_MODE_STRIP         5
#define PHP_MODE_CLI_DIRECT    6
#define PHP_MODE_PROCESS_STDIN 7
#define PHP_MODE_REFLECTION_FUNCTION    8
#define PHP_MODE_REFLECTION_CLASS       9
#define PHP_MODE_REFLECTION_EXTENSION   10
#define PHP_MODE_REFLECTION_EXT_INFO    11
#define PHP_MODE_REFLECTION_ZEND_EXTENSION 12
#define PHP_MODE_SHOW_INI_CONFIG        13

cli_shell_callbacks_t cli_shell_callbacks = { NULL, NULL, NULL };
PHP_CLI_API cli_shell_callbacks_t *php_cli_get_shell_callbacks(void)
{
	return &cli_shell_callbacks;
}

const char HARDCODED_INI[] =
	"html_errors=0\n"
	"register_argc_argv=1\n"
	"implicit_flush=1\n"
	"output_buffering=0\n"
	"max_execution_time=0\n"
	"max_input_time=-1\n";


const opt_struct OPTIONS[] = {
	{'a', 0, "interactive"},
	{'B', 1, "process-begin"},
	{'C', 0, "no-chdir"}, /* for compatibility with CGI (do not chdir to script directory) */
	{'c', 1, "php-ini"},
	{'d', 1, "define"},
	{'E', 1, "process-end"},
	{'e', 0, "profile-info"},
	{'F', 1, "process-file"},
	{'f', 1, "file"},
	{'h', 0, "help"},
	{'i', 0, "info"},
	{'l', 0, "syntax-check"},
	{'m', 0, "modules"},
	{'n', 0, "no-php-ini"},
	{'q', 0, "no-header"}, /* for compatibility with CGI (do not generate HTTP headers) */
	{'R', 1, "process-code"},
	{'H', 0, "hide-args"},
	{'r', 1, "run"},
	{'s', 0, "syntax-highlight"},
	{'s', 0, "syntax-highlighting"},
	{'S', 1, "server"},
	{'t', 1, "docroot"},
	{'w', 0, "strip"},
	{'?', 0, "usage"},/* help alias (both '?' and 'usage') */
	{'v', 0, "version"},
	{'z', 1, "zend-extension"},
	{10,  1, "rf"},
	{10,  1, "rfunction"},
	{11,  1, "rc"},
	{11,  1, "rclass"},
	{12,  1, "re"},
	{12,  1, "rextension"},
	{13,  1, "rz"},
	{13,  1, "rzendextension"},
	{14,  1, "ri"},
	{14,  1, "rextinfo"},
	{15,  0, "ini"},
	/* Internal testing option -- may be changed or removed without notice,
	 * including in patch releases. */
	{16,  1, "repeat"},
	{'-', 0, NULL} /* end of args */
};

static int module_name_cmp(Bucket *f, Bucket *s) /* {{{ */
{
	return strcasecmp(((zend_module_entry *)Z_PTR(f->val))->name,
				  ((zend_module_entry *)Z_PTR(s->val))->name);
}
/* }}} */

static void print_modules(void) /* {{{ */
{
	HashTable sorted_registry;
	zend_module_entry *module;

	zend_hash_init(&sorted_registry, 50, NULL, NULL, 0);
	zend_hash_copy(&sorted_registry, &module_registry, NULL);
	zend_hash_sort(&sorted_registry, module_name_cmp, 0);
	ZEND_HASH_MAP_FOREACH_PTR(&sorted_registry, module) {
		php_printf("%s\n", module->name);
	} ZEND_HASH_FOREACH_END();
	zend_hash_destroy(&sorted_registry);
}
/* }}} */

static void print_extension_info(zend_extension *ext) /* {{{ */
{
	php_printf("%s\n", ext->name);
}
/* }}} */

static int extension_name_cmp(const zend_llist_element **f, const zend_llist_element **s) /* {{{ */
{
	zend_extension *fe = (zend_extension*)(*f)->data;
	zend_extension *se = (zend_extension*)(*s)->data;
	return strcmp(fe->name, se->name);
}
/* }}} */

static void print_extensions(void) /* {{{ */
{
	zend_llist sorted_exts;

	zend_llist_copy(&sorted_exts, &zend_extensions);
	sorted_exts.dtor = NULL;
	zend_llist_sort(&sorted_exts, extension_name_cmp);
	zend_llist_apply(&sorted_exts, (llist_apply_func_t) print_extension_info);
	zend_llist_destroy(&sorted_exts);
}
/* }}} */

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

static inline bool sapi_cli_select(php_socket_t fd)
{
	fd_set wfd;
	struct timeval tv;
	int ret;

	FD_ZERO(&wfd);

	PHP_SAFE_FD_SET(fd, &wfd);

	tv.tv_sec = (long)FG(default_socket_timeout);
	tv.tv_usec = 0;

	ret = php_select(fd+1, NULL, &wfd, NULL, &tv);

	return ret != -1;
}

PHP_CLI_API ssize_t sapi_cli_single_write(const char *str, size_t str_length) /* {{{ */
{
	ssize_t ret;

	if (cli_shell_callbacks.cli_shell_write) {
		cli_shell_callbacks.cli_shell_write(str, str_length);
	}

#ifdef PHP_WRITE_STDOUT
	do {
		ret = write(STDOUT_FILENO, str, str_length);
	} while (ret <= 0 && (errno == EINTR || (errno == EAGAIN && sapi_cli_select(STDOUT_FILENO))));
#else
	ret = fwrite(str, 1, MIN(str_length, 16384), stdout);
	if (ret == 0 && ferror(stdout)) {
		return -1;
	}
#endif
	return ret;
}
/* }}} */

static size_t sapi_cli_ub_write(const char *str, size_t str_length) /* {{{ */
{
	const char *ptr = str;
	size_t remaining = str_length;
	ssize_t ret;

	if (!str_length) {
		return 0;
	}

	if (cli_shell_callbacks.cli_shell_ub_write) {
		size_t ub_wrote;
		ub_wrote = cli_shell_callbacks.cli_shell_ub_write(str, str_length);
		if (ub_wrote != (size_t) -1) {
			return ub_wrote;
		}
	}

	while (remaining > 0)
	{
		ret = sapi_cli_single_write(ptr, remaining);
		if (ret < 0) {
#ifndef PHP_CLI_WIN32_NO_CONSOLE
			EG(exit_status) = 255;
			php_handle_aborted_connection();
#endif
			break;
		}
		ptr += ret;
		remaining -= ret;
	}

	return (ptr - str);
}
/* }}} */

static void sapi_cli_flush(void *server_context) /* {{{ */
{
	/* Ignore EBADF here, it's caused by the fact that STDIN/STDOUT/STDERR streams
	 * are/could be closed before fflush() is called.
	 */
	if (fflush(stdout)==EOF && errno!=EBADF) {
#ifndef PHP_CLI_WIN32_NO_CONSOLE
		php_handle_aborted_connection();
#endif
	}
}
/* }}} */

static char *php_self = "";
static char *script_filename = "";

static void sapi_cli_register_variables(zval *track_vars_array) /* {{{ */
{
	size_t len;
	char   *docroot = "";

	/* In CGI mode, we consider the environment to be a part of the server
	 * variables
	 */
	php_import_environment_variables(track_vars_array);

	/* Build the special-case PHP_SELF variable for the CLI version */
	len = strlen(php_self);
	if (sapi_module.input_filter(PARSE_SERVER, "PHP_SELF", &php_self, len, &len)) {
		php_register_variable("PHP_SELF", php_self, track_vars_array);
	}
	if (sapi_module.input_filter(PARSE_SERVER, "SCRIPT_NAME", &php_self, len, &len)) {
		php_register_variable("SCRIPT_NAME", php_self, track_vars_array);
	}
	/* filenames are empty for stdin */
	len = strlen(script_filename);
	if (sapi_module.input_filter(PARSE_SERVER, "SCRIPT_FILENAME", &script_filename, len, &len)) {
		php_register_variable("SCRIPT_FILENAME", script_filename, track_vars_array);
	}
	if (sapi_module.input_filter(PARSE_SERVER, "PATH_TRANSLATED", &script_filename, len, &len)) {
		php_register_variable("PATH_TRANSLATED", script_filename, track_vars_array);
	}
	/* just make it available */
	len = 0U;
	if (sapi_module.input_filter(PARSE_SERVER, "DOCUMENT_ROOT", &docroot, len, &len)) {
		php_register_variable("DOCUMENT_ROOT", docroot, track_vars_array);
	}
}
/* }}} */

static void sapi_cli_log_message(const char *message, int syslog_type_int) /* {{{ */
{
	fprintf(stderr, "%s\n", message);
#ifdef PHP_WIN32
	fflush(stderr);
#endif
}
/* }}} */

static int sapi_cli_deactivate(void) /* {{{ */
{
	fflush(stdout);
	if(SG(request_info).argv0) {
		free(SG(request_info).argv0);
		SG(request_info).argv0 = NULL;
	}
	return SUCCESS;
}
/* }}} */

static char* sapi_cli_read_cookies(void) /* {{{ */
{
	return NULL;
}
/* }}} */

static int sapi_cli_header_handler(sapi_header_struct *h, sapi_header_op_enum op, sapi_headers_struct *s) /* {{{ */
{
	return 0;
}
/* }}} */

static int sapi_cli_send_headers(sapi_headers_struct *sapi_headers) /* {{{ */
{
	/* We do nothing here, this function is needed to prevent that the fallback
	 * header handling is called. */
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}
/* }}} */

static void sapi_cli_send_header(sapi_header_struct *sapi_header, void *server_context) /* {{{ */
{
}
/* }}} */

static int php_cli_startup(sapi_module_struct *sapi_module) /* {{{ */
{
	return php_module_startup(sapi_module, NULL);
}
/* }}} */

static int php_cli_child_startup(sapi_module_struct *sapi_module) /* {{{ */
{
	return php_module_child_startup(sapi_module);
}
/* }}} */

/* {{{ sapi_cli_ini_defaults */

/* overwritable ini defaults must be set in sapi_cli_ini_defaults() */
#define INI_DEFAULT(name,value)\
	ZVAL_NEW_STR(&tmp, zend_string_init(value, sizeof(value)-1, 1));\
	zend_hash_str_update(configuration_hash, name, sizeof(name)-1, &tmp);\

static void sapi_cli_ini_defaults(HashTable *configuration_hash)
{
	zval tmp;
	INI_DEFAULT("display_errors", "1");
}
/* }}} */

/* {{{ sapi_module_struct cli_sapi_module */
static sapi_module_struct cli_sapi_module = {
	"cli",							/* name */
	"Command Line Interface",    	/* pretty name */

	php_cli_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	php_cli_child_startup,			/* child_startup */

	NULL,							/* activate */
	sapi_cli_deactivate,			/* deactivate */

	sapi_cli_ub_write,		    	/* unbuffered write */
	sapi_cli_flush,				    /* flush */
	NULL,							/* get uid */
	NULL,							/* getenv */

	php_error,						/* error handler */

	sapi_cli_header_handler,		/* header handler */
	sapi_cli_send_headers,			/* send headers handler */
	sapi_cli_send_header,			/* send header handler */

	NULL,				            /* read POST data */
	sapi_cli_read_cookies,          /* read Cookies */

	sapi_cli_register_variables,	/* register server variables */
	sapi_cli_log_message,			/* Log message */
	NULL,							/* Get request time */
	NULL,							/* Child terminate */

	STANDARD_SAPI_MODULE_PROPERTIES
};
/* }}} */

static const zend_function_entry additional_functions[] = {
	ZEND_FE(dl, arginfo_dl)
	PHP_FE(cli_set_process_title,        arginfo_cli_set_process_title)
	PHP_FE(cli_get_process_title,        arginfo_cli_get_process_title)
	PHP_FE_END
};

/* {{{ php_cli_usage */
static void php_cli_usage(char *argv0)
{
	char *prog;

	prog = strrchr(argv0, '/');
	if (prog) {
		prog++;
	} else {
		prog = "php";
	}

	printf( "Usage: %s [options] [-f] <file> [--] [args...]\n"
				"   %s [options] -r <code> [--] [args...]\n"
				"   %s [options] [-B <begin_code>] -R <code> [-E <end_code>] [--] [args...]\n"
				"   %s [options] [-B <begin_code>] -F <file> [-E <end_code>] [--] [args...]\n"
				"   %s [options] -S <addr>:<port> [-t docroot] [router]\n"
				"   %s [options] -- [args...]\n"
				"   %s [options] -a\n"
				"\n"
				"  -a               Run as interactive shell (requires readline extension)\n"
				"  -c <path>|<file> Look for php.ini file in this directory\n"
				"  -n               No configuration (ini) files will be used\n"
				"  -d foo[=bar]     Define INI entry foo with value 'bar'\n"
				"  -e               Generate extended information for debugger/profiler\n"
				"  -f <file>        Parse and execute <file>.\n"
				"  -h               This help\n"
				"  -i               PHP information\n"
				"  -l               Syntax check only (lint)\n"
				"  -m               Show compiled in modules\n"
				"  -r <code>        Run PHP <code> without using script tags <?..?>\n"
				"  -B <begin_code>  Run PHP <begin_code> before processing input lines\n"
				"  -R <code>        Run PHP <code> for every input line\n"
				"  -F <file>        Parse and execute <file> for every input line\n"
				"  -E <end_code>    Run PHP <end_code> after processing all input lines\n"
				"  -H               Hide any passed arguments from external tools.\n"
				"  -S <addr>:<port> Run with built-in web server.\n"
				"  -t <docroot>     Specify document root <docroot> for built-in web server.\n"
				"  -s               Output HTML syntax highlighted source.\n"
				"  -v               Version number\n"
				"  -w               Output source with stripped comments and whitespace.\n"
				"  -z <file>        Load Zend extension <file>.\n"
				"\n"
				"  args...          Arguments passed to script. Use -- args when first argument\n"
				"                   starts with - or script is read from stdin\n"
				"\n"
				"  --ini            Show configuration file names\n"
				"\n"
				"  --rf <name>      Show information about function <name>.\n"
				"  --rc <name>      Show information about class <name>.\n"
				"  --re <name>      Show information about extension <name>.\n"
				"  --rz <name>      Show information about Zend extension <name>.\n"
				"  --ri <name>      Show configuration for extension <name>.\n"
				"\n"
				, prog, prog, prog, prog, prog, prog, prog);
}
/* }}} */

static php_stream *s_in_process = NULL;

static void cli_register_file_handles(void)
{
	php_stream *s_in, *s_out, *s_err;
	php_stream_context *sc_in=NULL, *sc_out=NULL, *sc_err=NULL;
	zend_constant ic, oc, ec;

	s_in  = php_stream_open_wrapper_ex("php://stdin",  "rb", 0, NULL, sc_in);
	s_out = php_stream_open_wrapper_ex("php://stdout", "wb", 0, NULL, sc_out);
	s_err = php_stream_open_wrapper_ex("php://stderr", "wb", 0, NULL, sc_err);

	/* Release stream resources, but don't free the underlying handles. Otherwise,
	 * extensions which write to stderr or company during mshutdown/gshutdown
	 * won't have the expected functionality.
	 */
	if (s_in) s_in->flags |= PHP_STREAM_FLAG_NO_RSCR_DTOR_CLOSE;
	if (s_out) s_out->flags |= PHP_STREAM_FLAG_NO_RSCR_DTOR_CLOSE;
	if (s_err) s_err->flags |= PHP_STREAM_FLAG_NO_RSCR_DTOR_CLOSE;

	if (s_in==NULL || s_out==NULL || s_err==NULL) {
		if (s_in) php_stream_close(s_in);
		if (s_out) php_stream_close(s_out);
		if (s_err) php_stream_close(s_err);
		return;
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
}

static const char *param_mode_conflict = "Either execute direct code, process stdin or use a file.\n";

/* {{{ cli_seek_file_begin */
static zend_result cli_seek_file_begin(zend_file_handle *file_handle, char *script_file)
{
	FILE *fp = VCWD_FOPEN(script_file, "rb");
	if (!fp) {
		fprintf(stderr, "Could not open input file: %s\n", script_file);
		return FAILURE;
	}

	zend_stream_init_fp(file_handle, fp, script_file);
	file_handle->primary_script = 1;
	return SUCCESS;
}
/* }}} */

/*{{{ php_cli_win32_ctrl_handler */
#if defined(PHP_WIN32)
BOOL WINAPI php_cli_win32_ctrl_handler(DWORD sig)
{
	(void)php_win32_cp_cli_do_restore(orig_cp);

	return FALSE;
}
#endif
/*}}}*/

static int do_cli(int argc, char **argv) /* {{{ */
{
	int c;
	zend_file_handle file_handle;
	int behavior = PHP_MODE_STANDARD;
	char *reflection_what = NULL;
	volatile int request_started = 0;
	char *php_optarg = NULL, *orig_optarg = NULL;
	int php_optind = 1, orig_optind = 1;
	char *exec_direct=NULL, *exec_run=NULL, *exec_begin=NULL, *exec_end=NULL;
	char *arg_free=NULL, **arg_excp=&arg_free;
	char *script_file=NULL, *translated_path = NULL;
	bool interactive = false;
	const char *param_error=NULL;
	bool hide_argv = false;
	int num_repeats = 1;
	pid_t pid = getpid();

	file_handle.filename = NULL;

	zend_try {

		CG(in_compilation) = 0; /* not initialized but needed for several options */

		while ((c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0, 2)) != -1) {
			switch (c) {

			case 'i': /* php info & quit */
				if (php_request_startup()==FAILURE) {
					goto err;
				}
				request_started = 1;
				php_print_info(PHP_INFO_ALL & ~PHP_INFO_CREDITS);
				php_output_end_all();
				EG(exit_status) = 0;
				goto out;

			case 'v': /* show php version & quit */
				php_printf("PHP %s (%s) (built: %s %s) (%s)\nCopyright (c) The PHP Group\n%s",
					PHP_VERSION, cli_sapi_module.name, __DATE__, __TIME__,
#ifdef ZTS
					"ZTS"
#else
					"NTS"
#endif
#ifdef PHP_BUILD_COMPILER
					" " PHP_BUILD_COMPILER
#endif
#ifdef PHP_BUILD_ARCH
					" " PHP_BUILD_ARCH
#endif
#if ZEND_DEBUG
					" DEBUG"
#endif
#ifdef HAVE_GCOV
					" GCOV"
#endif
					,
					get_zend_version()
				);
				sapi_deactivate();
				goto out;

			case 'm': /* list compiled in modules */
				if (php_request_startup()==FAILURE) {
					goto err;
				}
				request_started = 1;
				php_printf("[PHP Modules]\n");
				print_modules();
				php_printf("\n[Zend Modules]\n");
				print_extensions();
				php_printf("\n");
				php_output_end_all();
				EG(exit_status) = 0;
				goto out;

			default:
				break;
			}
		}

		/* Set some CLI defaults */
		SG(options) |= SAPI_OPTION_NO_CHDIR;

		php_optind = orig_optind;
		php_optarg = orig_optarg;
		while ((c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0, 2)) != -1) {
			switch (c) {

			case 'a':	/* interactive mode */
				if (!cli_shell_callbacks.cli_shell_run) {
					param_error = "Interactive shell (-a) requires the readline extension.\n";
					break;
				}
				if (!interactive) {
					if (behavior != PHP_MODE_STANDARD) {
						param_error = param_mode_conflict;
						break;
					}

					interactive = true;
				}
				break;

			case 'C': /* don't chdir to the script directory */
				/* This is default so NOP */
				break;

			case 'F':
				if (behavior == PHP_MODE_PROCESS_STDIN) {
					if (exec_run || script_file) {
						param_error = "You can use -R or -F only once.\n";
						break;
					}
				} else if (behavior != PHP_MODE_STANDARD) {
					param_error = param_mode_conflict;
					break;
				}
				behavior=PHP_MODE_PROCESS_STDIN;
				script_file = php_optarg;
				break;

			case 'f': /* parse file */
				if (behavior == PHP_MODE_CLI_DIRECT || behavior == PHP_MODE_PROCESS_STDIN) {
					param_error = param_mode_conflict;
					break;
				} else if (script_file) {
					param_error = "You can use -f only once.\n";
					break;
				}
				script_file = php_optarg;
				break;

			case 'l': /* syntax check mode */
				if (behavior != PHP_MODE_STANDARD) {
					break;
				}
				behavior=PHP_MODE_LINT;
				/* We want to set the error exit status if at least one lint failed.
				 * If all were successful we set the exit status to 0.
				 * We already set EG(exit_status) here such that only failures set the exit status. */
				EG(exit_status) = 0;
				break;

			case 'q': /* do not generate HTTP headers */
				/* This is default so NOP */
				break;

			case 'r': /* run code from command line */
				if (behavior == PHP_MODE_CLI_DIRECT) {
					if (exec_direct || script_file) {
						param_error = "You can use -r only once.\n";
						break;
					}
				} else if (behavior != PHP_MODE_STANDARD || interactive) {
					param_error = param_mode_conflict;
					break;
				}
				behavior=PHP_MODE_CLI_DIRECT;
				exec_direct=php_optarg;
				break;

			case 'R':
				if (behavior == PHP_MODE_PROCESS_STDIN) {
					if (exec_run || script_file) {
						param_error = "You can use -R or -F only once.\n";
						break;
					}
				} else if (behavior != PHP_MODE_STANDARD) {
					param_error = param_mode_conflict;
					break;
				}
				behavior=PHP_MODE_PROCESS_STDIN;
				exec_run=php_optarg;
				break;

			case 'B':
				if (behavior == PHP_MODE_PROCESS_STDIN) {
					if (exec_begin) {
						param_error = "You can use -B only once.\n";
						break;
					}
				} else if (behavior != PHP_MODE_STANDARD || interactive) {
					param_error = param_mode_conflict;
					break;
				}
				behavior=PHP_MODE_PROCESS_STDIN;
				exec_begin=php_optarg;
				break;

			case 'E':
				if (behavior == PHP_MODE_PROCESS_STDIN) {
					if (exec_end) {
						param_error = "You can use -E only once.\n";
						break;
					}
				} else if (behavior != PHP_MODE_STANDARD || interactive) {
					param_error = param_mode_conflict;
					break;
				}
				behavior=PHP_MODE_PROCESS_STDIN;
				exec_end=php_optarg;
				break;

			case 's': /* generate highlighted HTML from source */
				if (behavior == PHP_MODE_CLI_DIRECT || behavior == PHP_MODE_PROCESS_STDIN) {
					param_error = "Source highlighting only works for files.\n";
					break;
				}
				behavior=PHP_MODE_HIGHLIGHT;
				break;

			case 'w':
				if (behavior == PHP_MODE_CLI_DIRECT || behavior == PHP_MODE_PROCESS_STDIN) {
					param_error = "Source stripping only works for files.\n";
					break;
				}
				behavior=PHP_MODE_STRIP;
				break;

			case 'z': /* load extension file */
				zend_load_extension(php_optarg);
				break;
			case 'H':
				hide_argv = true;
				break;
			case 10:
				behavior=PHP_MODE_REFLECTION_FUNCTION;
				reflection_what = php_optarg;
				break;
			case 11:
				behavior=PHP_MODE_REFLECTION_CLASS;
				reflection_what = php_optarg;
				break;
			case 12:
				behavior=PHP_MODE_REFLECTION_EXTENSION;
				reflection_what = php_optarg;
				break;
			case 13:
				behavior=PHP_MODE_REFLECTION_ZEND_EXTENSION;
				reflection_what = php_optarg;
				break;
			case 14:
				behavior=PHP_MODE_REFLECTION_EXT_INFO;
				reflection_what = php_optarg;
				break;
			case 15:
				behavior = PHP_MODE_SHOW_INI_CONFIG;
				break;
			case 16:
				num_repeats = atoi(php_optarg);
				break;
			default:
				break;
			}
		}

		if (param_error) {
			PUTS(param_error);
			EG(exit_status) = 1;
			goto err;
		}

#if defined(PHP_WIN32) && !defined(PHP_CLI_WIN32_NO_CONSOLE) && (HAVE_LIBREADLINE || HAVE_LIBEDIT) && !defined(COMPILE_DL_READLINE)
		if (!interactive) {
		/* The -a option was not passed. If there is no file, it could
		 	still make sense to run interactively. The presence of a file
			is essential to mitigate buggy console info. */
			interactive = php_win32_console_is_own() &&
				!(script_file ||
					argc > php_optind && behavior!=PHP_MODE_CLI_DIRECT &&
					behavior!=PHP_MODE_PROCESS_STDIN &&
					strcmp(argv[php_optind-1],"--")
				);
		}
#endif

		if (interactive) {
			printf("Interactive shell\n\n");
			fflush(stdout);
		}

		if (num_repeats > 1) {
			fprintf(stdout, "Executing for the first time...\n");
			fflush(stdout);
		}

do_repeat:
		/* only set script_file if not set already and not in direct mode and not at end of parameter list */
		if (argc > php_optind
		  && !script_file
		  && behavior!=PHP_MODE_CLI_DIRECT
		  && behavior!=PHP_MODE_PROCESS_STDIN
		  && strcmp(argv[php_optind-1],"--"))
		{
			script_file=argv[php_optind];
			php_optind++;
		}
		if (script_file) {
			virtual_cwd_activate();
			if (cli_seek_file_begin(&file_handle, script_file) == FAILURE) {
				goto err;
			} else {
				char real_path[MAXPATHLEN];
				if (VCWD_REALPATH(script_file, real_path)) {
					translated_path = strdup(real_path);
				}
				script_filename = script_file;
				php_self = script_file;
			}
		} else {
			/* We could handle PHP_MODE_PROCESS_STDIN in a different manner  */
			/* here but this would make things only more complicated. And it */
			/* is consistent with the way -R works where the stdin file handle*/
			/* is also accessible. */
			php_self = "Standard input code";
			if (behavior < PHP_MODE_CLI_DIRECT
			 && (!interactive || PHP_MODE_STANDARD != PHP_MODE_STANDARD)) {
				zend_stream_init_fp(&file_handle, stdin, php_self);
				file_handle.primary_script = 1;
			}
		}

		/* before registering argv to module exchange the *new* argv[0] */
		/* we can achieve this without allocating more memory */
		SG(request_info).argc=argc-php_optind+1;
		arg_excp = argv+php_optind-1;
		arg_free = argv[php_optind-1];
		SG(request_info).path_translated = translated_path ? translated_path : php_self;
		argv[php_optind-1] = php_self;
		SG(request_info).argv=argv+php_optind-1;

		if (php_request_startup()==FAILURE) {
			*arg_excp = arg_free;
			PUTS("Could not startup.\n");
			goto err;
		}
		request_started = 1;
		CG(skip_shebang) = 1;

		zend_register_bool_constant(
			ZEND_STRL("PHP_CLI_PROCESS_TITLE"),
			is_ps_title_available() == PS_TITLE_SUCCESS,
			0, 0);

		*arg_excp = arg_free; /* reconstruct argv */

		if (hide_argv) {
			int i;
			for (i = 1; i < argc; i++) {
				memset(argv[i], 0, strlen(argv[i]));
			}
		}

		zend_is_auto_global(ZSTR_KNOWN(ZEND_STR_AUTOGLOBAL_SERVER));

		PG(during_request_startup) = 0;
		switch (behavior) {
		case PHP_MODE_STANDARD:
			cli_register_file_handles();

			if (interactive) {
				EG(exit_status) = cli_shell_callbacks.cli_shell_run();
			} else {
				php_execute_script(&file_handle);
			}
			break;
		case PHP_MODE_LINT:
			if (php_lint_script(&file_handle) == SUCCESS) {
				zend_printf("No syntax errors detected in %s\n", php_self);
			} else {
				zend_printf("Errors parsing %s\n", php_self);
				EG(exit_status) = 255;
			}
			break;
		case PHP_MODE_STRIP:
			if (open_file_for_scanning(&file_handle)==SUCCESS) {
				zend_strip();
			}
			goto out;
			break;
		case PHP_MODE_HIGHLIGHT:
			{
				zend_syntax_highlighter_ini syntax_highlighter_ini;

				if (open_file_for_scanning(&file_handle)==SUCCESS) {
					php_get_highlight_struct(&syntax_highlighter_ini);
					zend_highlight(&syntax_highlighter_ini);
				}
				goto out;
			}
			break;
		case PHP_MODE_CLI_DIRECT:
			cli_register_file_handles();
			zend_eval_string_ex(exec_direct, NULL, "Command line code", 1);
			break;

		case PHP_MODE_PROCESS_STDIN:
			{
				char *input;
				size_t len, index = 0;
				zval argn, argi;

				if (!exec_run && script_file) {
					zend_string_release_ex(file_handle.filename, 0);
					file_handle.filename = NULL;
				}

				cli_register_file_handles();

				if (exec_begin) {
					zend_eval_string_ex(exec_begin, NULL, "Command line begin code", 1);
				}
				while (EG(exit_status) == SUCCESS && (input=php_stream_gets(s_in_process, NULL, 0)) != NULL) {
					len = strlen(input);
					while (len > 0 && len-- && (input[len]=='\n' || input[len]=='\r')) {
						input[len] = '\0';
					}
					ZVAL_STRINGL(&argn, input, len + 1);
					zend_hash_str_update(&EG(symbol_table), "argn", sizeof("argn")-1, &argn);
					ZVAL_LONG(&argi, ++index);
					zend_hash_str_update(&EG(symbol_table), "argi", sizeof("argi")-1, &argi);
					if (exec_run) {
						zend_eval_string_ex(exec_run, NULL, "Command line run code", 1);
					} else {
						if (script_file) {
							if (cli_seek_file_begin(&file_handle, script_file) == FAILURE) {
								EG(exit_status) = 1;
							} else {
								CG(skip_shebang) = 1;
								php_execute_script(&file_handle);
							}
						}
					}
					efree(input);
				}
				if (exec_end) {
					zend_eval_string_ex(exec_end, NULL, "Command line end code", 1);
				}

				break;
			}

			case PHP_MODE_REFLECTION_FUNCTION:
			case PHP_MODE_REFLECTION_CLASS:
			case PHP_MODE_REFLECTION_EXTENSION:
			case PHP_MODE_REFLECTION_ZEND_EXTENSION:
				{
					zend_class_entry *pce = NULL;
					zval arg, ref;
					zend_execute_data execute_data;

					switch (behavior) {
						default:
							break;
						case PHP_MODE_REFLECTION_FUNCTION:
							if (strstr(reflection_what, "::")) {
								pce = reflection_method_ptr;
							} else {
								pce = reflection_function_ptr;
							}
							break;
						case PHP_MODE_REFLECTION_CLASS:
							pce = reflection_class_ptr;
							break;
						case PHP_MODE_REFLECTION_EXTENSION:
							pce = reflection_extension_ptr;
							break;
						case PHP_MODE_REFLECTION_ZEND_EXTENSION:
							pce = reflection_zend_extension_ptr;
							break;
					}

					ZVAL_STRING(&arg, reflection_what);
					object_init_ex(&ref, pce);

					memset(&execute_data, 0, sizeof(zend_execute_data));
					EG(current_execute_data) = &execute_data;
					zend_call_known_instance_method_with_1_params(
						pce->constructor, Z_OBJ(ref), NULL, &arg);

					if (EG(exception)) {
						zval rv;
						zval *msg = zend_read_property_ex(zend_ce_exception, EG(exception), ZSTR_KNOWN(ZEND_STR_MESSAGE), /* silent */ false, &rv);
						zend_printf("Exception: %s\n", Z_STRVAL_P(msg));
						zend_object_release(EG(exception));
						EG(exception) = NULL;
						EG(exit_status) = 1;
					} else {
						zend_print_zval(&ref, 0);
						zend_write("\n", 1);
					}
					zval_ptr_dtor(&ref);
					zval_ptr_dtor(&arg);

					break;
				}
			case PHP_MODE_REFLECTION_EXT_INFO:
				{
					size_t len = strlen(reflection_what);
					char *lcname = zend_str_tolower_dup(reflection_what, len);
					zend_module_entry *module;

					if ((module = zend_hash_str_find_ptr(&module_registry, lcname, len)) == NULL) {
						if (!strcmp(reflection_what, "main")) {
							display_ini_entries(NULL);
						} else {
							zend_printf("Extension '%s' not present.\n", reflection_what);
							EG(exit_status) = 1;
						}
					} else {
						php_info_print_module(module);
					}

					efree(lcname);
					break;
				}

			case PHP_MODE_SHOW_INI_CONFIG:
				{
					zend_printf("Configuration File (php.ini) Path: %s\n", PHP_CONFIG_FILE_PATH);
					zend_printf("Loaded Configuration File:         %s\n", php_ini_opened_path ? php_ini_opened_path : "(none)");
					zend_printf("Scan for additional .ini files in: %s\n", php_ini_scanned_path  ? php_ini_scanned_path : "(none)");
					zend_printf("Additional .ini files parsed:      %s\n", php_ini_scanned_files ? php_ini_scanned_files : "(none)");
					break;
				}
		}
	} zend_end_try();

out:
	if (file_handle.filename) {
		zend_destroy_file_handle(&file_handle);
	}
	if (request_started) {
		php_request_shutdown((void *) 0);
		request_started = 0;
	}
	if (translated_path) {
		free(translated_path);
		translated_path = NULL;
	}
	if (behavior == PHP_MODE_LINT && argc > php_optind && strcmp(argv[php_optind],"--")) {
		script_file = NULL;
		goto do_repeat;
	}
	/* Don't repeat fork()ed processes. */
	if (--num_repeats && pid == getpid()) {
		fprintf(stdout, "Finished execution, repeating...\n");
		fflush(stdout);
		goto do_repeat;
	}
	return EG(exit_status);
err:
	sapi_deactivate();
	zend_ini_deactivate();
	EG(exit_status) = 1;
	goto out;
}
/* }}} */

/* {{{ main */
#ifdef PHP_CLI_WIN32_NO_CONSOLE
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int main(int argc, char *argv[])
#endif
{
#if defined(PHP_WIN32)
# ifdef PHP_CLI_WIN32_NO_CONSOLE
	int argc = __argc;
	char **argv = __argv;
# endif
	int num_args;
	wchar_t **argv_wide;
	char **argv_save = argv;
	BOOL using_wide_argv = 0;
#endif

	int c;
	int exit_status = SUCCESS;
	int module_started = 0, sapi_started = 0;
	char *php_optarg = NULL;
	int php_optind = 1, use_extended_info = 0;
	char *ini_path_override = NULL;
	struct php_ini_builder ini_builder;
	int ini_ignore = 0;
	sapi_module_struct *sapi_module = &cli_sapi_module;

	/*
	 * Do not move this initialization. It needs to happen before argv is used
	 * in any way.
	 */
	argv = save_ps_args(argc, argv);

#if defined(PHP_WIN32) && !defined(PHP_CLI_WIN32_NO_CONSOLE)
	php_win32_console_fileno_set_vt100(STDOUT_FILENO, TRUE);
	php_win32_console_fileno_set_vt100(STDERR_FILENO, TRUE);
#endif

	cli_sapi_module.additional_functions = additional_functions;

#if defined(PHP_WIN32) && defined(_DEBUG)
	{
		char *tmp = getenv("PHP_WIN32_DEBUG_HEAP");
		if (tmp && ZEND_ATOL(tmp)) {
			int tmp_flag;
			_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
			_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
			_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
			_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
			_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
			_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
			tmp_flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
			tmp_flag |= _CRTDBG_DELAY_FREE_MEM_DF;
			tmp_flag |= _CRTDBG_LEAK_CHECK_DF;

			_CrtSetDbgFlag(tmp_flag);
		}
	}
#endif

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

#ifdef PHP_WIN32
	_fmode = _O_BINARY;			/*sets default for file streams to binary */
	setmode(_fileno(stdin), O_BINARY);		/* make the stdio mode be binary */
	setmode(_fileno(stdout), O_BINARY);		/* make the stdio mode be binary */
	setmode(_fileno(stderr), O_BINARY);		/* make the stdio mode be binary */
#endif

	php_ini_builder_init(&ini_builder);

	while ((c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 1, 2))!=-1) {
		switch (c) {
			case 'c':
				if (ini_path_override) {
					free(ini_path_override);
				}
				ini_path_override = strdup(php_optarg);
				break;
			case 'n':
				ini_ignore = 1;
				break;
			case 'd':
				/* define ini entries on command line */
				php_ini_builder_define(&ini_builder, php_optarg);
				break;
#ifndef PHP_CLI_WIN32_NO_CONSOLE
			case 'S':
				sapi_module = &cli_server_sapi_module;
				cli_server_sapi_module.additional_functions = server_additional_functions;
				break;
#endif
			case 'h': /* help & quit */
			case '?':
				php_cli_usage(argv[0]);
				goto out;
			case PHP_GETOPT_INVALID_ARG: /* print usage on bad options, exit 1 */
				php_cli_usage(argv[0]);
				exit_status = 1;
				goto out;
			case 'i': case 'v': case 'm':
				sapi_module = &cli_sapi_module;
				goto exit_loop;
			case 'e': /* enable extended info output */
				use_extended_info = 1;
				break;
		}
	}
exit_loop:

	sapi_module->ini_defaults = sapi_cli_ini_defaults;
	sapi_module->php_ini_path_override = ini_path_override;
	sapi_module->phpinfo_as_text = 1;
	sapi_module->php_ini_ignore_cwd = 1;
	sapi_startup(sapi_module);
	sapi_started = 1;

	sapi_module->php_ini_ignore = ini_ignore;

	sapi_module->executable_location = argv[0];

	if (sapi_module == &cli_sapi_module) {
		php_ini_builder_prepend_literal(&ini_builder, HARDCODED_INI);
	}

	sapi_module->ini_entries = php_ini_builder_finish(&ini_builder);

	/* startup after we get the above ini override se we get things right */
	if (sapi_module->startup(sapi_module) == FAILURE
			|| sapi_module->child_startup(sapi_module) == FAILURE) {
		/* there is no way to see if we must call zend_ini_deactivate()
		 * since we cannot check if EG(ini_directives) has been initialized
		 * because the executor's constructor does not set initialize it.
		 * Apart from that there seems no need for zend_ini_deactivate() yet.
		 * So we goto out_err.*/
		exit_status = 1;
		goto out;
	}
	module_started = 1;

#if defined(PHP_WIN32)
	php_win32_cp_cli_setup();
	orig_cp = (php_win32_cp_get_orig())->id;
	/* Ignore the delivered argv and argc, read from W API. This place
		might be too late though, but this is the earliest place ATW
		we can access the internal charset information from PHP. */
	argv_wide = CommandLineToArgvW(GetCommandLineW(), &num_args);
	PHP_WIN32_CP_W_TO_ANY_ARRAY(argv_wide, num_args, argv, argc)
	using_wide_argv = 1;

	SetConsoleCtrlHandler(php_cli_win32_ctrl_handler, TRUE);
#endif

	/* -e option */
	if (use_extended_info) {
		CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;
	}

	zend_first_try {
#ifndef PHP_CLI_WIN32_NO_CONSOLE
		if (sapi_module == &cli_sapi_module) {
#endif
			exit_status = do_cli(argc, argv);
#ifndef PHP_CLI_WIN32_NO_CONSOLE
		} else {
			exit_status = do_cli_server(argc, argv);
		}
#endif
	} zend_end_try();
out:
	if (ini_path_override) {
		free(ini_path_override);
	}
	php_ini_builder_deinit(&ini_builder);
	if (module_started) {
		php_module_shutdown();
	}
	if (sapi_started) {
		sapi_shutdown();
	}
#ifdef ZTS
	tsrm_shutdown();
#endif

#if defined(PHP_WIN32)
	(void)php_win32_cp_cli_restore();

	if (using_wide_argv) {
		PHP_WIN32_CP_FREE_ARRAY(argv, argc);
		LocalFree(argv_wide);
	}
	argv = argv_save;
#endif
	/*
	 * Do not move this de-initialization. It needs to happen right before
	 * exiting.
	 */
	cleanup_ps_args(argv);
	exit(exit_status);
}
/* }}} */
