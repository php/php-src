/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at the following url:        |
   | http://www.php.net/license/3_01.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: George Wang <gwang@litespeedtech.com>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "SAPI.h"
#include "php_main.h"
#include "php_ini.h"
#include "php_variables.h"
#include "zend_highlight.h"
#include "zend.h"

#include "lsapilib.h"

#include <stdio.h>

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef PHP_WIN32

#include <io.h>
#include <fcntl.h>
#include "win32/php_registry.h"

#else

#include <sys/wait.h>

#endif

#include <sys/stat.h>

#if HAVE_SYS_TYPES_H

#include <sys/types.h>

#endif

#if HAVE_SIGNAL_H

#include <signal.h>

#endif

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define SAPI_LSAPI_MAX_HEADER_LENGTH 2048

static int  lsapi_mode       = 1;
static char *php_self        = "";
static char *script_filename = "";
static int  source_highlight = 0;

#ifdef ZTS
zend_compiler_globals    *compiler_globals;
zend_executor_globals    *executor_globals;
php_core_globals         *core_globals;
sapi_globals_struct      *sapi_globals;
void ***tsrm_ls;
#endif

zend_module_entry litespeed_module_entry;

/* {{{ php_lsapi_startup
 */
static int php_lsapi_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, NULL, 0)==FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */



/* {{{ sapi_lsapi_ub_write
 */
static int sapi_lsapi_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	int ret;
	int remain;
	if ( lsapi_mode ) {
		ret  = LSAPI_Write( str, str_length );
		if ( ret < str_length ) {
			php_handle_aborted_connection();
			return str_length - ret;
		}
	} else {
		remain = str_length;
		while( remain > 0 ) {
			ret = write( 1, str, remain );
			if ( ret <= 0 ) {
				php_handle_aborted_connection();
				return str_length - remain;
			}
			str += ret;
			remain -= ret;
		}
	}
	return str_length;
}
/* }}} */


/* {{{ sapi_lsapi_flush
 */
static void sapi_lsapi_flush( void * server_context )
{
	if ( lsapi_mode ) {
		if ( LSAPI_Flush() == -1) {
			php_handle_aborted_connection();
		}
	}
}
/* }}} */


/* {{{ sapi_lsapi_deactivate
 */
static int sapi_lsapi_deactivate(TSRMLS_D)
{
	LSAPI_Finish();
	return SUCCESS;
}
/* }}} */




/* {{{ sapi_lsapi_getenv
 */
static char *sapi_lsapi_getenv( char * name, size_t name_len TSRMLS_DC )
{
	if ( lsapi_mode ) {
		return LSAPI_GetEnv( name );
	} else {
		return getenv( name );
	}
}
/* }}} */



static int add_variable( const char * pKey, int keyLen, const char * pValue, int valLen,
						 void * arg )
{
	php_register_variable_safe((char *)pKey, (char *)pValue, valLen, (zval *)arg TSRMLS_CC);
	return 1;
}



/* {{{ sapi_lsapi_register_variables
 */
static void sapi_lsapi_register_variables(zval *track_vars_array TSRMLS_DC)
{

	if ( lsapi_mode ) {
		LSAPI_ForeachHeader( add_variable, track_vars_array );
		LSAPI_ForeachEnv( add_variable, track_vars_array );
		php_import_environment_variables(track_vars_array TSRMLS_CC);

		php_register_variable("PHP_SELF", (SG(request_info).request_uri ? SG(request_info).request_uri:""), track_vars_array TSRMLS_CC);
	} else {
		php_import_environment_variables(track_vars_array TSRMLS_CC);

		php_register_variable("PHP_SELF", php_self, track_vars_array TSRMLS_CC);
		php_register_variable("SCRIPT_NAME", php_self, track_vars_array TSRMLS_CC);
		php_register_variable("SCRIPT_FILENAME", script_filename, track_vars_array TSRMLS_CC);
		php_register_variable("PATH_TRANSLATED", script_filename, track_vars_array TSRMLS_CC);
		php_register_variable("DOCUMENT_ROOT", "", track_vars_array TSRMLS_CC);

	}
}
/* }}} */


/* {{{ sapi_lsapi_read_post
 */
static int sapi_lsapi_read_post(char *buffer, uint count_bytes TSRMLS_DC)
{
	if ( lsapi_mode ) {
		return LSAPI_ReadReqBody( buffer, count_bytes );
	} else {
		return 0;
	}
}
/* }}} */




/* {{{ sapi_lsapi_read_cookies
 */
static char *sapi_lsapi_read_cookies(TSRMLS_D)
{
	if ( lsapi_mode ) {
		return LSAPI_GetHeader( H_COOKIE );
	} else {
		return NULL;
	}
}
/* }}} */


/* {{{ sapi_lsapi_send_headers
 */
static int sapi_lsapi_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	sapi_header_struct  *h;
	zend_llist_position pos;
	if ( lsapi_mode ) {
		LSAPI_SetRespStatus( SG(sapi_headers).http_response_code );

		h = zend_llist_get_first_ex(&sapi_headers->headers, &pos);
		while (h) {
			if ( h->header_len > 0 ) {
				LSAPI_AppendRespHeader(h->header, h->header_len);
			}
			h = zend_llist_get_next_ex(&sapi_headers->headers, &pos);
		}
		if (SG(sapi_headers).send_default_content_type) {
			char	*hd;
			int	 len;
			char	headerBuf[SAPI_LSAPI_MAX_HEADER_LENGTH];

			hd = sapi_get_default_content_type(TSRMLS_C);
			len = snprintf( headerBuf, SAPI_LSAPI_MAX_HEADER_LENGTH - 1,
							"Content-type: %s", hd );
			efree(hd);

			LSAPI_AppendRespHeader( headerBuf, len );
		}
	}
	LSAPI_FinalizeRespHeaders();
	return SAPI_HEADER_SENT_SUCCESSFULLY;


}
/* }}} */


/* {{{ sapi_lsapi_send_headers
 */
static void sapi_lsapi_log_message(char *message TSRMLS_DC)
{
	int len = strlen( message );
	LSAPI_Write_Stderr( message, len);
}
/* }}} */


/* {{{ sapi_module_struct cgi_sapi_module
 */
static sapi_module_struct lsapi_sapi_module =
{
	"litespeed",
	"LiteSpeed",

	php_lsapi_startup,              /* startup */
	php_module_shutdown_wrapper,    /* shutdown */

	NULL,                           /* activate */
	sapi_lsapi_deactivate,          /* deactivate */

	sapi_lsapi_ub_write,            /* unbuffered write */
	sapi_lsapi_flush,               /* flush */
	NULL,                           /* get uid */
	sapi_lsapi_getenv,              /* getenv */

	php_error,                      /* error handler */

	NULL,                           /* header handler */
	sapi_lsapi_send_headers,        /* send headers handler */
	NULL,                           /* send header handler */

	sapi_lsapi_read_post,           /* read POST data */
	sapi_lsapi_read_cookies,        /* read Cookies */

	sapi_lsapi_register_variables,  /* register server variables */
	sapi_lsapi_log_message,         /* Log message */

	NULL,                           /* php.ini path override */
	NULL,                           /* block interruptions */
	NULL,                           /* unblock interruptions */
	NULL,                           /* default post reader */
	NULL,                           /* treat data */
	NULL,                           /* executable location */

	0,                              /* php.ini ignore */

	STANDARD_SAPI_MODULE_PROPERTIES

};
/* }}} */

static int init_request_info( TSRMLS_D )
{
	char * pContentType = LSAPI_GetHeader( H_CONTENT_TYPE );
	char * pAuth;
	
	SG(request_info).content_type = pContentType ? pContentType : "";
	SG(request_info).request_method = LSAPI_GetRequestMethod();
	SG(request_info).query_string = LSAPI_GetQueryString();
	SG(request_info).request_uri = LSAPI_GetScriptName();
	SG(request_info).content_length = LSAPI_GetReqBodyLen();
	SG(request_info).path_translated = LSAPI_GetScriptFileName();

	/* It is not reset by zend engine, set it to 0. */
	SG(sapi_headers).http_response_code = 0;
	
	pAuth = LSAPI_GetHeader( H_AUTHORIZATION );
	php_handle_auth_data(pAuth TSRMLS_CC);
}

static int lsapi_module_main(int show_source TSRMLS_DC)
{
	zend_file_handle file_handle = {0};

	if (php_request_startup(TSRMLS_C) == FAILURE ) {
		return -1;
	}
	if (show_source) {
		zend_syntax_highlighter_ini syntax_highlighter_ini;

		php_get_highlight_struct(&syntax_highlighter_ini);
		highlight_file(SG(request_info).path_translated, &syntax_highlighter_ini TSRMLS_CC);
	} else {
		file_handle.type = ZEND_HANDLE_FILENAME;
		file_handle.handle.fd = 0;
		file_handle.filename = SG(request_info).path_translated;
		file_handle.free_filename = 0;
		file_handle.opened_path = NULL;

		php_execute_script(&file_handle TSRMLS_CC);
	}
	zend_try {
		php_request_shutdown(NULL);
	} zend_end_try();
	return 0;
}


static int alter_ini( const char * pKey, int keyLen, const char * pValue, int valLen,
				void * arg )
{
	int type = ZEND_INI_PERDIR;
	if ( '\001' == *pKey ) {
		++pKey;
		if ( *pKey == 4 ) {
			type = ZEND_INI_SYSTEM;
		}
		++pKey;
		--keyLen;
		zend_alter_ini_entry((char *)pKey, keyLen,
							 (char *)pValue, valLen,
							 type, PHP_INI_STAGE_ACTIVATE);
	}
	return 1;
}


static void override_ini()
{

	LSAPI_ForeachSpecialEnv( alter_ini, NULL );

}

static int processReq( TSRMLS_D )
{
	int ret = 0;
	zend_first_try {
		/* avoid server_context==NULL checks */
		SG(server_context) = (void *) 1;

		init_request_info( TSRMLS_C );

		override_ini();

		if ( lsapi_module_main( source_highlight TSRMLS_CC ) == -1 ) {
			ret = -1;
		}
	} zend_end_try();
	return ret;
}

static void cli_usage( TSRMLS_D )
{
	static const char * usage =
		"Usage: php\n"
		"      php -[b|c|h|i|q|s|v|?] [<file>] [args...]\n"
		"  Run in LSAPI mode, only '-b', '-s' and '-c' are effective\n"
		"  Run in Command Line Interpreter mode when parameters are specified\n"
		"\n"
		"  -b <address:port>|<port> Bind Path for external LSAPI Server mode\n"
		"  -c <path>|<file> Look for php.ini file in this directory\n"
		"  -h    This help\n"
		"  -i    PHP information\n"
		"  -q    Quiet-mode.  Suppress HTTP Header output.\n"
		"  -s    Display colour syntax highlighted source.\n"
		"  -v    Version number\n"
		"  -?    This help\n"
		"\n"
		"  args...	Arguments passed to script.\n";
	php_output_startup();
	php_output_activate(TSRMLS_C);
	php_printf( "%s", usage );
#ifdef PHP_OUTPUT_NEWAPI
    php_output_end_all(TSRMLS_C);
#else
    php_end_ob_buffers(1 TSRMLS_CC);
#endif
}

static int parse_opt( int argc, char * argv[], int *climode,
						char **php_ini_path, char ** php_bind )
{
	char ** p = &argv[1];
	char ** argend= &argv[argc];
	int c;
	while (( p < argend )&&(**p == '-' )) {
		c = *((*p)+1);
		++p;
		switch( c ) {
		case 'b':
			if ( p >= argend ) {
				fprintf( stderr, "TCP or socket address must be specified following '-b' option.\n");
				return -1;
			}
			*php_bind = *p++;
			break;
			
		case 'c':
			if ( p >= argend ) {
				fprintf( stderr, "<path> or <file> must be specified following '-c' option.\n");

				return -1;
			}
			*php_ini_path = *p++;
			break;
		case 's':
			source_highlight = 1;
			break;	
		case 'h':
		case 'i':
		case 'q':
		case 'v':
		case '?':
		default:
			*climode = 1;
			break;
		}
	}
	if ( p - argv < argc ) {
		*climode = 1;
	}
	return 0;
}

static int cli_main( int argc, char * argv[] )
{

	static const char * ini_defaults[] = {
		"report_zend_debug",    "0",
		"display_errors",       "1",
		"register_argc_argv",   "1",
		"html_errors",          "0",
		"implicit_flush",       "1",
		"output_buffering",     "0",
		"max_execution_time",   "0",
		"max_input_time",       "-1",
		NULL
	};

	const char ** ini;
	char ** p = &argv[1];
	char ** argend= &argv[argc];
	int ret = 0;
	int c;
	lsapi_mode = 0;        /* enter CLI mode */

#ifdef PHP_WIN32
	_fmode = _O_BINARY;			/*sets default for file streams to binary */
	setmode(_fileno(stdin), O_BINARY);    /* make the stdio mode be binary */
	setmode(_fileno(stdout), O_BINARY);   /* make the stdio mode be binary */
	setmode(_fileno(stderr), O_BINARY);   /* make the stdio mode be binary */
#endif

	zend_first_try 	{
		SG(server_context) = (void *) 1;

		zend_uv.html_errors = 0; /* tell the engine we're in non-html mode */
		CG(in_compilation) = 0; /* not initialized but needed for several options */
		EG(uninitialized_zval_ptr) = NULL;

		for( ini = ini_defaults; *ini; ini+=2 ) {
			zend_alter_ini_entry( (char *)*ini, strlen( *ini )+1,
								(char *)*(ini+1), strlen( *(ini+1) ),
								PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
		}

		while (( p < argend )&&(**p == '-' )) {
			c = *((*p)+1);
			++p;
			switch( c ) {
			case 'q':
				break;
			case 'i':
				if (php_request_startup(TSRMLS_C) != FAILURE) {
					php_print_info(0xFFFFFFFF TSRMLS_CC);
#ifdef PHP_OUTPUT_NEWAPI
                    php_output_end_all(TSRMLS_C);
#else
                    php_end_ob_buffers(1 TSRMLS_CC);
#endif
					php_request_shutdown( NULL );
				}
				ret = 1;
				break;
			case 'v':
				if (php_request_startup(TSRMLS_C) != FAILURE) {
#if ZEND_DEBUG
					php_printf("PHP %s (%s) (built: %s %s) (DEBUG)\nCopyright (c) 1997-2013 The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
#else
					php_printf("PHP %s (%s) (built: %s %s)\nCopyright (c) 1997-2013 The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
#endif
#ifdef PHP_OUTPUT_NEWAPI
                    php_output_end_all(TSRMLS_C);
#else
                    php_end_ob_buffers(1 TSRMLS_CC);
#endif
					php_request_shutdown( NULL );
				}
				ret = 1;
				break;
			case 'c':
				++p;
			/* fall through */
			case 's':
				break;
				
			case 'h':
			case '?':
			default:
				cli_usage(TSRMLS_C);
				ret = 1;
				break;

			}
		}
		if ( !ret ) {
			if ( *p ) {
				zend_file_handle file_handle = {0};

				file_handle.type = ZEND_HANDLE_FP;
				file_handle.handle.fp = VCWD_FOPEN(*p, "rb");

				if ( file_handle.handle.fp ) {
					script_filename = *p;
					php_self = *p;

					SG(request_info).path_translated = *p;
					SG(request_info).argc = argc - (p - argv);
					SG(request_info).argv = p;

					if (php_request_startup(TSRMLS_C) == FAILURE ) {
						fclose( file_handle.handle.fp );
						ret = 2;
					} else {
						if (source_highlight) {
							zend_syntax_highlighter_ini syntax_highlighter_ini;
					
							php_get_highlight_struct(&syntax_highlighter_ini);
							highlight_file(SG(request_info).path_translated, &syntax_highlighter_ini TSRMLS_CC);
						} else {
							file_handle.filename = *p;
							file_handle.free_filename = 0;
							file_handle.opened_path = NULL;

							php_execute_script(&file_handle TSRMLS_CC);
						}

						php_request_shutdown( NULL );
					}
				} else {
					php_printf("Could not open input file: %s.\n", *p);
				}
			} else {
				cli_usage(TSRMLS_C);
			}
		}

	}zend_end_try();

	php_module_shutdown(TSRMLS_C);

#ifdef ZTS
	tsrm_shutdown();
#endif
	return ret;
}

static int s_stop;
void litespeed_cleanup(int signal)
{
	s_stop = signal;
}


void start_children( int children )
{
	struct sigaction act, old_term, old_quit, old_int, old_usr1;
	int running = 0;
	int status;
	pid_t pid;

	/* Create a process group */
	setsid();

	/* Set up handler to kill children upon exit */
	act.sa_flags = 0;
	act.sa_handler = litespeed_cleanup;
	if( sigaction( SIGTERM, &act, &old_term ) ||
		sigaction( SIGINT,  &act, &old_int  ) ||
		sigaction( SIGUSR1, &act, &old_usr1 ) ||
		sigaction( SIGQUIT, &act, &old_quit )) {
		perror( "Can't set signals" );
		exit( 1 );
	}
	s_stop = 0;
	while( 1 ) {
		while((!s_stop )&&( running < children )) {
			pid = fork();
			switch( pid ) {
			case 0: /* children process */

				/* don't catch our signals */
				sigaction( SIGTERM, &old_term, 0 );
				sigaction( SIGQUIT, &old_quit, 0 );
				sigaction( SIGINT,  &old_int,  0 );
				sigaction( SIGUSR1, &old_usr1, 0 );
				return ;
			case -1:
				perror( "php (pre-forking)" );
				exit( 1 );
				break;
			default: /* parent process */
				running++;
				break;
			}
		} 
		if ( s_stop ) {
			break;
		}
		pid = wait( &status );
		running--;
	}
	kill( -getpgrp(), SIGUSR1 );
	exit( 0 );
}



#include <fcntl.h>
int main( int argc, char * argv[] )
{
	int ret;
	int bindFd;

	char * php_ini_path = NULL;
	char * php_bind	 = NULL;
	char * p;
	int n;
	int climode = 0;
	
#ifdef HAVE_SIGNAL_H
#if defined(SIGPIPE) && defined(SIG_IGN)
	signal(SIGPIPE, SIG_IGN);
#endif
#endif

#ifdef ZTS
	tsrm_startup(1, 1, 0, NULL);
#endif

	if (argc > 1 ) {
		if ( parse_opt( argc, argv, &climode, 
				&php_ini_path, &php_bind ) == -1 ) {
			return 1;
		}
	}
	if ( climode ) {
		lsapi_sapi_module.phpinfo_as_text = 1;
	}
	sapi_startup(&lsapi_sapi_module);

#ifdef ZTS
	compiler_globals = ts_resource(compiler_globals_id);
	executor_globals = ts_resource(executor_globals_id);
	core_globals = ts_resource(core_globals_id);
	sapi_globals = ts_resource(sapi_globals_id);
	tsrm_ls = ts_resource(0);

	SG(request_info).path_translated = NULL;
#endif

	lsapi_sapi_module.executable_location = argv[0];

	if ( php_ini_path ) {
		lsapi_sapi_module.php_ini_path_override = php_ini_path;
	}

	if (php_module_startup(&lsapi_sapi_module, &litespeed_module_entry, 1) == FAILURE) {
#ifdef ZTS
		tsrm_shutdown();
#endif
		return FAILURE;
	}

	if ( climode ) {
		return cli_main(argc, argv);
	}

	
	if ( php_bind ) {
		bindFd = LSAPI_CreateListenSock( php_bind, 10 );
		if ( bindFd == -1 ) {
			fprintf( stderr,
					 "Failed to bind socket [%s]: %s\n", php_bind, strerror( errno ) );
			exit( 2 );
		}
		if ( bindFd != 0 ) {
			dup2( bindFd, 0 );
			close( bindFd );
		}
	}

	LSAPI_Init();
   
	LSAPI_Init_Env_Parameters( NULL );

	if ( php_bind ) {
		LSAPI_No_Check_ppid();
	}

	while( LSAPI_Prefork_Accept_r( &g_req ) >= 0 ) {
		ret = processReq(TSRMLS_C);
		LSAPI_Finish();
		if ( ret ) {
			break;
		}
	}
	php_module_shutdown(TSRMLS_C);

#ifdef ZTS
	tsrm_shutdown();
#endif
	return ret;
}


/*   LiteSpeed PHP module starts here */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_litespeed__void, 0)
ZEND_END_ARG_INFO()
/* }}} */

PHP_FUNCTION(litespeed_request_headers);
PHP_FUNCTION(litespeed_response_headers);

PHP_MINFO_FUNCTION(litespeed);

zend_function_entry litespeed_functions[] = {
	PHP_FE(litespeed_request_headers, 	arginfo_litespeed__void)
	PHP_FE(litespeed_response_headers, 	arginfo_litespeed__void)
	PHP_FALIAS(getallheaders, 			litespeed_request_headers, 	arginfo_litespeed__void)
	PHP_FALIAS(apache_request_headers, 	litespeed_request_headers, 	arginfo_litespeed__void)
	PHP_FALIAS(apache_response_headers, litespeed_response_headers, arginfo_litespeed__void)
	{NULL, NULL, NULL}
};

static PHP_MINIT_FUNCTION(litespeed)
{
	/* REGISTER_INI_ENTRIES(); */
	return SUCCESS;
}


static PHP_MSHUTDOWN_FUNCTION(litespeed)
{
	/* UNREGISTER_INI_ENTRIES(); */
	return SUCCESS;
}

zend_module_entry litespeed_module_entry = {
	STANDARD_MODULE_HEADER,
	"litespeed",
	litespeed_functions,
	PHP_MINIT(litespeed),
	PHP_MSHUTDOWN(litespeed),
	NULL,
	NULL,
	NULL,
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

static int add_associate_array( const char * pKey, int keyLen, const char * pValue, int valLen,
						 void * arg )
{
	add_assoc_string_ex( (zval *)arg, (char *)pKey, keyLen+1, (char *)pValue, 1 );
	return 1;
}


/* {{{ proto array litespeed_request_headers(void)
   Fetch all HTTP request headers */
PHP_FUNCTION(litespeed_request_headers)
{
	/* TODO: */
	if (ZEND_NUM_ARGS() > 0) {
		WRONG_PARAM_COUNT;
	}
	array_init(return_value);

	LSAPI_ForeachOrgHeader( add_associate_array, return_value );

}
/* }}} */



/* {{{ proto array litespeed_response_headers(void)
   Fetch all HTTP response headers */
PHP_FUNCTION(litespeed_response_headers)
{
	sapi_header_struct  *h;
	zend_llist_position pos;
	char *	   p;
	int		  len;
	char		 headerBuf[SAPI_LSAPI_MAX_HEADER_LENGTH];

	if (ZEND_NUM_ARGS() > 0) {
		WRONG_PARAM_COUNT;
	}

	if (!&SG(sapi_headers).headers) {
		RETURN_FALSE;
	}
	array_init(return_value);

	h = zend_llist_get_first_ex(&SG(sapi_headers).headers, &pos);
	while (h) {
		if ( h->header_len > 0 ) {
			p = strchr( h->header, ':' );
			len = p - h->header;
			if (( p )&&( len > 0 )) {
				memmove( headerBuf, h->header, len );
				while( len > 0 && (isspace( headerBuf[len-1])) ) {
					--len;
				}
				headerBuf[len] = 0;
				if ( len ) {
					while( isspace(*++p));
					add_assoc_string_ex(return_value, headerBuf, len+1, p, 1 );
				}
			}
		}
		h = zend_llist_get_next_ex(&SG(sapi_headers).headers, &pos);
	}  
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


