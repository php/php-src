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
   | Author: Harald Radi <phanto@php.net                                  |
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

#ifdef __riscos__
#include <unixlib/local.h>
#endif

#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_highlight.h"
#include "zend_indent.h"

#include "libmilter/mfapi.h"

#define OPTSTRING "ac:d:Def:hnp:vVz:?"
#define MG(v) TSRMG(milter_globals_id, milter_globals *, v)

extern char *ap_php_optarg;
extern int ap_php_optind;

/*********************
 * globals
 */
static int milter_globals_id;
static int flag_debug=0;
static zend_file_handle file_handle;

/* per thread */
typedef struct _milter_globals {
	SMFICTX *ctx;
} milter_globals;

/*********************
 * Milter callbacks
 */

/* connection info filter */
static sfsistat	mlfi_connect(SMFICTX *ctx, char *hostname, _SOCK_ADDR *hostaddr)
{
	zval *function_name, *retval, *host, **params[1];
	TSRMLS_FETCH();

	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;

	if (php_request_startup(TSRMLS_C)==FAILURE) {
		SG(headers_sent) = 1;
		SG(request_info).no_headers = 1;
		php_request_shutdown((void *) 0);

		return SMFIS_TEMPFAIL;
	}
	
	SG(headers_sent) = 1;
	SG(request_info).no_headers = 1;
		
	php_execute_script(&file_handle TSRMLS_CC);

	/* call userland */
	ZVAL_INIT(function_name);
	ZVAL_INIT(host);

	ZVAL_STRING(function_name, "milter_connect", 1);
	ZVAL_STRING(host, hostname, 1);

	params[0] = &host;

	call_user_function(CG(function_table), NULL, function_name, retval, 1, params TSRMLS_CC);

	if (Z_TYPE_P(retval) == IS_LONG) {
		return Z_LONG_P(retval);
	} else {
		return SMFIS_CONTINUE;
	}
}

/* SMTP HELO command filter */
static sfsistat mlfi_helo(SMFICTX *ctx, char *helohost)
{
	zval *function_name, *retval, *host, **params[1];
	TSRMLS_FETCH();

	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	
	/* call userland */
	ZVAL_INIT(function_name);
	ZVAL_INIT(host);

	ZVAL_STRING(function_name, "milter_helo", 1);
	ZVAL_STRING(host, helohost, 1);

	params[0] = &host;
	
	call_user_function(CG(function_table), NULL, function_name, retval, 1, params TSRMLS_CC);

	if (Z_TYPE_P(retval) == IS_LONG) {
		return Z_LONG_P(retval);
	} else {
		return SMFIS_CONTINUE;
	}
}
	
/* envelope sender filter */
static sfsistat mlfi_envfrom(SMFICTX *ctx, char **argv)
{
	zval *function_name, *retval, *v, **params[1];
	TSRMLS_FETCH();

	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	
	/* call userland */
	ZVAL_INIT(function_name);
	ZVAL_INIT(v);

	ZVAL_STRING(function_name, "milter_envelope_from", 1);
	array_init(v);

	while (*argv) {
		add_next_index_string(v, *argv, 1);
		argv++;
	}

	params[0] = v;

	call_user_function(CG(function_table), NULL, function_name, retval, 1, params TSRMLS_CC);

	if (Z_TYPE_P(retval) == IS_LONG) {
		return Z_LONG_P(retval);
	} else {
		return SMFIS_CONTINUE;
	}
}

/* envelope recipient filter */
static sfsistat mlfi_envrcpt(SMFICTX *ctx, char **argv)
{
	zval *function_name, *retval, *v, **params[1];
	TSRMLS_FETCH();

	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	
	/* call userland */
	ZVAL_INIT(function_name);
	ZVAL_INIT(v);

	ZVAL_STRING(function_name, "milter_envelope_recipient", 1);
	array_init(v);

	while (*argv) {
		add_next_index_string(v, *argv, 1);
		argv++;
	}

	params[0] = v;
	
	call_user_function(CG(function_table), NULL, function_name, retval, 1, params TSRMLS_CC);

	if (Z_TYPE_P(retval) == IS_LONG) {
		return Z_LONG_P(retval);
	} else {
		return SMFIS_CONTINUE;
	}
}

/* header filter */
static sfsistat mlfi_header(SMFICTX *ctx, char *headerf, char *headerv)
{
	zval *function_name, *retval, *f, *v, **params[2];
	TSRMLS_FETCH();

	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	
	/* call userland */
	ZVAL_INIT(function_name);
	ZVAL_INIT(f);
	ZVAL_INIT(v);
	
	ZVAL_STRING(function_name, "milter_header", 1);
	ZVAL_STRING(f, headerf, 1);
	ZVAL_STRING(v, headerv, 1);

	params[0] = f;
	params[1] = v;
	
	call_user_function(CG(function_table), NULL, function_name, retval, 2, params TSRMLS_CC);

	if (Z_TYPE_P(retval) == IS_LONG) {
		return Z_LONG_P(retval);
	} else {
		return SMFIS_CONTINUE;
	}
}

/* end of header */
static sfsistat mlfi_eoh(SMFICTX *ctx)
{
	zval *function_name, *retval;
	TSRMLS_FETCH();

	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	
	/* call userland */
	ZVAL_INIT(function_name);
	ZVAL_STRING(function_name, "milter_eoh", 1);
	call_user_function(CG(function_table), NULL, function_name, retval, 0, NULL TSRMLS_CC);

	if (Z_TYPE_P(retval) == IS_LONG) {
		return Z_LONG_P(retval);
	} else {
		return SMFIS_CONTINUE;
	}
}

/* body block */
static sfsistat mlfi_body(SMFICTX *ctx, u_char *bodyp, size_t len)
{
	zval *function_name, *retval, *p, **params[1];;
	TSRMLS_FETCH();

	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	
	/* call userland */
	ZVAL_INIT(function_name);
	ZVAL_INIT(p);

	ZVAL_STRING(function_name, "milter_body", 1);
	ZVAL_STRINGL(p, bodyp, len, 1);
	
	params[0] = p;
	
	call_user_function(CG(function_table), NULL, function_name, retval, 0, NULL TSRMLS_CC);

	if (Z_TYPE_P(retval) == IS_LONG) {
		return Z_LONG_P(retval);
	} else {
		return SMFIS_CONTINUE;
	}
}

/* end of message */
static sfsistat mlfi_eom(SMFICTX *ctx)
{
	zval *function_name, *retval;
	TSRMLS_FETCH();

	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	
	/* call userland */
	ZVAL_INIT(function_name);
	ZVAL_STRING(function_name, "milter_eom", 1);
	call_user_function(CG(function_table), NULL, function_name, retval, 0, NULL TSRMLS_CC);

	if (Z_TYPE_P(retval) == IS_LONG) {
		return Z_LONG_P(retval);
	} else {
		return SMFIS_CONTINUE;
	}
}

/* message aborted */
static sfsistat mlfi_abort(SMFICTX *ctx)
{
	zval *function_name, *retval;
	TSRMLS_FETCH();

	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	
	/* call userland */
	ZVAL_INIT(function_name);
	ZVAL_STRING(function_name, "milter_abort", 1);
	call_user_function(CG(function_table), NULL, function_name, retval, 0, NULL TSRMLS_CC);

	if (Z_TYPE_P(retval) == IS_LONG) {
		return Z_LONG_P(retval);
	} else {
		return SMFIS_CONTINUE;
	}
}

/* connection cleanup */
static sfsistat mlfi_close(SMFICTX *ctx)
{
	int ret = SMFIS_CONTINUE;
	zval *function_name, *retval;
	TSRMLS_FETCH();

	/* set the milter context for possible use in API functions */
	MG(ctx) = ctx;
	
	/* call userland */
	ZVAL_INIT(function_name);
	ZVAL_STRING(function_name, "milter_close", 1);
	call_user_function(CG(function_table), NULL, function_name, retval, 0, NULL TSRMLS_CC);

	if (Z_TYPE_P(retval) == IS_LONG) {
		ret = Z_LONG_P(retval);
	}
	
	php_request_shutdown((void *) 0);
	return ret;
}

/*********************
 * Milter entry struct
 */
struct smfiDesc smfilter = {
    "php-milter",	/* filter name */
    SMFI_VERSION,   /* version code -- leave untouched */
    SMFIF_ADDHDRS|SMFIF_CHGHDRS|SMFIF_CHGBODY|SMFIF_ADDRCPT|SMFIF_DELRCPT,  /* flags */
    mlfi_connect,	/* info filter callback */
    mlfi_helo,		/* HELO filter callback */
    mlfi_envfrom,	/* envelope filter callback */
    mlfi_envrcpt,	/* envelope recipient filter callback */
    mlfi_header,	/* header filter callback */
    mlfi_eoh,		/* end of header callback */
    mlfi_body,		/* body filter callback */
    mlfi_eom,		/* end of message callback */
    mlfi_abort,		/* message aborted callback */
    mlfi_close,		/* connection cleanup callback */
};

/*********************
 * PHP Milter API
 */
PHP_FUNCTION(smfi_getsymval)
{
	char *symname, *ret;
	int len;

    if (zend_parse_parameters(1 TSRMLS_CC, "s", &symname, &len) == SUCCESS) {
		if ((ret = smfi_getsymval(MG(ctx), symname)) != NULL) {
			RETVAL_STRING(ret);
		}
	}

	RETVAL_NULL();
}

PHP_FUNCTION(smfi_setreply)
{
	char *rcode, *xcode, *message;
	int len;
	
	if (zend_parse_parameters(3 TSRMLS_CC, "sss", &rcode, &len, &xcode, &len, &message, &len) == SUCCESS) {
		if (smfi_setreply(MG(ctx), rcode, xcode, message) == MI_SUCCESS) {
			RETVAL_TRUE();
		}
	}
	
	RETVAL_FALSE();
}

PHP_FUNCTION(smfi_addheader)
{
	char *f, *v;
	int len;
	
	if (zend_parse_parameters(2 TSRMLS_CC, "ss", &f, &len, &v, &len) == SUCCESS) {
		if (smfi_addheader(MG(ctx), f, v) == MI_SUCCESS) {
			RETVAL_TRUE();
		}
	}
	
	RETVAL_FALSE();
}

PHP_FUNCTION(smfi_chgheader)
{
	char *f, *v;
	long idx;
	int len;
	
	if (zend_parse_parameters(3 TSRMLS_CC, "sls", &f, &len, &idx, &v, &len) == SUCCESS) {
		if (smfi_chgheader(MG(ctx), f, idx, v) == MI_SUCCESS) {
			RETVAL_TRUE();
		}
	}
	
	RETVAL_FALSE();
}

PHP_FUNCTION(smfi_addrcpt)
{
	char *rcpt;
	int len;
	
	if (zend_parse_parameters(1 TSRMLS_CC, "s", &rcpt, &len) == SUCCESS) {
		if (smfi_addrcpt(MG(ctx), rcpt) == MI_SUCCESS) {
			RETVAL_TRUE();
		}
	}
	
	RETVAL_FALSE();
}

PHP_FUNCTION(smfi_delrcpt)
{
	char *rcpt;
	int len;
	
	if (zend_parse_parameters(1 TSRMLS_CC, "s", &rcpt, &len) == SUCCESS) {
		if (smfi_delrcpt(MG(ctx), rcpt) == MI_SUCCESS) {
			RETVAL_TRUE();
		}
	}
	
	RETVAL_FALSE();
}

PHP_FUNCTION(smfi_replacebody)
{
	char *body;
	int len;
	
	if (zend_parse_parameters(1 TSRMLS_CC, "s", &rcpt, &len) == SUCCESS) {
		if (smfi_replacebody(MG(ctx), body, len) == MI_SUCCESS) {
			RETVAL_TRUE();
		}
	}
	
	RETVAL_FALSE();
}

PHP_MINIT_FUNCTION(milter)
{
	REGISTER_LONG_CONSTANT("SMFIS_CONTINUE",	SMFIS_CONTINUE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SMFIS_REJECT",		SMFIS_REJECT,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SMFIS_DISCARD",		SMFIS_DISCARD,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SMFIS_ACCEPT",		SMFIS_ACCEPT,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SMFIS_TEMPFAIL",	SMFIS_TEMPFAIL,	CONST_CS | CONST_PERSISTENT);
}

PHP_MINFO_FUNCTION(milter)
{
	DISPLAY_INI_ENTRIES();
}


/**

/*********************
 * Milter function entry
 */
/* {{{ function_entry */
static function_entry milter_functions[] = {
	PHP_FE(smfi_getsymval, NULL)
	PHP_FE(smfi_setreply, NULL)
	PHP_FE(smfi_addheader, NULL)
	PHP_FE(smfi_chgheader, NULL)
	PHP_FE(smfi_addrcpt, NULL)
	PHP_FE(smfi_delrcpt, NULL)
	PHP_FE(smfi_replacebody, NULL)
	{ NULL, NULL, NULL }
};
/* }}} */

/*********************
 * Milter module entry
 */
/* {{{ zend_module_entry */
static zend_module_entry php_milter_module = {
	STANDARD_MODULE_HEADER,
	"Milter",
	milter_functions,
	PHP_MINIT(milter),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(milter),
	"0.1.0",
	STANDARD_MODULE_PROPERTIES
};

/*********************
 * Milter SAPI
 */

static int sapi_milter_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	return str_length;
}

static void sapi_milter_flush(void *server_context)
{
}

static void sapi_milter_register_variables(zval *track_vars_array TSRMLS_DC)
{
	php_register_variable ("SERVER_SOFTWARE", "Sendmail Milter", track_vars_array TSRMLS_CC);
}

static int sapi_milter_post_read(char *buf, uint count_bytes TSRMLS_DC)
{
	return 0;
}

static char* sapi_milter_read_cookies(TSRMLS_D)
{
	return NULL;
}

static int sapi_milter_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}


static int php_milter_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, &php_milter_module, 1) == FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}


/* {{{ sapi_module_struct milter_sapi_module
 */
static sapi_module_struct milter_sapi_module = {
	"milter",						/* name */
	"Sendmail Milter SAPI",			/* pretty name */

	php_milter_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	NULL,							/* activate */
	NULL,							/* deactivate */

	sapi_milter_ub_write,			/* unbuffered write */
	sapi_milter_flush,				/* flush */
	NULL,							/* get uid */
	NULL,							/* getenv */

	php_error,						/* error handler */

	NULL,							/* header handler */
	sapi_milter_send_headers,		/* send headers handler */
	NULL,							/* send header handler */

	sapi_milter_post_read,			/* read POST data */
	sapi_milter_read_cookies,		/* read Cookies */

	sapi_milter_register_variables,	/* register server variables */
	NULL,							/* Log message */

	NULL,							/* Block interruptions */
	NULL,							/* Unblock interruptions */

	STANDARD_SAPI_MODULE_PROPERTIES
};
/* }}} */

/* {{{ php_milter_usage
 */
static void php_milter_usage(char *argv0)
{
	char *prog;

	prog = strrchr(argv0, '/');
	if (prog) {
		prog++;
	} else {
		prog = "php-milter";
	}

	printf(     "Usage: %s [options] [-f] <file> [args...]\n"
	            "       %s [options] -r <code> [args...]\n"
	            "       %s [options] [-- args...]\n"
				"  -a               Run interactively\n"
				"  -c <path>|<file> Look for php.ini file in this directory\n"
				"  -n               No php.ini file will be used\n"
				"  -d foo[=bar]     Define INI entry foo with value 'bar'\n"
				"  -D               run as daemon\n"
				"  -e               Generate extended information for debugger/profiler\n"
				"  -f <file>        Parse <file>.\n"
				"  -h               This help\n"
				"  -p               path to create socket\n"
				"  -v               Version number\n"
				"  -V <n>           set debug level to n (1 or 2).\n"
				"  -z <file>        Load Zend extension <file>.\n"
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

/* {{{ main
 */
int main(int argc, char *argv[])
{
    char *sock = NULL;
	int dofork = 0;

	int exit_status = SUCCESS;
	int c;
/* temporary locals */
	int orig_optind=ap_php_optind;
	char *orig_optarg=ap_php_optarg;
	char *arg_free=NULL, **arg_excp=&arg_free;
	char *script_file=NULL;
	int interactive=0;
	char *exec_direct=NULL;
	char *param_error=NULL;
/* end of temporary locals */

	void ***tsrm_ls;


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


	tsrm_startup(1, 1, 0, NULL);
	sapi_startup(&milter_sapi_module);
	
	while ((c=ap_php_getopt(argc, argv, OPTSTRING))!=-1) {
		switch (c) {
		case 'c':
			milter_sapi_module.php_ini_path_override = strdup(ap_php_optarg);
			break;
		case 'n':
			milter_sapi_module.php_ini_ignore = 1;
			break;
		}
	}
	ap_php_optind = orig_optind;
	ap_php_optarg = orig_optarg;

	milter_sapi_module.executable_location = argv[0];

	sapi_module.startup(&milter_sapi_module);

	tsrm_ls = ts_resource(0);

	zend_first_try {
		while ((c=ap_php_getopt(argc, argv, OPTSTRING))!=-1) {
			switch (c) {
			case '?':
				php_output_startup();
				php_output_activate(TSRMLS_C);
				SG(headers_sent) = 1;
				php_milter_usage(argv[0]);
				php_end_ob_buffers(1 TSRMLS_CC);
				exit(1);
				break;
			}
		}
		ap_php_optind = orig_optind;
		ap_php_optarg = orig_optarg;

        /* Set some CLI defaults */
		SG(options) |= SAPI_OPTION_NO_CHDIR;
		zend_alter_ini_entry("html_errors", 12, "0", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
		zend_alter_ini_entry("max_execution_time", 19, "0", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);

		zend_uv.html_errors = 0; /* tell the engine we're in non-html mode */

		if (milter_sapi_module.php_ini_path_override && milter_sapi_module.php_ini_ignore) {
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

			case 'D': /* daemon */
				dofork = 1;
				break;

			case 'e': /* enable extended info output */
				CG(extended_info) = 1;
				break;

			case 'f': /* parse file */
				script_file = ap_php_optarg;
				break;

			case 'h': /* help & quit */
			case '?':
				php_output_startup();
				php_output_activate(TSRMLS_C);
				SG(headers_sent) = 1;
				php_milter_usage(argv[0]);
				php_end_ob_buffers(1 TSRMLS_CC);
				exit(1);
				break;

			case 'p': /* socket */
				sock = strdup(optarg);
				break;

			case 'v': /* show php version & quit */
				if (php_request_startup(TSRMLS_C)==FAILURE) {
					zend_ini_deactivate(TSRMLS_C);
					php_module_shutdown(TSRMLS_C);
					sapi_shutdown();
					tsrm_shutdown();

					exit(1);
				}
				SG(headers_sent) = 1;
				SG(request_info).no_headers = 1;
				php_printf("PHP %s (%s) (built: %s %s)\nCopyright (c) 1997-2002 The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
				php_end_ob_buffers(1 TSRMLS_CC);
				exit(1);
				break;

			case 'V': /* verbose */
				flag_debug = atoi(optarg);
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
		if (argc > ap_php_optind && !script_file && strcmp(argv[ap_php_optind-1],"--")) {
			script_file=argv[ap_php_optind];
			ap_php_optind++;
		}
		if (script_file) {
			if (!(file_handle.handle.fp = VCWD_FOPEN(script_file, "rb"))) {
				SG(headers_sent) = 1;
				SG(request_info).no_headers = 1;
				php_printf("Could not open input file: %s.\n", script_file);
				
				zend_ini_deactivate(TSRMLS_C);
				php_module_shutdown(TSRMLS_C);
				sapi_shutdown();
				tsrm_shutdown();

				exit(1);
			}
			file_handle.filename = script_file;
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

		/* before registering argv to modulule exchange the *new* argv[0] */
		/* we can achieve this without allocating more memory */
		SG(request_info).argc=argc-ap_php_optind+1;
		arg_excp = argv+ap_php_optind-1;
		arg_free = argv[ap_php_optind-1];
		SG(request_info).path_translated = file_handle.filename;
		argv[ap_php_optind-1] = file_handle.filename;
		SG(request_info).argv=argv+ap_php_optind-1;

		/* TSRM is used to allocate a per-thread structure */
		ts_allocate_id (&milter_globals_id, sizeof(milter_globals), NULL, NULL);

		if (dofork) {
			switch(fork()) {
				case -1: /* Uh-oh, we have a problem forking. */
					fprintf(stderr, "Uh-oh, couldn't fork!\n");
					exit(errno);
					break;
				case 0: /* Child */
					break;
				default: /* Parent */
					exit(0);
			}
		}
			
		{
			struct stat junk;
			if (stat(sock,&junk) == 0) unlink(sock);
		}

		openlog("php-milter", LOG_PID, LOG_MAIL);

		(void) smfi_setconn(sock);
		if (smfi_register(smfilter) == MI_FAILURE) {
			fprintf(stderr, "smfi_register failed\n");
//				exit_status = EX_UNAVAILABLE;
		} else {
			debug(1, "smfi_register succeeded");
			exit_status = smfi_main();
		}			

		if (milter_sapi_module.php_ini_path_override) {
			free(milter_sapi_module.php_ini_path_override);
		}

	} zend_catch {
		exit_status = EG(exit_status);
	} zend_end_try();

	php_module_shutdown(TSRMLS_C);
	sapi_shutdown();
	tsrm_shutdown();

	exit(exit_status);
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
