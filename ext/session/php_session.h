/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
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

typedef enum {
	php_session_disabled,
	php_session_none,
	php_session_active,
} php_session_status;

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
	php_session_status session_status;
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
#define PS(v) TSRMG(ps_globals_id, php_ps_globals *, v)
#else
#define PS(v) (ps_globals.v)
#endif

#define PS_SERIALIZER_ENCODE_ARGS char **newstr, int *newlen TSRMLS_DC
#define PS_SERIALIZER_DECODE_ARGS const char *val, int vallen TSRMLS_DC

typedef struct ps_serializer_struct {
	const char *name;
	int (*encode)(PS_SERIALIZER_ENCODE_ARGS);
	int (*decode)(PS_SERIALIZER_DECODE_ARGS);
} ps_serializer;

#define PS_SERIALIZER_ENCODE_NAME(x) ps_srlzr_encode_##x
#define PS_SERIALIZER_DECODE_NAME(x) ps_srlzr_decode_##x

#define PS_SERIALIZER_ENCODE_FUNC(x) \
	int PS_SERIALIZER_ENCODE_NAME(x)(PS_SERIALIZER_ENCODE_ARGS)
#define PS_SERIALIZER_DECODE_FUNC(x) \
	int PS_SERIALIZER_DECODE_NAME(x)(PS_SERIALIZER_DECODE_ARGS)

#define PS_SERIALIZER_FUNCS(x) \
	PS_SERIALIZER_ENCODE_FUNC(x); \
	PS_SERIALIZER_DECODE_FUNC(x)

#define PS_SERIALIZER_ENTRY(x) \
	{ #x, PS_SERIALIZER_ENCODE_NAME(x), PS_SERIALIZER_DECODE_NAME(x) }

#ifdef TRANS_SID
void session_adapt_uris(const char *, size_t, char **, size_t * TSRMLS_DC);
void session_adapt_url(const char *, size_t, char **, size_t * TSRMLS_DC);
void session_adapt_flush(int (*)(const char *, uint) TSRMLS_DC);
#else
#define session_adapt_uris(a,b,c,d) do { } while(0)
#define session_adapt_url(a,b,c,d) do { } while(0)
#define session_adapt_flush(a) do { } while(0)
#endif

void php_set_session_var(char *name, size_t namelen, zval *state_val,HashTable *var_hash TSRMLS_DC);
int php_get_session_var(char *name, size_t namelen, zval ***state_var TSRMLS_DC);

int php_session_register_module(ps_module *);

int php_session_register_serializer(const char *name,
	        int (*encode)(PS_SERIALIZER_ENCODE_ARGS),
	        int (*decode)(PS_SERIALIZER_DECODE_ARGS));

#define PS_ADD_VARL(name,namelen) \
	zend_hash_add_empty_element(&PS(vars), name, namelen + 1)

#define PS_ADD_VAR(name) PS_ADD_VARL(name, strlen(name))

#define PS_DEL_VARL(name,namelen) \
	zend_hash_del(&PS(vars), name, namelen + 1);

#define PS_DEL_VAR(name) PS_DEL_VARL(name, strlen(name))

#define PS_ENCODE_VARS 											\
	char *key;													\
	ulong key_length;											\
	ulong num_key;												\
	zval **struc;

#define PS_ENCODE_LOOP(code)										\
	for (zend_hash_internal_pointer_reset(&PS(vars));			\
			zend_hash_get_current_key_ex(&PS(vars), &key, &key_length, &num_key, 0, NULL) == HASH_KEY_IS_STRING; \
			zend_hash_move_forward(&PS(vars))) {				\
			key_length--;										\
		if (php_get_session_var(key, key_length, &struc TSRMLS_CC) == SUCCESS) { \
			code;		 										\
		} 														\
	}

#ifdef ZTS
extern int ps_globals_id;
#else
extern php_ps_globals ps_globals;
#endif


void php_session_auto_start(void *data);
void php_session_shutdown(void *data);

#endif
