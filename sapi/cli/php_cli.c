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
   | Author: Edin Kadribasic <edink@php.net>                              |
   |         Marcus Boerger <helly@php.net>                               |
   |         Parts based on CGI SAPI Module by                            |
   |         Rasmus Lerdorf, Stig Bakken and Zeev Suraski                 |
   +----------------------------------------------------------------------+
*/


#include "php.h"
#include "php_globals.h"
#include "php_variables.h"
#include "zend_modules.h"

#include "SAPI.h"

#include <stdio.h>
#include "php.h"
#ifdef PHP_WIN32
#include "win32/time.h"
#include "win32/signal.h"
#include <process.h>
#endif
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_SIGNAL_H
#include <signal.h>
#endif
#if HAVE_SETLOCALE
#include <locale.h>
#endif
#include "zend.h"
#include "zend_extensions.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_main.h"
#include "fopen_wrappers.h"
#include "ext/standard/php_standard.h"
#ifdef PHP_WIN32
#include <io.h>
#include <fcntl.h>
#include "win32/php_registry.h"
#endif

#if HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef __riscos__
#include <unixlib/local.h>
#endif

#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_highlight.h"
#include "zend_indent.h"


#include "php_getopt.h"

#define PHP_MODE_STANDARD    1
#define PHP_MODE_HIGHLIGHT   2
#define PHP_MODE_INDENT      3
#define PHP_MODE_LINT        4
#define PHP_MODE_STRIP       5
#define PHP_MODE_CLI_DIRECT  6

extern char *ap_php_optarg;
extern int ap_php_optind;

#define OPTSTRING "aCc:d:ef:g:hilmnqr:sw?vz:"

static int _print_module_info(zend_module_entry *module, void *arg TSRMLS_DC)
{
	php_printf("%s\n", module->name);
	return 0;
}

static int _print_extension_info(zend_extension *module, void *arg TSRMLS_DC)
{
	php_printf("%s\n", module->name);
	return 0;
}

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

static inline size_t sapi_cli_single_write(const char *str, uint str_length)
{
#ifdef PHP_WRITE_STDOUT
	long ret;

	ret = write(STDOUT_FILENO, str, str_length);
	if (ret <= 0) {
		return 0;
	}
	return ret;
#else
	size_t ret;

	ret = fwrite(str, 1, MIN(str_length, 16384), stdout);
	return ret;
#endif
}

static int sapi_cli_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	const char *ptr = str;
	uint remaining = str_length;
	size_t ret;

	while (remaining > 0)
	{
		ret = sapi_cli_single_write(ptr, remaining);
		if (!ret) {
			php_handle_aborted_connection();
		}
		ptr += ret;
		remaining -= ret;
	}

	return str_length;
}


static void sapi_cli_flush(void *server_context)
{
	if (fflush(stdout)==EOF) {
		php_handle_aborted_connection();
	}
}

static char *php_self = "";
static char *script_filename = "";

static void sapi_cli_register_variables(zval *track_vars_array TSRMLS_DC)
{
	/* In CGI mode, we consider the environment to be a part of the server
	 * variables
	 */
	php_import_environment_variables(track_vars_array TSRMLS_CC);

	/* Build the special-case PHP_SELF variable for the CLI version */
	php_register_variable("PHP_SELF", php_self, track_vars_array TSRMLS_CC);
	php_register_variable("SCRIPT_NAME", php_self, track_vars_array TSRMLS_CC);
	/* filenames are empty for stdin */
	php_register_variable("SCRIPT_FILENAME", script_filename, track_vars_array TSRMLS_CC);
	php_register_variable("PATH_TRANSLATED", script_filename, track_vars_array TSRMLS_CC);
	/* just make it available */
	php_register_variable("DOCUMENT_ROOT", "", track_vars_array TSRMLS_CC);
}


static void sapi_cli_log_message(char *message)
{
	if (php_header()) {
		fprintf(stderr, "%s", message);
		fprintf(stderr, "\n");
	}
}

static int sapi_cli_deactivate(TSRMLS_D)
{
	fflush(stdout);
	if(SG(request_info).argv0) {
		free(SG(request_info).argv0);
		SG(request_info).argv0 = NULL;
	}
	return SUCCESS;
}

static char* sapi_cli_read_cookies(TSRMLS_D)
{
	return NULL;
}

static void sapi_cli_send_header(sapi_header_struct *sapi_header, void *server_context TSRMLS_DC)
{
	if (sapi_header) {
		PHPWRITE_H(sapi_header->header, sapi_header->header_len);
	}
	PHPWRITE_H("\r\n", 2);
}


static int php_cli_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, NULL, 0)==FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}


/* {{{ sapi_module_struct cli_sapi_module
 */
static sapi_module_struct cli_sapi_module = {
	"cli",							/* name */
	"Command Line Interface",    	/* pretty name */

	php_cli_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	NULL,							/* activate */
	sapi_cli_deactivate,			/* deactivate */

	sapi_cli_ub_write,		    	/* unbuffered write */
	sapi_cli_flush,				    /* flush */
	NULL,							/* get uid */
	NULL,							/* getenv */

	php_error,						/* error handler */

	NULL,							/* header handler */
	NULL,							/* send headers handler */
	sapi_cli_send_header,			/* send header handler */

	NULL,				            /* read POST data */
	sapi_cli_read_cookies,          /* read Cookies */

	sapi_cli_register_variables,	/* register server variables */
	sapi_cli_log_message,			/* Log message */

	NULL,							/* Block interruptions */
	NULL,							/* Unblock interruptions */

	STANDARD_SAPI_MODULE_PROPERTIES
};
/* }}} */

/* {{{ php_cli_usage
 */
static void php_cli_usage(char *argv0)
{
	char *prog;

	prog = strrchr(argv0, '/');
	if (prog) {
		prog++;
	} else {
		prog = "php";
	}
	
	php_printf( "Usage: %s [options] [-f] <file> [args...]\n"
	            "       %s [options] -r <code> [args...]\n"
	            "       %s [options] [-- args...]\n"
				"  -a               Run interactively\n"
				"  -c <path>|<file> Look for php.ini file in this directory\n"
				"  -n               No php.ini file will be used\n"
				"  -d foo[=bar]     Define INI entry foo with value 'bar'\n"
				"  -e               Generate extended information for debugger/profiler\n"
				"  -f <file>        Parse <file>.\n"
				"  -h               This help\n"
				"  -i               PHP information\n"
				"  -l               Syntax check only (lint)\n"
				"  -m               Show compiled in modules\n"
				"  -r <code>        Run PHP <code> without using script tags <?..?>\n"
				"  -s               Display colour syntax highlighted source.\n"
				"  -v               Version number\n"
				"  -w               Display source with stripped comments and whitespace.\n"
				"  -z <file>        Load Zend extension <file>.\n"
				"\n"
				"  args...          Arguments passed to script. Use -- args when first argument \n"
				"                   starts with - or script is read from stdin\n"
				, prog, prog, prog);
}
/* }}} */

static void define_command_line_ini_entry(char *arg)
{
	char *name, *value;

	name = arg;
	value = strchr(arg, '=');
	if (value) {
		*value = 0;
		value++;
	} else {
		value = "1";
	}
	zend_alter_ini_entry(name, strlen(name)+1, value, strlen(value), PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
}


static void php_register_command_line_global_vars(char **arg TSRMLS_DC)
{
	char *var, *val;

	var = *arg;
	val = strchr(var, '=');
	if (!val) {
		printf("No value specified for variable '%s'\n", var);
	} else {
		*val++ = '\0';
		php_register_variable(var, val, NULL TSRMLS_CC);
	}
	efree(*arg);
}

static void cli_register_file_handles(TSRMLS_D)
{
	zval *zin, *zout, *zerr;
	php_stream *s_in, *s_out, *s_err;
	php_stream_context *sc_in=NULL, *sc_out=NULL, *sc_err=NULL;
	zend_constant ic, oc, ec;
	
	MAKE_STD_ZVAL(zin);
	MAKE_STD_ZVAL(zout);
	MAKE_STD_ZVAL(zerr);

	s_in  = php_stream_open_wrapper_ex("php://stdin",  "rb", 0, NULL, sc_in);
	s_out = php_stream_open_wrapper_ex("php://stdout", "wb", 0, NULL, sc_out);
	s_err = php_stream_open_wrapper_ex("php://stderr", "wb", 0, NULL, sc_err);

	if (s_in==NULL || s_out==NULL || s_err==NULL) {
		return;
	}

	php_stream_to_zval(s_in,  zin);
	php_stream_to_zval(s_out, zout);
	php_stream_to_zval(s_err, zerr);
	
	ic.value = *zin;
	zval_copy_ctor(&ic.value);
	ic.flags = CONST_CS | CONST_PERSISTENT;
	ic.name = zend_strndup("STDIN", 6);
	ic.name_len = 6;
	zend_register_constant(&ic TSRMLS_CC);

	oc.value = *zout;
	zval_copy_ctor(&oc.value);
	oc.flags = CONST_CS | CONST_PERSISTENT;
	oc.name = zend_strndup("STDOUT", 7);
	oc.name_len = 7;
	zend_register_constant(&oc TSRMLS_CC);

	ec.value = *zerr;
	zval_copy_ctor(&ec.value);
	ec.flags = CONST_CS | CONST_PERSISTENT;
	ec.name = zend_strndup("STDERR", 7);
	ec.name_len = 7;
	zend_register_constant(&ec TSRMLS_CC);

	FREE_ZVAL(zin);
	FREE_ZVAL(zout);
	FREE_ZVAL(zerr);
}

/* {{{ main
 */
int main(int argc, char *argv[])
{
	int exit_status = SUCCESS;
	int c;
	zend_file_handle file_handle;
/* temporary locals */
	int behavior=PHP_MODE_STANDARD;
	int no_headers=1;
	int orig_optind=ap_php_optind;
	char *orig_optarg=ap_php_optarg;
	char *arg_free=NULL, **arg_excp=&arg_free;
	char *script_file=NULL;
	zend_llist global_vars;
	int interactive=0;
	int module_started = 0;
	char *exec_direct=NULL;
	char *param_error=NULL;
/* end of temporary locals */
#ifdef ZTS
	zend_compiler_globals *compiler_globals;
	zend_executor_globals *executor_globals;
	php_core_globals *core_globals;
	sapi_globals_struct *sapi_globals;
	void ***tsrm_ls;
#endif


#ifdef HAVE_SIGNAL_H
#if defined(SIGPIPE) && defined(SIG_IGN)
	signal(SIGPIPE, SIG_IGN); /* ignore SIGPIPE in standalone mode so
								that sockets created via fsockopen()
								don't kill PHP if the remote site
								closes it.  in apache|apxs mode apache
								does that for us!  thies@thieso.net
								20000419 */
#endif
#endif


#ifdef ZTS
	tsrm_startup(1, 1, 0, NULL);
#endif

	sapi_startup(&cli_sapi_module);

#ifdef PHP_WIN32
	_fmode = _O_BINARY;			/*sets default for file streams to binary */
	setmode(_fileno(stdin), O_BINARY);		/* make the stdio mode be binary */
	setmode(_fileno(stdout), O_BINARY);		/* make the stdio mode be binary */
	setmode(_fileno(stderr), O_BINARY);		/* make the stdio mode be binary */
#endif


	while ((c=ap_php_getopt(argc, argv, OPTSTRING))!=-1) {
		switch (c) {
		case 'c':
			cli_sapi_module.php_ini_path_override = strdup(ap_php_optarg);
			break;
		case 'n':
			cli_sapi_module.php_ini_ignore = 1;
			break;
		}
	}
	ap_php_optind = orig_optind;
	ap_php_optarg = orig_optarg;

	cli_sapi_module.executable_location = argv[0];

#ifdef ZTS
	compiler_globals = ts_resource(compiler_globals_id);
	executor_globals = ts_resource(executor_globals_id);
	core_globals = ts_resource(core_globals_id);
	sapi_globals = ts_resource(sapi_globals_id);
	tsrm_ls = ts_resource(0);
#endif

	/* startup after we get the above ini override se we get things right */
	if (php_module_startup(&cli_sapi_module, NULL, 0)==FAILURE) {
		/* there is no way to see if we must call zend_ini_deactivate()
		 * since we cannot check if EG(ini_directives) has been initialised
		 * because the executor's constructor does not set initialize it.
		 * Apart from that there seems no need for zend_ini_deactivate() yet.
		 * So we goto out_err.*/
		exit_status = 1;
		goto out_err;
	}
	module_started = 1;

	zend_first_try {
		while ((c=ap_php_getopt(argc, argv, OPTSTRING))!=-1) {
			switch (c) {
			case '?':
				no_headers = 1;
				php_output_startup();
				php_output_activate(TSRMLS_C);
				SG(headers_sent) = 1;
				php_cli_usage(argv[0]);
				php_end_ob_buffers(1 TSRMLS_CC);
				exit(1);
				break;
			}
		}
		ap_php_optind = orig_optind;
		ap_php_optarg = orig_optarg;

		zend_llist_init(&global_vars, sizeof(char *), NULL, 0);

        /* Set some CLI defaults */
		SG(options) |= SAPI_OPTION_NO_CHDIR;
		zend_alter_ini_entry("register_argc_argv", 19, "1", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
		zend_alter_ini_entry("html_errors", 12, "0", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
		zend_alter_ini_entry("implicit_flush", 15, "1", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
		zend_alter_ini_entry("max_execution_time", 19, "0", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);

		zend_uv.html_errors = 0; /* tell the engine we're in non-html mode */

		if (cli_sapi_module.php_ini_path_override && cli_sapi_module.php_ini_ignore) {
			SG(headers_sent) = 1;
			SG(request_info).no_headers = 1;
			PUTS("You cannot use both -n and -c switch. Use -h for help.\n");
			exit(1);
		}
	
		while ((c = ap_php_getopt(argc, argv, OPTSTRING)) != -1) {
			switch (c) {

			case 'a':	/* interactive mode */
				printf("Interactive mode enabled\n\n");
				interactive=1;
				break;

			case 'C': /* don't chdir to the script directory */
				/* This is default so NOP */
				break;
			case 'd': /* define ini entries on command line */
				define_command_line_ini_entry(ap_php_optarg);
				break;

			case 'e': /* enable extended info output */
				CG(extended_info) = 1;
				break;

			case 'f': /* parse file */
				if (behavior == PHP_MODE_CLI_DIRECT) {
					param_error = "Either execute direct code or use a file.\n";
					break;
				}
				script_file = ap_php_optarg;
				no_headers = 1;
				break;

			case 'g': /* define global variables on command line */
				{
					char *arg = estrdup(ap_php_optarg);

					zend_llist_add_element(&global_vars, &arg);
				}
				break;

			case 'h': /* help & quit */
			case '?':
				no_headers = 1;
				php_output_startup();
				php_output_activate(TSRMLS_C);
				SG(headers_sent) = 1;
				php_cli_usage(argv[0]);
				php_end_ob_buffers(1 TSRMLS_CC);
				exit(1);
				break;

			case 'i': /* php info & quit */
				if (php_request_startup(TSRMLS_C)==FAILURE) {
					goto err;
				}
				if (no_headers) {
					SG(headers_sent) = 1;
					SG(request_info).no_headers = 1;
				}
				php_print_info(0xFFFFFFFF TSRMLS_CC);
				php_end_ob_buffers(1 TSRMLS_CC);
				exit(1);
				break;

			case 'l': /* syntax check mode */
				if (behavior != PHP_MODE_STANDARD) {
					break;
				}
				no_headers = 1;
				behavior=PHP_MODE_LINT;
				break;

			case 'm': /* list compiled in modules */
				php_output_startup();
				php_output_activate(TSRMLS_C);
				SG(headers_sent) = 1;
				php_printf("[PHP Modules]\n");
				zend_hash_apply_with_argument(&module_registry, (apply_func_arg_t) _print_module_info, NULL TSRMLS_CC);
				php_printf("\n[Zend Modules]\n");
				zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) _print_extension_info, NULL TSRMLS_CC);
				php_printf("\n");
				php_end_ob_buffers(1 TSRMLS_CC);
				exit(1);
			break;

#if 0 /* not yet operational, see also below ... */
			case '': /* generate indented source mode*/
				if (behavior == PHP_MODE_CLI_DIRECT) {
					param_error = "Source indenting only works for files.\n";
					break;
				}
				behavior=PHP_MODE_INDENT;
				break;
#endif

			case 'q': /* do not generate HTTP headers */
				/* This is default so NOP */
				break;

			case 'r': /* run code from command line */
				if (behavior != PHP_MODE_STANDARD) {
					param_error = "Either execute direct code or use a file.\n";
					break;
				}
				behavior=PHP_MODE_CLI_DIRECT;
				exec_direct=ap_php_optarg;
				break;

			case 's': /* generate highlighted HTML from source */
				if (behavior == PHP_MODE_CLI_DIRECT) {
					param_error = "Source highlighting only works for files.\n";
					break;
				}
				behavior=PHP_MODE_HIGHLIGHT;
				break;

			case 'v': /* show php version & quit */
				no_headers = 1;
				if (php_request_startup(TSRMLS_C)==FAILURE) {
					goto err;
				}
				if (no_headers) {
					SG(headers_sent) = 1;
					SG(request_info).no_headers = 1;
				}
				php_printf("PHP %s (%s) (built: %s %s)\nCopyright (c) 1997-2002 The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
				php_end_ob_buffers(1 TSRMLS_CC);
				exit(1);
				break;

			case 'w':
				if (behavior == PHP_MODE_CLI_DIRECT) {
					param_error = "Source stripping only works for files.\n";
					break;
				}
				behavior=PHP_MODE_STRIP;
				break;

			case 'z': /* load extension file */
				zend_load_extension(ap_php_optarg);
				break;

			default:
				break;
			}
		}

		if (param_error) {
			SG(headers_sent) = 1;
			SG(request_info).no_headers = 1;
			PUTS(param_error);
			exit(1);
		}

		CG(interactive) = interactive;

		/* only set script_file if not set already and not in direct mode and not at end of parameter list */
		if (argc > ap_php_optind && !script_file && behavior!=PHP_MODE_CLI_DIRECT && strcmp(argv[ap_php_optind-1],"--")) {
			no_headers = 1;
			script_file=argv[ap_php_optind];
			ap_php_optind++;
		}
		if (script_file) {
			if (!(file_handle.handle.fp = VCWD_FOPEN(script_file, "rb"))) {
				SG(headers_sent) = 1;
				SG(request_info).no_headers = 1;
				php_printf("Could not open input file: %s.\n", script_file);
				goto err;
			}
			file_handle.filename = script_file;
			script_filename = script_file;
			/* #!php support */
			c = fgetc(file_handle.handle.fp);
			if (c == '#') {
				while (c != 10 && c != 13) {
					c = fgetc(file_handle.handle.fp);	/* skip to end of line */
				}
				CG(zend_lineno) = -2;
			} else {
				rewind(file_handle.handle.fp);
			}
		} else {
			file_handle.filename = "-";
			file_handle.handle.fp = stdin;
		}
		file_handle.type = ZEND_HANDLE_FP;
		file_handle.opened_path = NULL;
		file_handle.free_filename = 0;
		php_self = file_handle.filename;

		/* before registering argv to modulule exchange the *new* argv[0] */
		/* we can achieve this without allocating more memory */
		SG(request_info).argc=argc-ap_php_optind+1;
		arg_excp = argv+ap_php_optind-1;
		arg_free = argv[ap_php_optind-1];
		SG(request_info).path_translated = file_handle.filename;
		argv[ap_php_optind-1] = file_handle.filename;
		SG(request_info).argv=argv+ap_php_optind-1;

		if (php_request_startup(TSRMLS_C)==FAILURE) {
			*arg_excp = arg_free;
			fclose(file_handle.handle.fp);
			SG(headers_sent) = 1;
			SG(request_info).no_headers = 1;
			php_request_shutdown((void *) 0);
			PUTS("Could not startup.\n");
			goto err;
		}
		*arg_excp = arg_free; /* reconstuct argv */
		if (no_headers) {
			SG(headers_sent) = 1;
			SG(request_info).no_headers = 1;
		}

		/* This actually destructs the elements of the list - ugly hack */
		zend_llist_apply(&global_vars, (llist_apply_func_t) php_register_command_line_global_vars TSRMLS_CC);
		zend_llist_destroy(&global_vars);

		switch (behavior) {
		case PHP_MODE_STANDARD:
			if (strcmp(file_handle.filename, "-")) {
				cli_register_file_handles(TSRMLS_C);
			}
			php_execute_script(&file_handle TSRMLS_CC);
			exit_status = EG(exit_status);
			break;
		case PHP_MODE_LINT:
			PG(during_request_startup) = 0;
			exit_status = php_lint_script(&file_handle TSRMLS_CC);
			if (exit_status==SUCCESS) {
				zend_printf("No syntax errors detected in %s\n", file_handle.filename);
			} else {
				zend_printf("Errors parsing %s\n", file_handle.filename);
			}
			break;
		case PHP_MODE_STRIP:
			if (open_file_for_scanning(&file_handle TSRMLS_CC)==SUCCESS) {
				zend_strip(TSRMLS_C);
				fclose(file_handle.handle.fp);
			}
			goto out;
			break;
		case PHP_MODE_HIGHLIGHT:
			{
				zend_syntax_highlighter_ini syntax_highlighter_ini;

				if (open_file_for_scanning(&file_handle TSRMLS_CC)==SUCCESS) {
					php_get_highlight_struct(&syntax_highlighter_ini);
					zend_highlight(&syntax_highlighter_ini TSRMLS_CC);
					fclose(file_handle.handle.fp);
				}
				goto out;
			}
			break;
#if 0
			/* Zeev might want to do something with this one day */
		case PHP_MODE_INDENT:
			open_file_for_scanning(&file_handle TSRMLS_CC);
			zend_indent();
			fclose(file_handle.handle.fp);
			goto out;
			break;
#endif
		case PHP_MODE_CLI_DIRECT:
			cli_register_file_handles(TSRMLS_C);
			if (zend_eval_string(exec_direct, NULL, "Command line code" TSRMLS_CC) == FAILURE) {
				exit_status=254;
			}
			break;
		}

		if (cli_sapi_module.php_ini_path_override) {
			free(cli_sapi_module.php_ini_path_override);
		}

	} zend_catch {
		exit_status = EG(exit_status);
	} zend_end_try();

out:
	php_request_shutdown((void *) 0);
out_err:	
	if (module_started) {
		php_module_shutdown(TSRMLS_C);
	}
	sapi_shutdown();
#ifdef ZTS
	tsrm_shutdown();
#endif

	exit(exit_status);

err:
	zend_ini_deactivate(TSRMLS_C);
	exit_status = 1;
	goto out_err;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
