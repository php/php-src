/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef _PHP_H
#define _PHP_H

#define PHP_API_VERSION 19990421

#define YYDEBUG 0

#define CGI_BINARY (!APACHE && !USE_SAPI && !FHTTPD)

#include "php_version.h"
#include "zend.h"

/* automake defines PACKAGE and VERSION for Zend too */
#ifdef PACKAGE
# undef PACKAGE
#endif
#ifdef VERSION
# undef VERSION
#endif

#include "zend_API.h"


extern unsigned char first_arg_force_ref[];   
extern unsigned char first_arg_allow_ref[];
extern unsigned char second_arg_force_ref[];
extern unsigned char second_arg_allow_ref[];


/* somebody stealing BOOL from windows.  pick something else!
#ifndef BOOL
#define BOOL MYBOOL
#endif
*/


#if WIN32
#include "config.w32.h"
#include "win95nt.h"
#	ifdef PHP_EXPORTS
#	define PHPAPI __declspec(dllexport) 
#	else
#	define PHPAPI __declspec(dllimport) 
#	endif
#else
#include "php_config.h"
#define PHPAPI
#define THREAD_LS
#endif


/* PHP's DEBUG value must match Zend's ZEND_DEBUG value */
#undef DEBUG
#define DEBUG ZEND_DEBUG


#if DEBUG || !(defined(__GNUC__)||defined(WIN32))
#ifdef inline
#undef inline
#endif
#define inline
#endif


#if HAVE_UNIX_H
#include <unix.h>
#endif

#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include "request_info.h"

#if HAVE_LIBDL
# if MSVC5
#  include <windows.h>
#  define dlclose FreeLibrary
#  define dlopen(a,b) LoadLibrary(a)
#  define dlsym GetProcAddress
# else
#if HAVE_DLFCN_H && !(defined(_AIX) && APACHE)
#  include <dlfcn.h>
#endif
# endif
#endif

#define CREATE_MUTEX(a,b)
#define SET_MUTEX(a)
#define FREE_MUTEX(a)

/*
 * Then the ODBC support can use both iodbc and Solid,
 * uncomment this.
 * #define HAVE_ODBC (HAVE_IODBC|HAVE_SOLID)
 */

#include <stdlib.h>
#include <ctype.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_STDARG_H
#include <stdarg.h>
#else   
# if HAVE_SYS_VARARGS_H
# include <sys/varargs.h>
# endif 
#endif 


#include "zend_hash.h"
#include "php3_compat.h"
#include "zend_alloc.h"
#include "zend_stack.h"

typedef zval pval;

#define pval_copy_constructor	zval_copy_ctor
#define pval_destructor			zval_dtor

#if STDC_HEADERS
# include <string.h>
#else
# ifndef HAVE_MEMCPY
#  define memcpy(d, s, n)	bcopy((s), (d), (n))
#  define memmove(d, s, n)	bcopy ((s), (d), (n))
# endif
#endif

#include "safe_mode.h"

#ifndef HAVE_STRERROR
extern char *strerror(int);
#endif

#include "fopen-wrappers.h"

#if APACHE /* apache httpd */
# if HAVE_AP_CONFIG_H
#include "ap_config.h"
# endif
# if HAVE_OLD_COMPAT_H
#include "compat.h"
# endif
# if HAVE_AP_COMPAT_H
#include "ap_compat.h"
# endif
#include "httpd.h"
#include "http_main.h"
#include "http_core.h"
#include "http_request.h"
#include "http_protocol.h"
#include "http_config.h"
#include "http_log.h"
#define BLOCK_INTERRUPTIONS block_alarms
#define UNBLOCK_INTERRUPTIONS unblock_alarms
#endif

#if HAVE_PWD_H
# if MSVC5
#include "win32/pwd.h"
#include "win32/param.h"
# else
#include <pwd.h>
#include <sys/param.h>
# endif
#endif
#if CGI_BINARY /* CGI version */
#define BLOCK_INTERRUPTIONS		NULL
#define UNBLOCK_INTERRUPTIONS	NULL
#endif

#if HAVE_LIMITS_H
#include <limits.h>
#endif

#ifndef LONG_MAX
#define LONG_MAX 2147483647L
#endif

#ifndef LONG_MIN
#define LONG_MIN (- LONG_MAX - 1)
#endif

#if FHTTPD /* fhttpd */
#define BLOCK_INTERRUPTIONS		NULL
#define UNBLOCK_INTERRUPTIONS	NULL
#endif

#if (!HAVE_SNPRINTF)
#define snprintf ap_snprintf
#define vsnprintf ap_vsnprintf
extern int ap_snprintf(char *, size_t, const char *, ...);
extern int ap_vsnprintf(char *, size_t, const char *, va_list);
#endif

#define EXEC_INPUT_BUF 4096


#if FHTTPD

#include <servproc.h>

#ifndef IDLE_TIMEOUT
#define IDLE_TIMEOUT 120
#endif
#ifndef SIGACTARGS
#define SIGACTARGS int n
#endif

extern struct http_server *server;
extern struct request *req;
extern struct httpresponse *response;
extern int global_alarmflag;
extern int idle_timeout;
extern int exit_status;
extern int headermade;
extern char **currentheader;
extern char *headerfirstline;
extern int headerlines;

void alarmhandler(SIGACTARGS);
void setalarm(int t);
int checkinput(int h);

extern PHPAPI void php3_fhttpd_free_header(void);
extern PHPAPI void php3_fhttpd_puts_header(char *s);
extern PHPAPI void php3_fhttpd_puts(char *s);
extern PHPAPI void php3_fhttpd_putc(char c);
extern PHPAPI int php3_fhttpd_write(char *a,int n);
# if !defined(COMPILE_DL)
# define PUTS(s) php3_fhttpd_puts(s)
# define PUTC(c) php3_fhttpd_putc(c)
# define PHPWRITE(a,n) php3_fhttpd_write((a),(n))
# endif
#endif



#define DONT_FREE 0
#define DO_FREE 1

#define PHP3_MIME_TYPE "application/x-httpd-php3"

/* macros */
#undef MIN
#undef MAX
#undef COPY_STRING
#define DO_OR_DIE(retvalue) if (retvalue==FAILURE) { return FAILURE; }
#define MAX(a,b)  (((a)>(b))?(a):(b))
#define MIN(a,b)  (((a)<(b))?(a):(b))
#define STR_FREE(ptr) if (ptr && ptr!=empty_string && ptr!=undefined_variable_string) { efree(ptr); }
#define COPY_STRING(yy)   (yy).value.str.val = (char *) estrndup((yy).value.str.val,(yy).value.str.len)

#ifndef MAXPATHLEN
#define MAXPATHLEN 256    /* Should be safe for any weird systems that do not define it */
#endif

#define PHP_NAMED_FUNCTION(name) void name(INTERNAL_FUNCTION_PARAMETERS)
#define PHP_FUNCTION(name) PHP_NAMED_FUNCTION(php3_##name)

#define PHP_NAMED_FE(php_name, name, arg_types) { #php_name, name, arg_types },
#define PHP_FE(name, arg_types) PHP_NAMED_FE(name, php3_##name, arg_types)
#define PHP_FALIAS(name, alias, arg_types) PHP_NAMED_FE(name, php3_##alias, arg_types)





/* global variables */
extern pval *data;
#if !(WIN32||WINNT)
extern char **environ;
#endif

extern PHPAPI int le_index_ptr;  /* list entry type for index pointers */

extern void phperror(char *error);
extern PHPAPI void php3_error(int type, const char *format,...);
extern PHPAPI int php3_printf(const char *format,...);
extern void php3_log_err(char *log_message);
extern int Debug(char *format,...);
extern int cfgparse(void);

extern void html_putc(char c);

#define zenderror phperror
#define zendlex phplex

#define phpparse zendparse
#define phprestart zendrestart
#define phpin zendin

/* functions */
#ifndef THREAD_SAFE
extern int end_current_file_execution(int *retval);
#endif
extern int module_startup_modules(void);

/* needed for modules only */
extern PHPAPI int php3i_get_le_fp(void);

/*from basic functions*/
extern PHPAPI int _php3_error_log(int opt_err,char *message,char *opt,char *headers);

PHPAPI int cfg_get_long(char *varname, long *result);
PHPAPI int cfg_get_double(char *varname, double *result);
PHPAPI int cfg_get_string(char *varname, char **result);


/* Output support */
#include "output.h"
#define PHPWRITE(str, str_len)		zend_body_write((str), (str_len))
#define PUTS(str)					zend_body_write((str), strlen((str)))
#define PUTC(c)						zend_body_write(&(c), 1), (c)
#define PHPWRITE_H(str, str_len)	zend_header_write((str), (str_len))
#define PUTS_H(str)					zend_header_write((str), strlen((str)))
#define PUTC_H(c)					zend_header_write(&(c), 1), (c)


#include "zend_operators.h"
#include "zend_variables.h"
#include "zend_constants.h"

#define RETVAL_LONG(l) { return_value->type = IS_LONG; \
                         return_value->value.lval = l; }
#define RETVAL_DOUBLE(d) { return_value->type = IS_DOUBLE; \
                           return_value->value.dval = d; }
#define RETVAL_STRING(s,duplicate) { char *__s=(s); \
			return_value->value.str.len = strlen(__s); \
			return_value->value.str.val = (duplicate?estrndup(__s,return_value->value.str.len):__s); \
			return_value->type = IS_STRING; }
#define RETVAL_STRINGL(s,l,duplicate) { char *__s=(s); int __l=l; \
			return_value->value.str.len = __l; \
			return_value->value.str.val = (duplicate?estrndup(__s,__l):__s); \
      			return_value->type = IS_STRING; }

#define RETVAL_FALSE  {var_reset(return_value);}
#define RETVAL_TRUE   RETVAL_LONG(1L)

#define RETURN_LONG(l) { return_value->type = IS_LONG; \
                         return_value->value.lval = l; \
                         return; }
#define RETURN_DOUBLE(d) { return_value->type = IS_DOUBLE; \
                           return_value->value.dval = d; \
                           return; }
#define RETURN_STRING(s,duplicate) { char *__s=(s); \
			return_value->value.str.len = strlen(__s); \
			return_value->value.str.val = (duplicate?estrndup(__s,return_value->value.str.len):__s); \
			return_value->type = IS_STRING; \
			return; }
#define RETURN_STRINGL(s,l,duplicate) { char *__s=(s); int __l=l; \
			return_value->value.str.len = __l; \
			return_value->value.str.val = (duplicate?estrndup(__s,__l):__s); \
      			return_value->type = IS_STRING; \
			return; }

/*#define RETURN_NEG    RETURN_LONG(-1L) */
#define RETURN_ZERO   RETURN_LONG(0L)
#define RETURN_FALSE  {var_reset(return_value); return;}
#define RETURN_TRUE   RETURN_LONG(1L)

#define SET_VAR_STRING(n,v) { \
                           { \
                               pval var; \
							   char *str=v; /* prevent 'v' from being evaluated more than once */ \
                               var.value.str.val = (str); \
                               var.value.str.len = strlen((str)); \
                               var.type = IS_STRING; \
                               _php3_hash_update(&EG(symbol_table), (n), strlen((n))+1, &var, sizeof(pval),NULL); \
                           } \
                       }
#define SET_VAR_STRINGL(n,v,l) { \
                           { \
                               pval var; \
                               char *name=(n); \
                               var.value.str.val = (v); \
                               var.value.str.len = (l); \
                               var.type = IS_STRING; \
                               _php3_hash_update(&EG(symbol_table), name, strlen(name)+1, &var, sizeof(pval),NULL); \
                           } \
                       }
#define SET_VAR_LONG(n,v) { \
                           { \
                               pval var; \
                               var.value.lval = (v); \
                               var.type = IS_LONG; \
                               _php3_hash_update(&EG(symbol_table), (n), strlen((n))+1, &var, sizeof(pval),NULL); \
                           } \
                       }
#define SET_VAR_DOUBLE(n,v) { \
                           { \
                               pval var; \
                               var.value.dval = (v); \
                               var.type = IS_DOUBLE; \
                               _php3_hash_update(&EG(symbol_table)), (n), strlen((n))+1, &var, sizeof(pval),NULL); \
                           } \
                       }

#ifndef THREAD_SAFE
extern int yylineno;
#endif
extern void phprestart(FILE *input_file);


/* Finding offsets of elements within structures.
 * Taken from the Apache code, which in turn, was taken from X code...
 */

#if defined(CRAY) || (defined(__arm) && !defined(LINUX))
#ifdef __STDC__
#define XtOffset(p_type,field) _Offsetof(p_type,field)
#else
#ifdef CRAY2
#define XtOffset(p_type,field) \
    (sizeof(int)*((unsigned int)&(((p_type)NULL)->field)))

#else /* !CRAY2 */

#define XtOffset(p_type,field) ((unsigned int)&(((p_type)NULL)->field))

#endif /* !CRAY2 */  
#endif /* __STDC__ */
#else /* ! (CRAY || __arm) */

#define XtOffset(p_type,field) \
    ((long) (((char *) (&(((p_type)NULL)->field))) - ((char *) NULL)))

#endif /* !CRAY */

#ifdef offsetof
#define XtOffsetOf(s_type,field) offsetof(s_type,field)
#else
#define XtOffsetOf(s_type,field) XtOffset(s_type*,field)
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
