/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stig Bakken <ssb@fast.no>                                   |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#include "php.h"
#include "php_globals.h"

#include "SAPI.h"

#if CGI_BINARY

#include <stdio.h>
#include "php.h"
#ifdef MSVC5
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
#include "php_ini.h"
#include "php_globals.h"
#include "main.h"
#include "fopen-wrappers.h"
#include "ext/standard/php_standard.h"
#include "snprintf.h"
#if WIN32|WINNT
#include <io.h>
#include <fcntl.h>
#include "win32/syslog.h"
#include "win32/php_registry.h"
#else
#include <syslog.h>
#endif

#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_highlight.h"
#include "zend_indent.h"


#if MSVC5 || !defined(HAVE_GETOPT)
#include "php_getopt.h"
#endif

PHPAPI extern char *php3_ini_path;

#define PHP_MODE_STANDARD	1
#define PHP_MODE_HIGHLIGHT	2
#define PHP_MODE_INDENT		3

PHPAPI extern char *optarg;
PHPAPI extern int optind;


static int zend_cgibin_ub_write(const char *str, uint str_length)
{
	return fwrite(str, 1, str_length, stdout);
}


static void sapi_cgi_send_header(sapi_header_struct *sapi_header, void *server_context)
{
	if (sapi_header) {
		PHPWRITE_H(sapi_header->header, sapi_header->header_len);
	}
	PHPWRITE_H("\r\n", 2);
}


static int sapi_cgi_read_post(char *buffer, uint count_bytes SLS_DC)
{
	uint read_bytes=0, tmp_read_bytes;

	count_bytes = MIN(count_bytes, SG(request_info).content_length-SG(read_post_bytes));
	while (read_bytes < count_bytes) {
		tmp_read_bytes = read(0, buffer+read_bytes, count_bytes-read_bytes);
		if (tmp_read_bytes<=0) {
			break;
		}
		read_bytes += tmp_read_bytes;
	}
	return read_bytes;
}


static char *sapi_cgi_read_cookies(SLS_D)
{
	return getenv("HTTP_COOKIE");
}


static sapi_module_struct sapi_module = {
	"PHP Language",					/* name */
									
	php_module_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	zend_cgibin_ub_write,			/* unbuffered write */

	php_error,						/* error handler */

	NULL,							/* header handler */
	NULL,							/* send headers handler */
	sapi_cgi_send_header,			/* send header handler */

	sapi_cgi_read_post,				/* read POST data */
	sapi_cgi_read_cookies,			/* read Cookies */

	STANDARD_SAPI_MODULE_PROPERTIES
};


static void php_cgi_usage(char *argv0)
{
	char *prog;

	prog = strrchr(argv0, '/');
	if (prog) {
		prog++;
	} else {
		prog = "php";
	}

	php_printf("Usage: %s [-q] [-h]"
				" [-s]"
				" [-v] [-i] [-f <file>] | "
				"{<file> [args...]}\n"
				"  -q             Quiet-mode.  Suppress HTTP Header output.\n"
				"  -s             Display colour syntax highlighted source.\n"
				"  -f<file>       Parse <file>.  Implies `-q'\n"
				"  -v             Version number\n"
				"  -c<path>       Look for php.ini file in this directory\n"
#if SUPPORT_INTERACTIVE
				"  -a             Run interactively\n"
#endif
				"  -d foo[=bar]   Define INI entry foo with value 'bar'\n"
				"  -e             Generate extended information for debugger/profiler\n"
				"  -i             PHP information\n"
				"  -h             This help\n", prog);
}


static void init_request_info(SLS_D)
{
	char *content_length = getenv("CONTENT_LENGTH");

	SG(request_info).request_method = getenv("REQUEST_METHOD");
	SG(request_info).query_string = getenv("QUERY_STRING");
	SG(request_info).request_uri = getenv("PATH_INFO");
	SG(request_info).path_translated = NULL; /* we have to update it later, when we have that information */
	SG(request_info).content_type = getenv("CONTENT_TYPE");
	SG(request_info).content_length = (content_length?atoi(content_length):0);

	/* CGI does not support HTTP authentication */
	SG(request_info).auth_user = NULL;
	SG(request_info).auth_password = NULL;
}


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
	php_alter_ini_entry(name, strlen(name), value, strlen(value), PHP_INI_SYSTEM);
}


int main(int argc, char *argv[])
{
	int cgi = 0, c, i, len;
	zend_file_handle file_handle;
	char *s;
/* temporary locals */
	int cgi_started=0;
	int behavior=PHP_MODE_STANDARD;
	int no_headers=0;
	int free_path_translated=0;
#if SUPPORT_INTERACTIVE
	int interactive=0;
#endif
/* end of temporary locals */
#ifdef ZTS
	zend_compiler_globals *compiler_globals;
	zend_executor_globals *executor_globals;
	php_core_globals *core_globals;
	sapi_globals_struct *sapi_globals;
#endif


#ifndef ZTS
	if (setjmp(EG(bailout))!=0) {
		return -1;
	}
#endif

#ifdef ZTS
	tsrm_startup(1,1,0);
#endif

	sapi_startup(&sapi_module);

#if WIN32|WINNT
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
		if (argc > 1)
			request_info.php_argv0 = strdup(argv[1]);
		else request_info.php_argv0 = NULL;
#if FORCE_CGI_REDIRECT
		if (!getenv("REDIRECT_STATUS")) {
			PUTS("<b>Security Alert!</b>  PHP CGI cannot be accessed directly.\n\
\n\
<P>This PHP CGI binary was compiled with force-cgi-redirect enabled.  This\n\
means that a page will only be served up if the REDIRECT_STATUS CGI variable is\n\
set.  This variable is set, for example, by Apache's Action directive redirect.\n\
<P>You may disable this restriction by recompiling the PHP binary with the\n\
--disable-force-cgi-redirect switch.  If you do this and you have your PHP CGI\n\
binary accessible somewhere in your web tree, people will be able to circumvent\n\
.htaccess security by loading files through the PHP parser.  A good way around\n\
this is to define doc_root in your php.ini file to something other than your\n\
top-level DOCUMENT_ROOT.  This way you can separate the part of your web space\n\n\
which uses PHP from the normal part using .htaccess security.  If you do not have\n\
any .htaccess restrictions anywhere on your site you can leave doc_root undefined.\n\
\n");

			/* remove that detailed explanation some time */

			return FAILURE;
		}
#endif							/* FORCE_CGI_REDIRECT */
	}

	if (php_module_startup(&sapi_module)==FAILURE) {
		return FAILURE;
	}
#ifdef ZTS
	compiler_globals = ts_resource(compiler_globals_id);
	executor_globals = ts_resource(executor_globals_id);
	core_globals = ts_resource(core_globals_id);
	sapi_globals = ts_resource(sapi_globals_id);
	if (setjmp(EG(bailout))!=0) {
		return -1;
	}
#endif

	init_request_info(SLS_C);
	SG(server_context) = (void *) 1; /* avoid server_context==NULL checks */
	CG(extended_info) = 0;

	if (!cgi) {					/* never execute the arguments if you are a CGI */
		request_info.php_argv0 = NULL;
		while ((c = getopt(argc, argv, "c:d:qvisnaeh?vf:")) != -1) {
			switch (c) {
				case 'f':
					if (!cgi_started){ 
						if (php_request_startup(CLS_C ELS_CC PLS_CC SLS_CC)==FAILURE) {
							php_module_shutdown();
							return FAILURE;
						}
					}
					if (no_headers) {
						SG(headers_sent) = 1;
					}
					cgi_started=1;
					SG(request_info).path_translated = estrdup(optarg);
					free_path_translated=1;
					/* break missing intentionally */
				case 'q':
					no_headers = 1;
					break;
				case 'v':
					if (!cgi_started) {
						if (php_request_startup(CLS_C ELS_CC PLS_CC SLS_CC)==FAILURE) {
							php_module_shutdown();
							return FAILURE;
						}
					}
					if (no_headers) {
						SG(headers_sent) = 1;
					}
					php_printf("%s\n", PHP_VERSION);
					exit(1);
					break;
				case 'i':
					if (!cgi_started) {
						if (php_request_startup(CLS_C ELS_CC PLS_CC SLS_CC)==FAILURE) {
							php_module_shutdown();
							return FAILURE;
						}
					}
					if (no_headers) {
						SG(headers_sent) = 1;
					}
					cgi_started=1;
					php_print_info(0xFFFFFFFF);
					exit(1);
					break;
				case 's':
					behavior=PHP_MODE_HIGHLIGHT;
					break;
				case 'n':
					behavior=PHP_MODE_INDENT;
					break;
				case 'c':
					php3_ini_path = strdup(optarg);		/* intentional leak */
					break;
				case 'a':
#if SUPPORT_INTERACTIVE
					printf("Interactive mode enabled\n\n");
					interactive=1;
#else
					printf("Interactive mode not supported!\n\n");
#endif
					break;
				case 'e':
					CG(extended_info) = 1;
					break;
				case 'h':
				case '?':
					php_output_startup();
					SG(headers_sent) = 1;
					php_cgi_usage(argv[0]);
					exit(1);
					break;
				case 'd':
					define_command_line_ini_entry(optarg);
					break;
				default:
					break;
			}
		}
	}							/* not cgi */

#if SUPPORT_INTERACTIVE
	EG(interactive) = interactive;
#endif

	if (!cgi_started) {
		if (php_request_startup(CLS_C ELS_CC PLS_CC SLS_CC)==FAILURE) {
			php_module_shutdown();
			return FAILURE;
		}
	}
	if (no_headers) {
		SG(headers_sent) = 1;
	}
	file_handle.filename = "-";
	file_handle.type = ZEND_HANDLE_FP;
	file_handle.handle.fp = stdin;


	if (!cgi) {
		if (!SG(request_info).query_string) {
			for (i = optind, len = 0; i < argc; i++)
				len += strlen(argv[i]) + 1;

			s = malloc(len + 1);	/* leak - but only for command line version, so ok */
			*s = '\0';			/* we are pretending it came from the environment  */
			for (i = optind, len = 0; i < argc; i++) {
				strcat(s, argv[i]);
				if (i < (argc - 1))
					strcat(s, "+");
			}
			SG(request_info).query_string = s;
		}
		if (!SG(request_info).path_translated && argc > optind)
			SG(request_info).path_translated = argv[optind];
	}
	/* If for some reason the CGI interface is not setting the
	   PATH_TRANSLATED correctly, SG(request_info).path_translated is NULL.
	   We still call php_fopen_primary_script, because if you set doc_root
	   or user_dir configuration directives, PATH_INFO is used to construct
	   the filename as a side effect of php_fopen_primary_script.
	 */
	if(cgi) {
		SG(request_info).path_translated = getenv("PATH_TRANSLATED");
	}
	if (cgi || SG(request_info).path_translated) {
		file_handle.handle.fp = php_fopen_primary_script();
		file_handle.filename = SG(request_info).path_translated;
	}

	if (cgi && !file_handle.handle.fp) {
		PUTS("No input file specified.\n");
#if 0	/* this is here for debuging under windows */
		if (argc) {
			i = 0;
			php_printf("\nargc %d\n",argc); 
			while (i <= argc) {
				php_printf("%s\n",argv[i]); 
				i++;
			}
		}
#endif
		php_request_shutdown((void *) 0);
		php_module_shutdown();
		return FAILURE;
	} else if (file_handle.handle.fp && file_handle.handle.fp!=stdin) {
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

	file_handle.free_filename = 0;
	switch (behavior) {
		case PHP_MODE_STANDARD:
			php_execute_script(&file_handle CLS_CC ELS_CC PLS_CC);
			break;
		case PHP_MODE_HIGHLIGHT: {
				zend_syntax_highlighter_ini syntax_highlighter_ini;

				if (open_file_for_scanning(&file_handle CLS_CC)==SUCCESS) {
					php_get_highlight_struct(&syntax_highlighter_ini);
					zend_highlight(&syntax_highlighter_ini);
					fclose(file_handle.handle.fp);
				}
				return 0;
			}
			break;
		case PHP_MODE_INDENT:
			open_file_for_scanning(&file_handle CLS_CC);
			zend_indent();
			fclose(file_handle.handle.fp);
			return 0;
			break;
	}

	php3_header();			/* Make sure headers have been sent */
	if (free_path_translated)
		efree(SG(request_info).path_translated);
	php_request_shutdown((void *) 0);
	php_module_shutdown();
#ifdef ZTS
	tsrm_shutdown();
#endif
	return SUCCESS;
}


#endif
