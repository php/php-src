/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 4.0                     |
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
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_SESSION_H
#define PHP_SESSION_H

#define PS_OPEN_ARGS void **mod_data, const char *save_path, const char *session_name
#define PS_CLOSE_ARGS void **mod_data
#define PS_READ_ARGS void **mod_data, const char *key, char **val, int *vallen
#define PS_WRITE_ARGS void **mod_data, const char *key, const char *val, const int vallen
#define PS_DESTROY_ARGS void **mod_data, const char *key
#define PS_GC_ARGS void **mod_data, int maxlifetime, int *nrdels

typedef struct ps_module_struct {
	const char *name;
	int (*open)(PS_OPEN_ARGS);
	int (*close)(PS_CLOSE_ARGS);
	int (*read)(PS_READ_ARGS);
	int (*write)(PS_WRITE_ARGS);
	int (*destroy)(PS_DESTROY_ARGS);
	int (*gc)(PS_GC_ARGS);
} ps_module;

#define PS_GET_MOD_DATA() *mod_data
#define PS_SET_MOD_DATA(a) *mod_data = (a)

#define PS_OPEN_FUNC(x) 	int ps_open_##x(PS_OPEN_ARGS)
#define PS_CLOSE_FUNC(x) 	int ps_close_##x(PS_CLOSE_ARGS)
#define PS_READ_FUNC(x) 	int ps_read_##x(PS_READ_ARGS)
#define PS_WRITE_FUNC(x) 	int ps_write_##x(PS_WRITE_ARGS)
#define PS_DESTROY_FUNC(x) 	int ps_delete_##x(PS_DESTROY_ARGS)
#define PS_GC_FUNC(x) 		int ps_gc_##x(PS_GC_ARGS)

#define PS_FUNCS(x) \
	PS_OPEN_FUNC(x); \
	PS_CLOSE_FUNC(x); \
	PS_READ_FUNC(x); \
	PS_WRITE_FUNC(x); \
	PS_DESTROY_FUNC(x); \
	PS_GC_FUNC(x)


#define PS_MOD(x) \
	#x, ps_open_##x, ps_close_##x, ps_read_##x, ps_write_##x, \
	 ps_delete_##x, ps_gc_##x 

typedef struct {
	char *save_path;
	char *session_name;
	char *id;
	char *extern_referer_chk;
	char *entropy_file;
	char *cache_limiter;
	long entropy_length;
	long cookie_lifetime;
	char *cookie_path;
	char *cookie_domain;
    zend_bool  cookie_secure;
	ps_module *mod;
	void *mod_data;
	HashTable vars;
	int nr_open_sessions;
	long gc_probability;
	long gc_maxlifetime;
	int module_number;
	long cache_expire;
	const struct ps_serializer_struct *serializer;
	zval *http_session_vars;
	zend_bool auto_start;
	zend_bool define_sid;
	zend_bool use_cookies;
} php_ps_globals;

extern zend_module_entry session_module_entry;
#define phpext_session_ptr &session_module_entry

PHP_FUNCTION(session_name);
PHP_FUNCTION(session_module_name);
PHP_FUNCTION(session_save_path);
PHP_FUNCTION(session_id);
PHP_FUNCTION(session_decode);
PHP_FUNCTION(session_register);
PHP_FUNCTION(session_unregister);
PHP_FUNCTION(session_is_registered);
PHP_FUNCTION(session_encode);
PHP_FUNCTION(session_start);
PHP_FUNCTION(session_destroy);
PHP_FUNCTION(session_unset);
PHP_FUNCTION(session_set_save_handler);
PHP_FUNCTION(session_cache_limiter);
PHP_FUNCTION(session_set_cookie_params);
PHP_FUNCTION(session_get_cookie_params);
PHP_FUNCTION(session_write_close);

#ifdef ZTS
#define PSLS_D php_ps_globals *ps_globals
#define PSLS_DC , PSLS_D
#define PSLS_C ps_globals
#define PSLS_CC , PSLS_C
#define PS(v) (ps_globals->v)
#define PSLS_FETCH() php_ps_globals *ps_globals = ts_resource(ps_globals_id)
#else
#define PSLS_D	void
#define PSLS_DC
#define PSLS_C
#define PSLS_CC
#define PS(v) (ps_globals.v)
#define PSLS_FETCH()
#endif

#define PS_SERIALIZER_ENCODE_ARGS char **newstr, int *newlen PSLS_DC
#define PS_SERIALIZER_DECODE_ARGS const char *val, int vallen PSLS_DC

typedef struct ps_serializer_struct {
	const char *name;
	int (*encode)(PS_SERIALIZER_ENCODE_ARGS);
	int (*decode)(PS_SERIALIZER_DECODE_ARGS);
} ps_serializer;

#define PS_SERIALIZER_ENCODE_FUNC(x) \
	int ps_srlzr_encode_##x(PS_SERIALIZER_ENCODE_ARGS)
#define PS_SERIALIZER_DECODE_FUNC(x) \
	int ps_srlzr_decode_##x(PS_SERIALIZER_DECODE_ARGS)

#define PS_SERIALIZER_FUNCS(x) \
	PS_SERIALIZER_ENCODE_FUNC(x); \
	PS_SERIALIZER_DECODE_FUNC(x)

#define PS_SERIALIZER_ENTRY(x) \
	{ #x,ps_srlzr_encode_##x, ps_srlzr_decode_##x }

#ifdef TRANS_SID
void session_adapt_uris(const char *, size_t, char **, size_t *);
void session_adapt_url(const char *, size_t, char **, size_t *);
#else
#define session_adapt_uris(a,b,c,d)
#define session_adapt_url(a,b,c,d)
#endif

void php_session_auto_start(void *data);
void php_session_shutdown(void *data);

#if HAVE_WDDX
#define WDDX_SERIALIZER
#include "ext/wddx/php_wddx_api.h"
#endif

#endif
