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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stig Bakken <ssb@fast.no>                                   |
   |          Zeev Suraski <zeev@zend.com>                                |
   | FastCGI: Ben Mansell <php@slimyhorror.com>                           |
   |          Shane Caraveo <shane@caraveo.com>                           |
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
#else
#include "build-defs.h"
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

#ifdef PHP_FASTCGI
#include "fcgi_config.h"
#include "fcgiapp.h"
/* don't want to include fcgios.h, causes conflicts */
extern int OS_SetImpersonate(void);

FCGX_Stream *in, *out, *err;
FCGX_ParamArray envp;

/* Our original environment from when the FastCGI first started */
char **orig_env;

/* The environment given by the FastCGI */
char **cgi_env;

/* The manufactured environment, from merging the base environ with
 * the parameters set by the per-connection environment
 */
char **merge_env;

/**
 * Number of child processes that will get created to service requests
 */
static int children = 0;

/**
 * Set to non-zero if we are the parent process
 */
static int parent = 1;

/**
 * Process group
 */
static pid_t pgroup;


#endif

#define PHP_MODE_STANDARD	1
#define PHP_MODE_HIGHLIGHT	2
#define PHP_MODE_INDENT		3
#define PHP_MODE_LINT		4
#define PHP_MODE_STRIP		5

extern char *ap_php_optarg;
extern int ap_php_optind;

#define OPTSTRING "aCc:d:ef:g:hilmnqsw?vz:"

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

static inline size_t sapi_cgibin_single_write(const char *str, uint str_length)
{
#ifdef PHP_WRITE_STDOUT
	long ret;
#else
	size_t ret;
#endif

#ifdef PHP_FASTCGI
	if (!FCGX_IsCGI()) {
		return FCGX_PutStr( str, str_length, out );
	}
#endif
#ifdef PHP_WRITE_STDOUT
	ret = write(STDOUT_FILENO, str, str_length);
	if (ret <= 0) return 0;
	return ret;
#else
	ret = fwrite(str, 1, MIN(str_length, 16384), stdout);
	return ret;
#endif
}

static int sapi_cgibin_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	const char *ptr = str;
	uint remaining = str_length;
	size_t ret;

	while (remaining > 0)
	{
		ret = sapi_cgibin_single_write(ptr, remaining);
		if (!ret) {
			php_handle_aborted_connection();
		}
		ptr += ret;
		remaining -= ret;
	}

	return str_length;
}


static void sapi_cgibin_flush(void *server_context)
{
#ifdef PHP_FASTCGI
	if (!FCGX_IsCGI()) {
		if( FCGX_FFlush( out ) == -1 ) {
			php_handle_aborted_connection();
		}
	} else
#endif
	if (fflush(stdout)==EOF) {
		php_handle_aborted_connection();
	}
}


static int sapi_cgi_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	char buf[1024];
	int len;
	sapi_header_struct *h;
	zend_llist_position pos;
	
	len = sprintf(buf, "Status: %d\r\n", SG(sapi_headers).http_response_code);
	PHPWRITE_H(buf, len);

	if (SG(sapi_headers).send_default_content_type) {
		char *hd;

		hd = sapi_get_default_content_type(TSRMLS_C);
		PHPWRITE_H("Content-type: ", sizeof("Content-type: ")-1);
		PHPWRITE_H(hd, strlen(hd));
		PHPWRITE_H("\r\n", 2);
		efree(hd);
	}
	
	h = zend_llist_get_first_ex(&sapi_headers->headers, &pos);
    while (h) {
		PHPWRITE_H(h->header, h->header_len);
		PHPWRITE_H("\r\n", 2);
		h = zend_llist_get_next_ex(&sapi_headers->headers, &pos);
	}
	PHPWRITE_H("\r\n", 2);

	return SAPI_HEADER_SENT_SUCCESSFULLY;
}


static int sapi_cgi_read_post(char *buffer, uint count_bytes TSRMLS_DC)
{
	uint read_bytes=0, tmp_read_bytes;
#ifdef PHP_FASTCGI
	char *pos = buffer;
#endif

	count_bytes = MIN(count_bytes, (uint)SG(request_info).content_length-SG(read_post_bytes));
	while (read_bytes < count_bytes) {
#ifdef PHP_FASTCGI
		if (!FCGX_IsCGI()) {
			tmp_read_bytes = FCGX_GetStr( pos, count_bytes-read_bytes, in );
			pos += tmp_read_bytes;
		} else
#endif
			tmp_read_bytes = read(0, buffer+read_bytes, count_bytes-read_bytes);

		if (tmp_read_bytes<=0) {
			break;
		}
		read_bytes += tmp_read_bytes;
	}
	return read_bytes;
}


static char *sapi_cgi_read_cookies(TSRMLS_D)
{
	return getenv("HTTP_COOKIE");
}


static void sapi_cgi_register_variables(zval *track_vars_array TSRMLS_DC)
{
	/* In CGI mode, we consider the environment to be a part of the server
	 * variables
	 */
	php_import_environment_variables(track_vars_array TSRMLS_CC);

	/* Build the special-case PHP_SELF variable for the CGI version */
	php_register_variable("PHP_SELF", (SG(request_info).request_uri ? SG(request_info).request_uri:""), track_vars_array TSRMLS_CC);
}


static void sapi_cgi_log_message(char *message)
{
	if (php_header()) {
		fprintf(stderr, "%s", message);
		fprintf(stderr, "\n");
	}
}

static int sapi_cgi_deactivate(TSRMLS_D)
{
	fflush(stdout);
	if(SG(request_info).argv0) {
		free(SG(request_info).argv0);
		SG(request_info).argv0 = NULL;
	}
	return SUCCESS;
}


static int php_cgi_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, NULL, 0)==FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}


/* {{{ sapi_module_struct cgi_sapi_module
 */
static sapi_module_struct cgi_sapi_module = {
	"cgi",							/* name */
#ifdef PHP_FASTCGI
	"CGI/FastCGI",					/* pretty name */
#else
	"CGI",							/* pretty name */
#endif
	
	php_cgi_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	NULL,							/* activate */
	sapi_cgi_deactivate,			/* deactivate */

	sapi_cgibin_ub_write,			/* unbuffered write */
	sapi_cgibin_flush,				/* flush */
	NULL,							/* get uid */
	NULL,							/* getenv */

	php_error,						/* error handler */

	NULL,							/* header handler */
	sapi_cgi_send_headers,			/* send headers handler */
	NULL,							/* send header handler */

	sapi_cgi_read_post,				/* read POST data */
	sapi_cgi_read_cookies,			/* read Cookies */

	sapi_cgi_register_variables,	/* register server variables */
	sapi_cgi_log_message,			/* Log message */

	NULL,							/* Block interruptions */
	NULL,							/* Unblock interruptions */

	STANDARD_SAPI_MODULE_PROPERTIES
};
/* }}} */

/* {{{ php_cgi_usage
 */
static void php_cgi_usage(char *argv0)
{
	char *prog;

	prog = strrchr(argv0, '/');
	if (prog) {
		prog++;
	} else {
		prog = "php";
	}

	php_printf("Usage: %s [-q] [-h] [-s [-v] [-i] [-f <file>] |  {<file> [args...]}\n"
			   "  -q               Quiet-mode.  Suppress HTTP Header output.\n"
			   "  -s               Display colour syntax highlighted source.\n"
			   "  -w               Display source with stripped comments and whitespace.\n"
			   "  -f <file>        Parse <file>.  Implies `-q'\n"
			   "  -v               Version number\n"
			   "  -C               Do not chdir to the script's directory\n"
			   "  -c <path>|<file> Look for php.ini file in this directory\n"
			   "  -a               Run interactively\n"
			   "  -d foo[=bar]     Define INI entry foo with value 'bar'\n"
			   "  -e               Generate extended information for debugger/profiler\n"
			   "  -z <file>        Load Zend extension <file>.\n"
			   "  -l               Syntax check only (lint)\n"
			   "  -m               Show compiled in modules\n"
			   "  -i               PHP information\n"
			   "  -h               This help\n", prog);
}
/* }}} */

/* {{{ init_request_info
 */
static void init_request_info(TSRMLS_D)
{
	char *content_length = getenv("CONTENT_LENGTH");
	char *content_type = getenv("CONTENT_TYPE");
	const char *auth;

#if 0
/* SG(request_info).path_translated is always set to NULL at the end of this function
   call so why the hell did this code exist in the first place? Am I missing something? */
	char *script_filename;


	script_filename = getenv("SCRIPT_FILENAME");
	/* Hack for annoying servers that do not set SCRIPT_FILENAME for us */
	if (!script_filename) {
		script_filename = SG(request_info).argv0;
	}
#ifdef PHP_WIN32
	/* FIXME WHEN APACHE NT IS FIXED */
	/* a hack for apache nt because it does not appear to set argv[1] and sets
	   script filename to php.exe thus makes us parse php.exe instead of file.php
	   requires we get the info from path translated.  This can be removed at
	   such a time that apache nt is fixed */
	if (!script_filename) {
		script_filename = getenv("PATH_TRANSLATED");
	}
#endif

	/* doc_root configuration variable is currently ignored,
	   as it is with every other access method currently also. */

	/* We always need to emalloc() filename, since it gets placed into
	   the include file hash table, and gets freed with that table.
	   Notice that this means that we don't need to efree() it in
	   php_destroy_request_info()! */
#if DISCARD_PATH
	if (script_filename) {
		SG(request_info).path_translated = estrdup(script_filename);
	} else {
		SG(request_info).path_translated = NULL;
	}
#endif

#endif /* 0 */

	SG(request_info).request_method = getenv("REQUEST_METHOD");
	SG(request_info).query_string = getenv("QUERY_STRING");
	SG(request_info).request_uri = getenv("PATH_INFO");
	if (!SG(request_info).request_uri) {
		SG(request_info).request_uri = getenv("SCRIPT_NAME");
	}
	SG(request_info).path_translated = NULL; /* we have to update it later, when we have that information */
	SG(request_info).content_type = (content_type ? content_type : "" );
	SG(request_info).content_length = (content_length?atoi(content_length):0);
	SG(sapi_headers).http_response_code = 200;
	
	/* The CGI RFC allows servers to pass on unvalidated Authorization data */
	auth = getenv("HTTP_AUTHORIZATION");
	php_handle_auth_data(auth TSRMLS_CC);
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

/* {{{ main
 */
int main(int argc, char *argv[])
{
	int exit_status = SUCCESS;
	int cgi = 0, c, i, len;
	zend_file_handle file_handle;
	int retval = FAILURE;
	char *s;
/* temporary locals */
	int behavior=PHP_MODE_STANDARD;
	int no_headers=0;
	int orig_optind=ap_php_optind;
	char *orig_optarg=ap_php_optarg;
	char *argv0=NULL;
	char *script_file=NULL;
	zend_llist global_vars;
	int interactive=0;

#if FORCE_CGI_REDIRECT
	int force_redirect = 1;
	char *redirect_status_env = NULL;
#endif

/* end of temporary locals */
#ifdef ZTS
	zend_compiler_globals *compiler_globals;
	zend_executor_globals *executor_globals;
	php_core_globals *core_globals;
	sapi_globals_struct *sapi_globals;
	void ***tsrm_ls;
#endif

#ifdef PHP_FASTCGI
	int env_size, cgi_env_size;
	int max_requests = 500;
	int requests = 0;
	int fastcgi = !FCGX_IsCGI();
#ifdef PHP_WIN32
	int impersonate = 0;
#endif

	if (fastcgi) { 
		/* Calculate environment size */
		env_size = 0;
		while( environ[ env_size ] ) { env_size++; }
		/* Also include the final NULL pointer */
		env_size++;

		/* Allocate for our environment */
		orig_env = malloc( env_size * sizeof( char *));
		if( !orig_env ) {
			perror( "Can't malloc environment" );
			exit( 1 );
		}
		memcpy( orig_env, environ, env_size * sizeof( char *));
	}
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

	sapi_startup(&cgi_sapi_module);

#ifdef PHP_WIN32
	_fmode = _O_BINARY;			/*sets default for file streams to binary */
	setmode(_fileno(stdin), O_BINARY);		/* make the stdio mode be binary */
	setmode(_fileno(stdout), O_BINARY);		/* make the stdio mode be binary */
	setmode(_fileno(stderr), O_BINARY);		/* make the stdio mode be binary */
#endif


	/* Make sure we detect we are a cgi - a bit redundancy here,
	   but the default case is that we have to check only the first one. */
	if (getenv("SERVER_SOFTWARE")
		|| getenv("SERVER_NAME")
		|| getenv("GATEWAY_INTERFACE")
		|| getenv("REQUEST_METHOD")) {
		cgi = 1;
		if (argc > 1) {
			argv0 = strdup(argv[1]);
		} else {
			argv0 = NULL;
		}
	}

	if (!cgi
#ifdef PHP_FASTCGI
		/* allow ini override for fastcgi */
#endif
		) {
		while ((c=ap_php_getopt(argc, argv, OPTSTRING))!=-1) {
			switch (c) {
				case 'c':
					cgi_sapi_module.php_ini_path_override = strdup(ap_php_optarg);
					break;
			}

		}
		ap_php_optind = orig_optind;
		ap_php_optarg = orig_optarg;
	}


#ifdef ZTS
	compiler_globals = ts_resource(compiler_globals_id);
	executor_globals = ts_resource(executor_globals_id);
	core_globals = ts_resource(core_globals_id);
	sapi_globals = ts_resource(sapi_globals_id);
	tsrm_ls = ts_resource(0);
#endif

	cgi_sapi_module.executable_location = argv[0];

	/* startup after we get the above ini override se we get things right */
	if (php_module_startup(&cgi_sapi_module, NULL, 0)==FAILURE) {
#ifdef ZTS
		tsrm_shutdown();
#endif
		return FAILURE;
	}

#if FORCE_CGI_REDIRECT
	/* check force_cgi after startup, so we have proper output */
	if (cfg_get_long("cgi.force_redirect", &force_redirect) == FAILURE) {
        force_redirect = 1;
	}
	if (cgi && force_redirect) {
        if (cfg_get_string("cgi.redirect_status_env", &redirect_status_env) == FAILURE) {
            redirect_status_env = NULL;
        }
		/* Apache will generate REDIRECT_STATUS,
		 * Netscape and redirect.so will generate HTTP_REDIRECT_STATUS.
		 * redirect.so and installation instructions available from
		 * http://www.koehntopp.de/php.
		 *   -- kk@netuse.de
		 */
		if (!getenv("REDIRECT_STATUS") 
			&& !getenv ("HTTP_REDIRECT_STATUS")
			/* this is to allow a different env var to be configured
			    in case some server does something different than above */
			&& (!redirect_status_env || !getenv(redirect_status_env))
			) {
			PUTS("<b>Security Alert!</b> The PHP CGI cannot be accessed directly.\n\n\
<p>This PHP CGI binary was compiled with force-cgi-redirect enabled.  This\n\
means that a page will only be served up if the REDIRECT_STATUS CGI variable is\n\
set, e.g. via an Apache Action directive.</p>\n\
<p>For more information as to <i>why</i> this behaviour exists, see the <a href=\"http://php.net/security.cgi-bin\">\
manual page for CGI security</a>.</p>\n\
<p>For more information about changing this behaviour or re-enabling this webserver,\n\
consult the installation file that came with this distribution, or visit \n\
<a href=\"http://php.net/install.windows\">the manual page</a>.</p>\n");

#ifdef ZTS
	        tsrm_shutdown();
#endif

			return FAILURE;
		}
	}
#endif							/* FORCE_CGI_REDIRECT */

#ifdef PHP_FASTCGI
	/* How many times to run PHP scripts before dying */
	if( getenv( "PHP_FCGI_MAX_REQUESTS" )) {
		max_requests = atoi( getenv( "PHP_FCGI_MAX_REQUESTS" ));
		if( !max_requests ) {
			fprintf( stderr,
				 "PHP_FCGI_MAX_REQUESTS is not valid\n" );
			exit( 1 );
		}
	}

#ifndef PHP_WIN32
	/* Pre-fork, if required */
	if( getenv( "PHP_FCGI_CHILDREN" )) {
		children = atoi( getenv( "PHP_FCGI_CHILDREN" ));
		if( !children ) {
			fprintf( stderr,
				 "PHP_FCGI_CHILDREN is not valid\n" );
			exit( 1 );
		}
	}

	if( children ) {
		int running = 0;
		int i;
		pid_t pid;

		/* Create a process group for ourself & children */
		setsid();
		pgroup = getpgrp();
#ifdef DEBUG_FASTCGI
		fprintf( stderr, "Process group %d\n", pgroup );
#endif

		/* Set up handler to kill children upon exit */
		act.sa_flags = 0;
		act.sa_handler = fastcgi_cleanup;
		if( sigaction( SIGTERM, &act, &old_term ) ||
		    sigaction( SIGINT, &act, &old_int ) ||
		    sigaction( SIGQUIT, &act, &old_quit )) {
			perror( "Can't set signals" );
			exit( 1 );
		}

		while( parent ) {
			do {
#ifdef DEBUG_FASTCGI
				fprintf( stderr, "Forking, %d running\n",
					 running );
#endif
				pid = fork();
				switch( pid ) {
				case 0:
					/* One of the children.
					 * Make sure we don't go round the
					 * fork loop any more
					 */
					parent = 0;

					/* don't catch our signals */
					sigaction( SIGTERM, &old_term, 0 );
					sigaction( SIGQUIT, &old_quit, 0 );
					sigaction( SIGINT, &old_int, 0 );
					break;
				case -1:
					perror( "php (pre-forking)" );
					exit( 1 );
					break;
				default:
					/* Fine */
					running++;
					break;
				}
			} while( parent && ( running < children ));

			if( parent ) {
#ifdef DEBUG_FASTCGI
				fprintf( stderr, "Wait for kids, pid %d\n",
					 getpid() );
#endif
				wait( &status );
				running--;
			}
		}
	}

#endif /* WIN32 */

#endif

	zend_first_try {
		if (!cgi
#ifdef PHP_FASTCGI
			&& !fastcgi
#endif
			) {
			while ((c=ap_php_getopt(argc, argv, OPTSTRING))!=-1) {
				switch (c) {
					case '?':
						no_headers = 1;
						php_output_startup();
						php_output_activate(TSRMLS_C);
						SG(headers_sent) = 1;
						php_cgi_usage(argv[0]);
						php_end_ob_buffers(1 TSRMLS_CC);
						exit(1);
						break;
				}
			}
			ap_php_optind = orig_optind;
			ap_php_optarg = orig_optarg;
		}

#ifdef PHP_FASTCGI
		/* start of FAST CGI loop */

#ifdef PHP_WIN32
		/* attempt to set security impersonation for fastcgi
		   will only happen on NT based OS, others will ignore it. */
		if (fastcgi) {
			if (cfg_get_long("fastcgi.impersonate", &impersonate) == FAILURE) {
				impersonate = 0;
			}
			if (impersonate) OS_SetImpersonate();
		}
#endif

		while (!fastcgi
			|| FCGX_Accept( &in, &out, &err, &cgi_env ) >= 0) {

			if (fastcgi) {
				/* set up environment */
				cgi_env_size = 0;
				while( cgi_env[ cgi_env_size ] ) { cgi_env_size++; }
				merge_env = malloc( (env_size+cgi_env_size)*sizeof(char*) );
				if( !merge_env ) {
				   perror( "Can't malloc environment" );
				   exit( 1 );
				}
				memcpy( merge_env, orig_env, (env_size-1)*sizeof(char *) );
				memcpy( merge_env + env_size - 1,
					cgi_env, (cgi_env_size+1)*sizeof(char *) );
				environ = merge_env;
			}
#endif

		init_request_info(TSRMLS_C);
		SG(server_context) = (void *) 1; /* avoid server_context==NULL checks */

		SG(request_info).argv0 = argv0;

		zend_llist_init(&global_vars, sizeof(char *), NULL, 0);

		if (!cgi
#ifdef PHP_FASTCGI
			&& !fastcgi
#endif
			) {					/* never execute the arguments if you are a CGI */
			if (SG(request_info).argv0) {
				free(SG(request_info).argv0);
				SG(request_info).argv0 = NULL;
			}
			while ((c = ap_php_getopt(argc, argv, OPTSTRING)) != -1) {
				switch (c) {
					
  				case 'a':	/* interactive mode */
						printf("Interactive mode enabled\n\n");
						interactive=1;
						break;
					
				case 'C': /* don't chdir to the script directory */
						SG(options) |= SAPI_OPTION_NO_CHDIR;
						break;
				case 'd': /* define ini entries on command line */
						define_command_line_ini_entry(ap_php_optarg);
						break;
						
  				case 'e': /* enable extended info output */
						CG(extended_info) = 1;
						break;

  				case 'f': /* parse file */
						script_file = estrdup(ap_php_optarg);
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
						php_cgi_usage(argv[0]);
						php_end_ob_buffers(1 TSRMLS_CC);
						exit(1);
						break;

				case 'i': /* php info & quit */
						if (php_request_startup(TSRMLS_C)==FAILURE) {
							php_module_shutdown(TSRMLS_C);
							return FAILURE;
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
  				case 'n': /* generate indented source mode*/ 
						behavior=PHP_MODE_INDENT;
						break;
#endif

  				case 'q': /* do not generate HTTP headers */
						no_headers = 1;
						break;

  				case 's': /* generate highlighted HTML from source */
						behavior=PHP_MODE_HIGHLIGHT;
						break;

				case 'v': /* show php version & quit */
						no_headers = 1;
						if (php_request_startup(TSRMLS_C)==FAILURE) {
							php_module_shutdown(TSRMLS_C);
							return FAILURE;
						}
						if (no_headers) {
							SG(headers_sent) = 1;
							SG(request_info).no_headers = 1;
						}
						php_printf("PHP %s (%s), Copyright (c) 1997-2002 The PHP Group\n%s", PHP_VERSION, sapi_module.name, get_zend_version());
						php_end_ob_buffers(1 TSRMLS_CC);
						exit(1);
						break;

  				case 'w': 
						behavior=PHP_MODE_STRIP;
						break;

				case 'z': /* load extension file */
						zend_load_extension(ap_php_optarg);
						break;

					default:
						break;
				}
			}
		}							/* not cgi */

		CG(interactive) = interactive;

		if (!cgi
#ifdef PHP_FASTCGI
			&& !fastcgi
#endif
			) {
			if (!SG(request_info).query_string) {
				len = 0;
				if (script_file) {
					len += strlen(script_file) + 1;
				}
				for (i = ap_php_optind; i < argc; i++) {
					len += strlen(argv[i]) + 1;
				}

				s = malloc(len + 1);	/* leak - but only for command line version, so ok */
				*s = '\0';			/* we are pretending it came from the environment  */
				if (script_file) {
					strcpy(s, script_file);
					if (ap_php_optind<argc) {
						strcat(s, "+");
					}
				}
				for (i = ap_php_optind, len = 0; i < argc; i++) {
					strcat(s, argv[i]);
					if (i < (argc - 1)) {
						strcat(s, "+");
					}
				}
				SG(request_info).query_string = s;
			}
		}

		if (script_file) {
			SG(request_info).path_translated = script_file;
		}

		if (php_request_startup(TSRMLS_C)==FAILURE) {
			php_module_shutdown(TSRMLS_C);
			return FAILURE;
		}
		if (no_headers) {
			SG(headers_sent) = 1;
			SG(request_info).no_headers = 1;
		}
#ifdef PHP_FASTCGI
		if (fastcgi) {
			file_handle.type = ZEND_HANDLE_FILENAME;
			file_handle.filename = SG(request_info).path_translated;
		} else {
#endif
			file_handle.filename = "-";
			file_handle.type = ZEND_HANDLE_FP;
			file_handle.handle.fp = stdin;
#ifdef PHP_FASTCGI
		}
#endif
		file_handle.opened_path = NULL;
		file_handle.free_filename = 0;

		/* This actually destructs the elements of the list - ugly hack */
		zend_llist_apply(&global_vars, (llist_apply_func_t) php_register_command_line_global_vars TSRMLS_CC);
		zend_llist_destroy(&global_vars);

		if (!cgi
#ifdef PHP_FASTCGI
			&& !fastcgi
#endif
			) {
			if (!SG(request_info).path_translated && argc > ap_php_optind) {
				SG(request_info).path_translated = estrdup(argv[ap_php_optind]);
			}
		} else {
		/* If for some reason the CGI interface is not setting the
		   PATH_TRANSLATED correctly, SG(request_info).path_translated is NULL.
		   We still call php_fopen_primary_script, because if you set doc_root
		   or user_dir configuration directives, PATH_INFO is used to construct
		   the filename as a side effect of php_fopen_primary_script.
		 */
			char *env_path_translated=NULL;
#if DISCARD_PATH
			env_path_translated = getenv("SCRIPT_FILENAME");
#else
			env_path_translated = getenv("PATH_TRANSLATED");
#endif
			if(env_path_translated) {
#ifdef __riscos__
				/* Convert path to unix format*/
				__riscosify_control|=__RISCOSIFY_DONT_CHECK_DIR;
				env_path_translated=__unixify(env_path_translated,0,NULL,1,0);
#endif
				SG(request_info).path_translated = estrdup(env_path_translated);
			}
		}
		if (cgi || SG(request_info).path_translated) {
			retval = php_fopen_primary_script(&file_handle TSRMLS_CC);
		}

		if (cgi && (retval == FAILURE)) {
			if(!argv0 || !(file_handle.handle.fp = VCWD_FOPEN(argv0, "rb"))) {
				PUTS("No input file specified.\n");
				php_request_shutdown((void *) 0);
				php_module_shutdown(TSRMLS_C);
				return FAILURE;
			}
			file_handle.filename = argv0;
			file_handle.opened_path = expand_filepath(argv0, NULL TSRMLS_CC);
		}

		if (file_handle.handle.fp && (file_handle.handle.fp != stdin)) {
			/* #!php support */
			c = fgetc(file_handle.handle.fp);
			if (c == '#') {
				while (c != 10 && c != 13) {
					c = fgetc(file_handle.handle.fp);	/* skip to end of line */
				}
				CG(zend_lineno)++;
			} else {
				rewind(file_handle.handle.fp);
			}
		}

		switch (behavior) {
			case PHP_MODE_STANDARD:
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
				return SUCCESS;
				break;
			case PHP_MODE_HIGHLIGHT:
				{
					zend_syntax_highlighter_ini syntax_highlighter_ini;

					if (open_file_for_scanning(&file_handle TSRMLS_CC)==SUCCESS) {
						php_get_highlight_struct(&syntax_highlighter_ini);
						zend_highlight(&syntax_highlighter_ini TSRMLS_CC);
						fclose(file_handle.handle.fp);
					}
					return SUCCESS;
				}
				break;
#if 0
			/* Zeev might want to do something with this one day */
			case PHP_MODE_INDENT:
				open_file_for_scanning(&file_handle TSRMLS_CC);
				zend_indent();
				fclose(file_handle.handle.fp);
				return SUCCESS;
				break;
#endif
		}

		{
			char *path_translated;
		
			/*	Go through this trouble so that the memory manager doesn't warn
			 *	about SG(request_info).path_translated leaking
			 */
			if (SG(request_info).path_translated) {
				path_translated = strdup(SG(request_info).path_translated);
				STR_FREE(SG(request_info).path_translated);
				SG(request_info).path_translated = path_translated;
			}
			
			php_request_shutdown((void *) 0);

			if (SG(request_info).path_translated) {
				free(SG(request_info).path_translated);
			}
		}

#ifdef PHP_FASTCGI
			if (!fastcgi) break;
			/* only fastcgi will get here */

			/* TODO: We should free our environment here, but
			 * some platforms are unhappy if they've altered our
			 * existing environment and we then free() the new
			 * environ pointer
			 */

			requests++;
			if( max_requests && ( requests == max_requests )) {
				FCGX_Finish();
				break;
			}
		/* end of fastcgi loop */
		}
#endif

		if (cgi_sapi_module.php_ini_path_override) {
			free(cgi_sapi_module.php_ini_path_override);
		}
	} zend_catch {
		exit_status = 255;
	} zend_end_try();

	php_module_shutdown(TSRMLS_C);

#ifdef ZTS
	tsrm_shutdown();
#endif

#if PHP_WIN32 && ZEND_DEBUG
	_CrtDumpMemoryLeaks( );
#endif

	return exit_status;
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
