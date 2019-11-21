/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#include "php.h"
#include "SAPI.h"
#include "php_main.h"
#include "php_ini.h"
#include "php_variables.h"
#include "zend_highlight.h"
#include "zend.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/info.h"
#include "lsapilib.h"

#include <stdio.h>
#include <stdlib.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/wait.h>
#include <sys/stat.h>

#if HAVE_SYS_TYPES_H

#include <sys/types.h>

#endif

#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
#include "lscriu.c"
#endif

#define SAPI_LSAPI_MAX_HEADER_LENGTH 2048

/* Key for each cache entry is dirname(PATH_TRANSLATED).
 *
 * NOTE: Each cache entry config_hash contains the combination from all user ini files found in
 *       the path starting from doc_root through to dirname(PATH_TRANSLATED).  There is no point
 *       storing per-file entries as it would not be possible to detect added / deleted entries
 *       between separate files.
 */
typedef struct _user_config_cache_entry {
    time_t expires;
    HashTable user_config;
} user_config_cache_entry;
static HashTable user_config_cache;

static int  lsapi_mode       = 0;
static char *php_self        = "";
static char *script_filename = "";
static int  source_highlight = 0;
static int  ignore_php_ini   = 0;
static char * argv0 = NULL;
static int  engine = 1;
static int  parse_user_ini   = 0;

#ifdef ZTS
zend_compiler_globals    *compiler_globals;
zend_executor_globals    *executor_globals;
php_core_globals         *core_globals;
sapi_globals_struct      *sapi_globals;
#endif

zend_module_entry litespeed_module_entry;

static void init_sapi_from_env(sapi_module_struct *sapi_module)
{
    char *p;
    p = getenv("LSPHPRC");
    if (p)
        sapi_module->php_ini_path_override = p;
}

/* {{{ php_lsapi_startup
 */
static int php_lsapi_startup(sapi_module_struct *sapi_module)
{
    if (php_module_startup(sapi_module, NULL, 0)==FAILURE) {
        return FAILURE;
    }
    argv0 = sapi_module->executable_location;
    return SUCCESS;
}
/* }}} */

/* {{{ sapi_lsapi_ini_defaults */

/* overwriteable ini defaults must be set in sapi_cli_ini_defaults() */
#define INI_DEFAULT(name,value)\
    ZVAL_STRING(tmp, value, 0);\
    zend_hash_update(configuration_hash, name, sizeof(name), tmp, sizeof(zval), (void**)&entry);\
    Z_STRVAL_P(entry) = zend_strndup(Z_STRVAL_P(entry), Z_STRLEN_P(entry))

static void sapi_lsapi_ini_defaults(HashTable *configuration_hash)
{
#if PHP_MAJOR_VERSION > 4
/*
    zval *tmp, *entry;

    MAKE_STD_ZVAL(tmp);

    INI_DEFAULT("register_long_arrays", "0");

    FREE_ZVAL(tmp);
*/
#endif

}
/* }}} */


/* {{{ sapi_lsapi_ub_write
 */
static size_t sapi_lsapi_ub_write(const char *str, size_t str_length)
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
static void sapi_lsapi_flush(void * server_context)
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
static int sapi_lsapi_deactivate(void)
{
    if ( SG(request_info).path_translated ) {
        efree( SG(request_info).path_translated );
        SG(request_info).path_translated = NULL;
    }

    return SUCCESS;
}
/* }}} */




/* {{{ sapi_lsapi_getenv
 */
static char *sapi_lsapi_getenv( char * name, size_t name_len )
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
    int filter_arg = (Z_ARR_P((zval *)arg) == Z_ARR(PG(http_globals)[TRACK_VARS_ENV]))
        ? PARSE_ENV : PARSE_SERVER;
    char * new_val = (char *) pValue;
    size_t new_val_len;

    if (sapi_module.input_filter(filter_arg, (char *)pKey, &new_val, valLen, &new_val_len)) {
        php_register_variable_safe((char *)pKey, new_val, new_val_len, (zval *)arg );
    }
    return 1;
}

static void litespeed_php_import_environment_variables(zval *array_ptr)
{
    char buf[128];
    char **env, *p, *t = buf;
    size_t alloc_size = sizeof(buf);
    unsigned long nlen; /* ptrdiff_t is not portable */

    if (Z_TYPE(PG(http_globals)[TRACK_VARS_ENV]) == IS_ARRAY &&
        Z_ARR_P(array_ptr) != Z_ARR(PG(http_globals)[TRACK_VARS_ENV]) &&
        zend_hash_num_elements(Z_ARRVAL(PG(http_globals)[TRACK_VARS_ENV])) > 0
    ) {
        zval_ptr_dtor_nogc(array_ptr);
        ZVAL_DUP(array_ptr, &PG(http_globals)[TRACK_VARS_ENV]);
        return;
    } else if (Z_TYPE(PG(http_globals)[TRACK_VARS_SERVER]) == IS_ARRAY &&
        Z_ARR_P(array_ptr) != Z_ARR(PG(http_globals)[TRACK_VARS_SERVER]) &&
        zend_hash_num_elements(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER])) > 0
    ) {
        zval_ptr_dtor_nogc(array_ptr);
        ZVAL_DUP(array_ptr, &PG(http_globals)[TRACK_VARS_SERVER]);
        return;
    }

    tsrm_env_lock();
    for (env = environ; env != NULL && *env != NULL; env++) {
        p = strchr(*env, '=');
        if (!p) {               /* malformed entry? */
            continue;
        }
        nlen = p - *env;
        if (nlen >= alloc_size) {
            alloc_size = nlen + 64;
            t = (t == buf ? emalloc(alloc_size): erealloc(t, alloc_size));
        }
        memcpy(t, *env, nlen);
        t[nlen] = '\0';
        add_variable(t, nlen, p + 1, strlen( p + 1 ), array_ptr);
    }
    tsrm_env_unlock();
    if (t != buf && t != NULL) {
        efree(t);
    }
}

/* {{{ sapi_lsapi_register_variables
 */
static void sapi_lsapi_register_variables(zval *track_vars_array)
{
    char * php_self = "";
    if ( lsapi_mode ) {
        if ( (SG(request_info).request_uri ) )
            php_self = (SG(request_info).request_uri );

        litespeed_php_import_environment_variables(track_vars_array);

        LSAPI_ForeachHeader( add_variable, track_vars_array );
        LSAPI_ForeachEnv( add_variable, track_vars_array );
        add_variable("PHP_SELF", 8, php_self, strlen( php_self ), track_vars_array );
    } else {
        php_import_environment_variables(track_vars_array);

        php_register_variable("PHP_SELF", php_self, track_vars_array);
        php_register_variable("SCRIPT_NAME", php_self, track_vars_array);
        php_register_variable("SCRIPT_FILENAME", script_filename, track_vars_array);
        php_register_variable("PATH_TRANSLATED", script_filename, track_vars_array);
        php_register_variable("DOCUMENT_ROOT", "", track_vars_array);

    }
}
/* }}} */


/* {{{ sapi_lsapi_read_post
 */
static size_t sapi_lsapi_read_post(char *buffer, size_t count_bytes)
{
    if ( lsapi_mode ) {
        ssize_t rv = LSAPI_ReadReqBody(buffer, (unsigned long long)count_bytes);
        return (rv >= 0) ? (size_t)rv : 0;
    } else {
        return 0;
    }
}
/* }}} */




/* {{{ sapi_lsapi_read_cookies
 */
static char *sapi_lsapi_read_cookies(void)
{
    if ( lsapi_mode ) {
        return LSAPI_GetHeader( H_COOKIE );
    } else {
        return NULL;
    }
}
/* }}} */


typedef struct _http_error {
  int code;
  const char* msg;
} http_error;

static const http_error http_error_codes[] = {
       {100, "Continue"},
       {101, "Switching Protocols"},
       {200, "OK"},
       {201, "Created"},
       {202, "Accepted"},
       {203, "Non-Authoritative Information"},
       {204, "No Content"},
       {205, "Reset Content"},
       {206, "Partial Content"},
       {300, "Multiple Choices"},
       {301, "Moved Permanently"},
       {302, "Moved Temporarily"},
       {303, "See Other"},
       {304, "Not Modified"},
       {305, "Use Proxy"},
       {400, "Bad Request"},
       {401, "Unauthorized"},
       {402, "Payment Required"},
       {403, "Forbidden"},
       {404, "Not Found"},
       {405, "Method Not Allowed"},
       {406, "Not Acceptable"},
       {407, "Proxy Authentication Required"},
       {408, "Request Time-out"},
       {409, "Conflict"},
       {410, "Gone"},
       {411, "Length Required"},
       {412, "Precondition Failed"},
       {413, "Request Entity Too Large"},
       {414, "Request-URI Too Large"},
       {415, "Unsupported Media Type"},
       {428, "Precondition Required"},
       {429, "Too Many Requests"},
       {431, "Request Header Fields Too Large"},
       {451, "Unavailable For Legal Reasons"},
       {500, "Internal Server Error"},
       {501, "Not Implemented"},
       {502, "Bad Gateway"},
       {503, "Service Unavailable"},
       {504, "Gateway Time-out"},
       {505, "HTTP Version not supported"},
       {511, "Network Authentication Required"},
       {0,   NULL}
};


static int sapi_lsapi_send_headers_like_cgi(sapi_headers_struct *sapi_headers)
{
    char buf[SAPI_LSAPI_MAX_HEADER_LENGTH];
    sapi_header_struct *h;
    zend_llist_position pos;
    zend_bool ignore_status = 0;
    int response_status = SG(sapi_headers).http_response_code;

    if (SG(request_info).no_headers == 1) {
        LSAPI_FinalizeRespHeaders();
        return SAPI_HEADER_SENT_SUCCESSFULLY;
    }

    if (SG(sapi_headers).http_response_code != 200)
    {
        int len;
        zend_bool has_status = 0;

        char *s;

        if (SG(sapi_headers).http_status_line &&
             (s = strchr(SG(sapi_headers).http_status_line, ' ')) != 0 &&
             (s - SG(sapi_headers).http_status_line) >= 5 &&
             strncasecmp(SG(sapi_headers).http_status_line, "HTTP/", 5) == 0
        ) {
            len = slprintf(buf, sizeof(buf), "Status:%s", s);
            response_status = atoi((s + 1));
        } else {
            h = (sapi_header_struct*)zend_llist_get_first_ex(&sapi_headers->headers, &pos);
            while (h) {
                if (h->header_len > sizeof("Status:")-1 &&
                    strncasecmp(h->header, "Status:", sizeof("Status:")-1) == 0
                ) {
                    has_status = 1;
                    break;
                }
                h = (sapi_header_struct*)zend_llist_get_next_ex(&sapi_headers->headers, &pos);
            }
            if (!has_status) {
                http_error *err = (http_error*)http_error_codes;

                while (err->code != 0) {
                    if (err->code == SG(sapi_headers).http_response_code) {
                        break;
                    }
                    err++;
                }
                if (err->msg) {
                    len = slprintf(buf, sizeof(buf), "Status: %d %s", SG(sapi_headers).http_response_code, err->msg);
                } else {
                    len = slprintf(buf, sizeof(buf), "Status: %d", SG(sapi_headers).http_response_code);
                }
            }
        }

        if (!has_status) {
            LSAPI_AppendRespHeader( buf, len );
            ignore_status = 1;
        }
    }

    h = (sapi_header_struct*)zend_llist_get_first_ex(&sapi_headers->headers, &pos);
    while (h) {
        /* prevent CRLFCRLF */
        if (h->header_len) {
            if (h->header_len > sizeof("Status:")-1 &&
                strncasecmp(h->header, "Status:", sizeof("Status:")-1) == 0
            ) {
                if (!ignore_status) {
                    ignore_status = 1;
                    LSAPI_AppendRespHeader(h->header, h->header_len);
                }
            } else if (response_status == 304 && h->header_len > sizeof("Content-Type:")-1 &&
                       strncasecmp(h->header, "Content-Type:", sizeof("Content-Type:")-1) == 0
                   ) {
                h = (sapi_header_struct*)zend_llist_get_next_ex(&sapi_headers->headers, &pos);
                continue;
            } else {
                LSAPI_AppendRespHeader(h->header, h->header_len);
            }
        }
        h = (sapi_header_struct*)zend_llist_get_next_ex(&sapi_headers->headers, &pos);
    }

    LSAPI_FinalizeRespHeaders();
    return SAPI_HEADER_SENT_SUCCESSFULLY;
}

/*
    mod_lsapi mode or legacy LS mode
*/
static int mod_lsapi_mode = 0;


/* {{{ sapi_lsapi_send_headers
 */
static int sapi_lsapi_send_headers(sapi_headers_struct *sapi_headers)
{
    sapi_header_struct  *h;
    zend_llist_position pos;

    if ( mod_lsapi_mode ) {
        /* mod_lsapi mode */
        return sapi_lsapi_send_headers_like_cgi(sapi_headers);
    }

    /* Legacy mode */
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
            char    *hd;
            int     len;
            char    headerBuf[SAPI_LSAPI_MAX_HEADER_LENGTH];

            hd = sapi_get_default_content_type();
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
static void sapi_lsapi_log_message(char *message, int syslog_type_int)
{
    char buf[8192];
    int len = strlen( message );
    if ( *(message + len - 1 ) != '\n' )
    {
        snprintf( buf, 8191, "%s\n", message );
        message = buf;
        if (len > 8191)
            len = 8191;
        ++len;
    }
    LSAPI_Write_Stderr( message, len);
}
/* }}} */

/* Set to 1 to turn on log messages useful during development:
 */
#if 0
static void log_message (const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char buf[0x100];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    sapi_lsapi_log_message(buf
#if PHP_MAJOR_VERSION > 7 || (PHP_MAJOR_VERSION == 7 && PHP_MINOR_VERSION >= 1)
                               , 0
#endif
                                  );
}
#define DEBUG_MESSAGE(fmt, ...) log_message("LS:%d " fmt "\n", __LINE__, ##__VA_ARGS__)
#else
#define DEBUG_MESSAGE(fmt, ...)
#endif

static int lsapi_activate_user_ini();

static int sapi_lsapi_activate()
{
    char *path, *server_name;
    size_t path_len, server_name_len;

    /* PATH_TRANSLATED should be defined at this stage but better safe than sorry :) */
    if (!SG(request_info).path_translated) {
            return FAILURE;
    }

    if (php_ini_has_per_host_config()) {
        server_name = sapi_lsapi_getenv("SERVER_NAME", 0);
        /* SERVER_NAME should also be defined at this stage..but better check it anyway */
        if (server_name) {
                server_name_len = strlen(server_name);
                server_name = estrndup(server_name, server_name_len);
                zend_str_tolower(server_name, server_name_len);
                php_ini_activate_per_host_config(server_name, server_name_len);
                efree(server_name);
        }
    }

    if (php_ini_has_per_dir_config()) {
        /* Prepare search path */
        path_len = strlen(SG(request_info).path_translated);

        /* Make sure we have trailing slash! */
        if (!IS_SLASH(SG(request_info).path_translated[path_len])) {
            path = emalloc(path_len + 2);
            memcpy(path, SG(request_info).path_translated, path_len + 1);
            path_len = zend_dirname(path, path_len);
            path[path_len++] = DEFAULT_SLASH;
        } else {
            path = estrndup(SG(request_info).path_translated, path_len);
            path_len = zend_dirname(path, path_len);
        }
        path[path_len] = 0;

        /* Activate per-dir-system-configuration defined in php.ini and stored into configuration_hash during startup */
        php_ini_activate_per_dir_config(path, path_len); /* Note: for global settings sake we check from root to path */

        efree(path);
    }

    if (parse_user_ini && lsapi_activate_user_ini() == FAILURE) {
        return FAILURE;
    }
    return SUCCESS;
}
/* {{{ sapi_module_struct cgi_sapi_module
 */
static sapi_module_struct lsapi_sapi_module =
{
    "litespeed",
    "LiteSpeed V7.6",

    php_lsapi_startup,              /* startup */
    php_module_shutdown_wrapper,    /* shutdown */

    sapi_lsapi_activate,            /* activate */
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
    NULL,                           /* Get request time */
    NULL,                           /* Child terminate */

    STANDARD_SAPI_MODULE_PROPERTIES

};
/* }}} */

static void init_request_info( void )
{
    char * pContentType = LSAPI_GetHeader( H_CONTENT_TYPE );
    char * pAuth;

    SG(request_info).content_type = pContentType ? pContentType : "";
    SG(request_info).request_method = LSAPI_GetRequestMethod();
    SG(request_info).query_string = LSAPI_GetQueryString();
    SG(request_info).request_uri = LSAPI_GetScriptName();
    SG(request_info).content_length = LSAPI_GetReqBodyLen();
    SG(request_info).path_translated = estrdup( LSAPI_GetScriptFileName());

    /* It is not reset by zend engine, set it to 200. */
    SG(sapi_headers).http_response_code = 200;

    pAuth = LSAPI_GetHeader( H_AUTHORIZATION );
    php_handle_auth_data(pAuth);
}

static int lsapi_execute_script( zend_file_handle * file_handle)
{
    char *p;
    int len;
	zend_stream_init_filename(file_handle, SG(request_info).path_translated);

    p = argv0;
    *p++ = ':';
    len = strlen( SG(request_info).path_translated );
    if ( len > 45 )
        len = len - 45;
    else
        len = 0;
    memccpy( p, SG(request_info).path_translated + len, 0, 46 );

    php_execute_script(file_handle);
    return 0;

}

static void lsapi_sigsegv( int signal )
{
    //fprintf(stderr, "lsapi_sigsegv: %d: Segmentation violation signal is caught during request shutdown\n", getpid());
    _exit(1);
}

static int do_clean_shutdown = 1;

static int clean_onexit = 1;


static void lsapi_clean_shutdown()
{
    struct sigaction act;
    int sa_rc;
    struct itimerval tmv;
#if PHP_MAJOR_VERSION >= 7
    zend_string * key;
#endif
    clean_onexit = 1;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = lsapi_sigsegv;
    sa_rc = sigaction(SIGINT,  &act, NULL);
    sa_rc = sigaction(SIGQUIT, &act, NULL);
    sa_rc = sigaction(SIGILL,  &act, NULL);
    sa_rc = sigaction(SIGABRT, &act, NULL);
    sa_rc = sigaction(SIGBUS,  &act, NULL);
    sa_rc = sigaction(SIGSEGV, &act, NULL);
    sa_rc = sigaction(SIGTERM, &act, NULL);

    sa_rc = sigaction(SIGPROF, &act, NULL);
    memset(&tmv, 0, sizeof(struct itimerval));
    tmv.it_value.tv_sec = 0;
    tmv.it_value.tv_usec = 100000;
    setitimer(ITIMER_PROF, &tmv, NULL);

#if PHP_MAJOR_VERSION >= 7
    key = zend_string_init("error_reporting", 15, 1);
    zend_alter_ini_entry_chars_ex(key, "0", 1,
                        PHP_INI_SYSTEM, PHP_INI_STAGE_SHUTDOWN, 1);
    zend_string_release(key);
#else
    zend_alter_ini_entry("error_reporting", 16, "0", 1,
                        PHP_INI_SYSTEM, PHP_INI_STAGE_SHUTDOWN);
#endif

    zend_try {
        php_request_shutdown(NULL);
    } zend_end_try();
}

static void lsapi_sigterm(int signal)
{

    // fprintf(stderr, "lsapi_sigterm: %d: clean_onexit %d\n", getpid(), clean_onexit );
    if(!clean_onexit)
    {
        lsapi_clean_shutdown();
    }
    exit(1);
}

static void lsapi_atexit(void)
{
    //fprintf(stderr, "lsapi_atexit: %d: clean_onexit %d\n", getpid(), clean_onexit );
    if(!clean_onexit)
    {
        lsapi_clean_shutdown();
    }
}


static int lsapi_module_main(int show_source)
{
    struct sigaction act;
    int sa_rc;
    zend_file_handle file_handle;
    memset(&file_handle, 0, sizeof(file_handle));
    if (php_request_startup() == FAILURE ) {
        return -1;
    }

    if (do_clean_shutdown) {
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_NODEFER;
        act.sa_handler = lsapi_sigterm;
        sa_rc = sigaction( SIGINT, &act, NULL);
        sa_rc = sigaction( SIGQUIT, &act, NULL);
        sa_rc = sigaction( SIGILL, &act, NULL);
        sa_rc = sigaction( SIGABRT, &act, NULL);
        sa_rc = sigaction( SIGBUS, &act, NULL);
        sa_rc = sigaction( SIGSEGV, &act, NULL);
        sa_rc = sigaction( SIGTERM, &act, NULL);

        clean_onexit = 0;
    }

    if (show_source) {
        zend_syntax_highlighter_ini syntax_highlighter_ini;

        php_get_highlight_struct(&syntax_highlighter_ini);
        highlight_file(SG(request_info).path_translated, &syntax_highlighter_ini);
    } else {
        lsapi_execute_script( &file_handle);
    }
    zend_try {
        php_request_shutdown(NULL);

        clean_onexit = 1;

        memset( argv0, 0, 46 );
    } zend_end_try();
    return 0;
}


static int alter_ini( const char * pKey, int keyLen, const char * pValue, int valLen,
                void * arg )
{
    zend_string * psKey;

    int type = ZEND_INI_PERDIR;
    int stage = PHP_INI_STAGE_RUNTIME;
    if ( '\001' == *pKey ) {
        ++pKey;
        if ( *pKey == 4 ) {
            type = ZEND_INI_SYSTEM;
            /*
              Use ACTIVATE stage in legacy mode only.

              RUNTIME stage should be used here,
              as with ACTIVATE it's impossible to change the option from script with ini_set 
            */
            if(!mod_lsapi_mode)
            {
                stage = PHP_INI_STAGE_ACTIVATE;
            }
        }
        else
        {
            stage = PHP_INI_STAGE_HTACCESS;
        }
        ++pKey;
        --keyLen;
        if (( keyLen == 7 )&&( strncasecmp( pKey, "engine", 6 )== 0 ))
        {
            if ( *pValue == '0' )
                engine = 0;
        }
        else
        {
            --keyLen;
            psKey = zend_string_init(pKey, keyLen, 1);
            zend_alter_ini_entry_chars(psKey,
                             (char *)pValue, valLen,
                             type, stage);
            zend_string_release_ex(psKey, 1);
        }
    }
    return 1;
}

static void user_config_cache_entry_dtor(zval *el)
{
    user_config_cache_entry *entry = (user_config_cache_entry *)Z_PTR_P(el);
    zend_hash_destroy(&entry->user_config);
    free(entry);
}

static void user_config_cache_init()
{
    zend_hash_init(&user_config_cache, 0, NULL, user_config_cache_entry_dtor, 1);
}

static int pathlen_without_trailing_slash(char *path)
{
    int len = (int)strlen(path);
    while (len > 1 && /* mind "/" as root dir */
           path[len-1] == DEFAULT_SLASH)
    {
        --len;
    }
    return len;
}

static inline char* skip_slash(char *s)
{
    while (*s == DEFAULT_SLASH) {
        ++s;
    }
    return s;
}

/**
 * Walk down the path_stop starting at path_start.
 *
 * If path_start = "/path1" and path_stop = "/path1/path2/path3"
 * the callback will be called 3 times with the next args:
 *
 *   1. "/path1/path2/path3"
 *             ^ end
 *       ^ start
 *   2. "/path1/path2/path3"
 *                   ^ end
 *       ^ start
 *   3. "/path1/path2/path3"
 *                         ^ end
 *       ^ start
 *
 * path_stop has to be a subdir of path_start
 * or to be path_start itself.
 *
 * Both path args have to be absolute.
 * Trailing slashes are allowed.
 * NULL or empty string args are not allowed.
 */
static void walk_down_the_path(char* path_start,
                               char* path_stop,
                               void (*cb)(char* begin,
                                          char* end,
                                          void* data),
                               void* data)
{
    char *pos = path_stop + pathlen_without_trailing_slash(path_start);
    cb(path_stop, pos, data);

    while ((pos = skip_slash(pos))[0]) {
        pos = strchr(pos, DEFAULT_SLASH);
        if (!pos) {
            /* The last token without trailing slash
             */
            cb(path_stop, path_stop + strlen(path_stop), data);
            return;
        }
        cb(path_stop, pos, data);
    }
}


typedef struct {
    char *path;
    uint32_t path_len;
    char *doc_root;
    user_config_cache_entry *entry;
} _lsapi_activate_user_ini_ctx;

typedef int (*fn_activate_user_ini_chain_t)
        (_lsapi_activate_user_ini_ctx *ctx, void* next);

static int lsapi_activate_user_ini_basic_checks(_lsapi_activate_user_ini_ctx *ctx,
                                                void* next)
{
    int rc = SUCCESS;
    fn_activate_user_ini_chain_t *fn_next = next;

    if (!PG(user_ini_filename) || !*PG(user_ini_filename)) {
        return SUCCESS;
    }

    /* PATH_TRANSLATED should be defined at this stage */
    ctx->path = SG(request_info).path_translated;
    if (!ctx->path || !*ctx->path) {
        return FAILURE;
    }

    ctx->doc_root = sapi_lsapi_getenv("DOCUMENT_ROOT", 0);
    DEBUG_MESSAGE("doc_root: %s", ctx->doc_root);

    if (*fn_next) {
        rc = (*fn_next)(ctx, fn_next + 1);
    }

    return rc;
}

static int lsapi_activate_user_ini_mk_path(_lsapi_activate_user_ini_ctx *ctx,
                                           void* next)
{
    char *path;
    int rc = SUCCESS;
    fn_activate_user_ini_chain_t *fn_next = next;

    /* Extract dir name from path_translated * and store it in 'path' */
    ctx->path_len = strlen(ctx->path);
    path = ctx->path = estrndup(SG(request_info).path_translated, ctx->path_len);
    ctx->path_len = zend_dirname(path, ctx->path_len);

    if (*fn_next) {
        rc = (*fn_next)(ctx, fn_next + 1);
    }

    efree(path);
    return rc;
}

static int lsapi_activate_user_ini_mk_realpath(_lsapi_activate_user_ini_ctx *ctx,
                                               void* next)
{
    char *real_path;
    int rc = SUCCESS;
    fn_activate_user_ini_chain_t *fn_next = next;

    if (!IS_ABSOLUTE_PATH(ctx->path, ctx->path_len)) {
        real_path = tsrm_realpath(ctx->path, NULL);
        if (!real_path) {
            return SUCCESS;
        }
        ctx->path = real_path;
        ctx->path_len = strlen(ctx->path);
    } else {
        real_path = NULL;
    }

    if (*fn_next) {
        rc = (*fn_next)(ctx, fn_next + 1);
    }

    if (real_path)
        efree(real_path);
    return rc;
}

static int lsapi_activate_user_ini_mk_user_config(_lsapi_activate_user_ini_ctx *ctx,
                                                  void* next)
{
    fn_activate_user_ini_chain_t *fn_next = next;

    /* Find cached config entry: If not found, create one */
    ctx->entry = zend_hash_str_find_ptr(&user_config_cache, ctx->path, ctx->path_len);

    if (!ctx->entry)
    {
        ctx->entry = pemalloc(sizeof(user_config_cache_entry), 1);
        ctx->entry->expires = 0;
        zend_hash_init(&ctx->entry->user_config, 0, NULL,
                       config_zval_dtor, 1);
        zend_hash_str_update_ptr(&user_config_cache, ctx->path,
                                            ctx->path_len, ctx->entry);
    }

    if (*fn_next) {
        return (*fn_next)(ctx, fn_next + 1);
    } else {
        return SUCCESS;
    }
}

static void walk_down_the_path_callback(char* begin,
                                        char* end,
                                        void* data)
{
    _lsapi_activate_user_ini_ctx *ctx = data;
    char tmp = end[0];
    end[0] = 0;
    php_parse_user_ini_file(begin, PG(user_ini_filename), &ctx->entry->user_config);
    end[0] = tmp;
}

static int lsapi_activate_user_ini_walk_down_the_path(_lsapi_activate_user_ini_ctx *ctx,
                                                      void* next)
{
    time_t request_time = sapi_get_request_time();
    uint32_t docroot_len;
    int rc = SUCCESS;
    fn_activate_user_ini_chain_t *fn_next = next;

    if (!ctx->entry->expires || request_time > ctx->entry->expires)
    {
        docroot_len = ctx->doc_root && ctx->doc_root[0]
                    ? pathlen_without_trailing_slash(ctx->doc_root)
                    : 0;

        int is_outside_of_docroot = !docroot_len ||
                ctx->path_len < docroot_len ||
                strncmp(ctx->path, ctx->doc_root, docroot_len) != 0;

        if (is_outside_of_docroot) {
            php_parse_user_ini_file(ctx->path, PG(user_ini_filename),
                                    &ctx->entry->user_config);
        } else {
            walk_down_the_path(ctx->doc_root, ctx->path,
                               &walk_down_the_path_callback, ctx);
        }

        ctx->entry->expires = request_time + PG(user_ini_cache_ttl);
    }

    if (*fn_next) {
        rc = (*fn_next)(ctx, fn_next + 1);
    }

    return rc;
}

static int lsapi_activate_user_ini_finally(_lsapi_activate_user_ini_ctx *ctx,
                                           void* next)
{
    int rc = SUCCESS;
    fn_activate_user_ini_chain_t *fn_next = next;

    php_ini_activate_config(&ctx->entry->user_config, PHP_INI_PERDIR,
                            PHP_INI_STAGE_HTACCESS);

    if (*fn_next) {
        rc = (*fn_next)(ctx, fn_next + 1);
    }

    return rc;
}

static int lsapi_activate_user_ini( void )
{
    _lsapi_activate_user_ini_ctx ctx;
    /**
     * The reason to have this function list stacked
     * is each function now can define a scoped destructor.
     *
     * Passing control via function pointer is a sign of low coupling,
     * which means dependencies between these functions are to be
     * controlled from a single place
     * (here below, by modifying this function list order)
     */
    static const fn_activate_user_ini_chain_t fn_chain[] = {
        &lsapi_activate_user_ini_basic_checks,
        &lsapi_activate_user_ini_mk_path,
        &lsapi_activate_user_ini_mk_realpath,
        &lsapi_activate_user_ini_mk_user_config,
        &lsapi_activate_user_ini_walk_down_the_path,
        &lsapi_activate_user_ini_finally,
        NULL
    };

    return fn_chain[0](&ctx, (fn_activate_user_ini_chain_t*)(fn_chain + 1));
}


static void override_ini()
{

    LSAPI_ForeachSpecialEnv( alter_ini, NULL );

}


static int processReq(void)
{
    int ret = 0;
    zend_first_try {

        /* avoid server_context==NULL checks */
        SG(server_context) = (void *) 1;

        engine = 1;
        override_ini();

        if ( engine ) {
            init_request_info();

            if ( lsapi_module_main( source_highlight ) == -1 ) {
                ret = -1;
            }
        } else {
            LSAPI_AppendRespHeader( "status: 403", 11 );
            LSAPI_AppendRespHeader( "content-type: text/html", 23 );
            LSAPI_Write( "Forbidden: PHP engine is disable.\n", 34 );
        }
    } zend_end_try();
    return ret;
}

static void cli_usage(void)
{
    static const char * usage =
        "Usage: php\n"
        "      php -[b|c|n|h|i|q|s|v|?] [<file>] [args...]\n"
        "  Run in LSAPI mode, only '-b', '-s' and '-c' are effective\n"
        "  Run in Command Line Interpreter mode when parameters are specified\n"
        "\n"
        "  -b <address:port>|<port> Bind Path for external LSAPI Server mode\n"
        "  -c <path>|<file> Look for php.ini file in this directory\n"
        "  -n    No php.ini file will be used\n"
        "  -h    This help\n"
        "  -i    PHP information\n"
        "  -l    Syntax check\n"
        "  -q    Quiet-mode.  Suppress HTTP Header output.\n"
        "  -s    Display colour syntax highlighted source.\n"
        "  -v    Version number\n"
        "  -?    This help\n"
        "\n"
        "  args...    Arguments passed to script.\n";
    php_output_startup();
    php_output_activate();
    php_printf( "%s", usage );
#ifdef PHP_OUTPUT_NEWAPI
    php_output_end_all();
#else
    php_end_ob_buffers(1);
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
            *php_bind = strdup(*p++);
            break;

        case 'c':
            if ( p >= argend ) {
                fprintf( stderr, "<path> or <file> must be specified following '-c' option.\n");

                return -1;
            }
            *php_ini_path = strdup( *p++ );
            break;
        case 's':
            source_highlight = 1;
            break;
        case 'n':
            ignore_php_ini = 1;
            break;
        case '?':
            if ( *((*(p-1))+2) == 's' )
                exit( 99 );
        case 'h':
        case 'i':
        case 'l':
        case 'q':
        case 'v':
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
    int ret = -1;
    int c;
    zend_string *psKey;
    lsapi_mode = 0;        /* enter CLI mode */

    zend_first_try     {
        SG(server_context) = (void *) 1;

        zend_uv.html_errors = 0; /* tell the engine we're in non-html mode */
        CG(in_compilation) = 0; /* not initialized but needed for several options */
        SG(options) |= SAPI_OPTION_NO_CHDIR;

#if PHP_MAJOR_VERSION < 7
        EG(uninitialized_zval_ptr) = NULL;
#endif
        for( ini = ini_defaults; *ini; ini+=2 ) {
            psKey = zend_string_init(*ini, strlen( *ini ), 1);
            zend_alter_ini_entry_chars(psKey,
                                (char *)*(ini+1), strlen( *(ini+1) ),
                                PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
            zend_string_release_ex(psKey, 1);
        }

        while (( p < argend )&&(**p == '-' )) {
            c = *((*p)+1);
            ++p;
            switch( c ) {
            case 'q':
                break;
            case 'i':
                if (php_request_startup() != FAILURE) {
                    php_print_info(0xFFFFFFFF);
#ifdef PHP_OUTPUT_NEWAPI
                    php_output_end_all();
#else
                    php_end_ob_buffers(1);
#endif
                    php_request_shutdown( NULL );
                    ret = 0;
                }
                break;
            case 'v':
                if (php_request_startup() != FAILURE) {
#if ZEND_DEBUG
                    php_printf("PHP %s (%s) (built: %s %s) (DEBUG)\nCopyright (c) The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
#else
                    php_printf("PHP %s (%s) (built: %s %s)\nCopyright (c) The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
#endif
#ifdef PHP_OUTPUT_NEWAPI
                    php_output_end_all();
#else
                    php_end_ob_buffers(1);
#endif
                    php_request_shutdown( NULL );
                    ret = 0;
                }
                break;
            case 'c':
                ++p;
            /* fall through */
            case 's':
                break;
            case 'l':
                source_highlight = 2;
                break;
            case 'h':
            case '?':
            default:
                cli_usage();
                ret = 0;
                break;

            }
        }
        if ( ret == -1 ) {
            if ( *p ) {
                zend_file_handle file_handle;
				zend_stream_init_fp(&file_handle, VCWD_FOPEN(*p, "rb"), NULL);

                if ( file_handle.handle.fp ) {
                    script_filename = *p;
                    php_self = *p;

                    SG(request_info).path_translated = estrdup(*p);
                    SG(request_info).argc = argc - (p - argv);
                    SG(request_info).argv = p;

                    if (php_request_startup() == FAILURE ) {
                        fclose( file_handle.handle.fp );
                        ret = 2;
                    } else {
                        if (source_highlight == 1) {
                            zend_syntax_highlighter_ini syntax_highlighter_ini;

                            php_get_highlight_struct(&syntax_highlighter_ini);
                            highlight_file(SG(request_info).path_translated, &syntax_highlighter_ini);
                        } else if (source_highlight == 2) {
                            file_handle.filename = *p;
                            file_handle.free_filename = 0;
                            file_handle.opened_path = NULL;
                            ret = php_lint_script(&file_handle);
                            if (ret==SUCCESS) {
                                zend_printf("No syntax errors detected in %s\n", file_handle.filename);
                            } else {
                                zend_printf("Errors parsing %s\n", file_handle.filename);
                            }

                        } else {
                            file_handle.filename = *p;
                            file_handle.free_filename = 0;
                            file_handle.opened_path = NULL;

                            php_execute_script(&file_handle);
                            ret = EG(exit_status);
                       }

                        php_request_shutdown( NULL );
                    }
                } else {
                    php_printf("Could not open input file: %s.\n", *p);
                }
            } else {
                cli_usage();
            }
        }

    }zend_end_try();

    php_module_shutdown();

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
    sigemptyset(&act.sa_mask);
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

void setArgv0( int argc, char * argv[] )
{
    char * p;
    int i;
    argv0 = argv[0] + strlen( argv[0] );
    p = argv0;
    while(( p > argv[0] )&&( p[-1] != '/'))
        --p;
    if ( p > argv[0] )
    {
        memmove( argv[0], p, argv0 - p );
        memset( argv[0] + ( argv0 - p ), 0, p - argv[0] );
        argv0 = argv[0] + (argv0 - p);
    }
    for( i = 1; i < argc; ++i )
    {
        memset( argv[i], 0, strlen( argv[i] ) );
    }
}

#include <fcntl.h>
int main( int argc, char * argv[] )
{
    int ret;
    int bindFd;

    char * php_ini_path = NULL;
    char * php_bind     = NULL;
    int n;
    int climode = 0;
    struct timeval tv_req_begin;
    struct timeval tv_req_end;
    int slow_script_msec = 0;
    char time_buf[40];


#if defined(SIGPIPE) && defined(SIG_IGN)
    signal(SIGPIPE, SIG_IGN);
#endif

#ifdef ZTS
    php_tsrm_startup();
#endif

#if PHP_MAJOR_VERSION >= 7
#if defined(ZEND_SIGNALS) || PHP_MINOR_VERSION > 0
    zend_signal_startup();
#endif
#endif

    if (argc > 1 ) {
        if ( parse_opt( argc, argv, &climode,
                &php_ini_path, &php_bind ) == -1 ) {
            return 1;
        }
    }
    if ( climode ) {
        lsapi_sapi_module.phpinfo_as_text = 1;
    } else {
        setArgv0(argc, argv );
    }

    sapi_startup(&lsapi_sapi_module);

#ifdef ZTS
    compiler_globals = ts_resource(compiler_globals_id);
    executor_globals = ts_resource(executor_globals_id);
    core_globals = ts_resource(core_globals_id);
    sapi_globals = ts_resource(sapi_globals_id);

    SG(request_info).path_translated = NULL;
#endif

    lsapi_sapi_module.executable_location = argv[0];

    /* Initialize from environment variables before processing command-line
     * options: the latter override the former.
     */
    init_sapi_from_env(&lsapi_sapi_module);

    if ( ignore_php_ini )
        lsapi_sapi_module.php_ini_ignore = 1;

    if ( php_ini_path ) {
        lsapi_sapi_module.php_ini_path_override = php_ini_path;
    }


    lsapi_sapi_module.ini_defaults = sapi_lsapi_ini_defaults;

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

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
    int is_criu = LSCRIU_Init(); // Must be called before the regular init as it unsets the parameters.
#endif

    LSAPI_Init_Env_Parameters( NULL );
    lsapi_mode = 1;

    slow_script_msec = LSAPI_Get_Slow_Req_Msecs();

    if ( php_bind ) {
        LSAPI_No_Check_ppid();
        free( php_bind );
        php_bind = NULL;
    }

    int result;

    if (do_clean_shutdown)
        atexit(lsapi_atexit);

    while( ( result = LSAPI_Prefork_Accept_r( &g_req )) >= 0 ) {
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
        if (is_criu && !result) {
            LSCRIU_inc_req_procssed();
        }
#endif
        if ( slow_script_msec ) {
            gettimeofday( &tv_req_begin, NULL );
        }
        ret = processReq();
        if ( slow_script_msec ) {
            gettimeofday( &tv_req_end, NULL );
            n = ((long) tv_req_end.tv_sec - tv_req_begin.tv_sec ) * 1000
                + (tv_req_end.tv_usec - tv_req_begin.tv_usec) / 1000;
            if ( n > slow_script_msec )
            {
                strftime( time_buf, 30, "%d/%b/%Y:%H:%M:%S", localtime( &tv_req_end.tv_sec ) );
                fprintf( stderr, "[%s] Slow PHP script: %d ms\n  URL: %s %s\n  Query String: %s\n  Script: %s\n",
                         time_buf, n,  LSAPI_GetRequestMethod(),
                         LSAPI_GetScriptName(), LSAPI_GetQueryString(),
                         LSAPI_GetScriptFileName() );

            }
        }
        LSAPI_Finish();
        if ( ret ) {
            break;
        }
    }

    php_module_shutdown();

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
PHP_FUNCTION(apache_get_modules);
PHP_FUNCTION(litespeed_finish_request);

PHP_MINFO_FUNCTION(litespeed);

static const zend_function_entry litespeed_functions[] = {
    PHP_FE(litespeed_request_headers,   arginfo_litespeed__void)
    PHP_FE(litespeed_response_headers,  arginfo_litespeed__void)
    PHP_FE(apache_get_modules,          arginfo_litespeed__void)
    PHP_FE(litespeed_finish_request,    arginfo_litespeed__void)
    PHP_FALIAS(getallheaders,           litespeed_request_headers,  arginfo_litespeed__void)
    PHP_FALIAS(apache_request_headers,  litespeed_request_headers,  arginfo_litespeed__void)
    PHP_FALIAS(apache_response_headers, litespeed_response_headers, arginfo_litespeed__void)
    {NULL, NULL, NULL}
};

static PHP_MINIT_FUNCTION(litespeed)
{
    user_config_cache_init();

    const char *p = getenv("LSPHP_ENABLE_USER_INI");
    if (p && 0 == strcasecmp(p, "on"))
        parse_user_ini = 1;

    p = getenv("LSAPI_CLEAN_SHUTDOWN");
    if (p) {
        if (*p == '1' || 0 == strcasecmp(p, "on"))
            do_clean_shutdown = 1;
        else if (*p == '0' || 0 == strcasecmp(p, "off"))
            do_clean_shutdown = 0;
    }
    /*
     * mod_lsapi always sets this env var,
     * so we can detect mod_lsapi mode with its presense.
     */
    mod_lsapi_mode = ( getenv("LSAPI_DISABLE_CPAN_BEHAV") != NULL );

    /* REGISTER_INI_ENTRIES(); */
    return SUCCESS;
}


static PHP_MSHUTDOWN_FUNCTION(litespeed)
{
    zend_hash_destroy(&user_config_cache);

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
    PHP_VERSION,
    STANDARD_MODULE_PROPERTIES
};

static int add_associate_array( const char * pKey, int keyLen, const char * pValue, int valLen,
                         void * arg )
{
    add_assoc_string_ex((zval *)arg, (char *)pKey, keyLen, (char *)pValue);
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
    char *       p;
    int          len;
    char         headerBuf[SAPI_LSAPI_MAX_HEADER_LENGTH];

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
                    add_assoc_string_ex(return_value, headerBuf, len, p);
                }
            }
        }
        h = zend_llist_get_next_ex(&SG(sapi_headers).headers, &pos);
    }
}

/* }}} */


/* {{{ proto array apache_get_modules(void)
   Fetch all loaded module names  */
PHP_FUNCTION(apache_get_modules)
{
    static const char * mod_names[] =
    {
        "mod_rewrite", "mod_mime", "mod_headers", "mod_expires", "mod_auth_basic", NULL
    };
    const char **name = mod_names;
    /* TODO: */
    if (ZEND_NUM_ARGS() > 0) {
        WRONG_PARAM_COUNT;
    }
    array_init(return_value);
    while( *name )
    {
        add_next_index_string(return_value, *name);
        ++name;
    }
}
/* }}} */


/* {{{ proto array litespeed_finish_request(void)
   Flushes all response data to the client */
PHP_FUNCTION(litespeed_finish_request)
{
    if (ZEND_NUM_ARGS() > 0) {
        WRONG_PARAM_COUNT;
    }

    php_output_end_all();
    php_header();

    if (LSAPI_End_Response() != -1) {
        RETURN_TRUE;
    }
    RETURN_FALSE;
}
/* }}} */
