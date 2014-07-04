/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_SESSION_H
#define PHP_SESSION_H

#include "ext/standard/php_var.h"

#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH)
# include "ext/hash/php_hash.h"
#endif

#define PHP_SESSION_API 20020330

/* To check php_session_valid_key()/php_session_reset_id() */
#define PHP_SESSION_STRICT 1

#define PS_OPEN_ARGS void **mod_data, const char *save_path, const char *session_name TSRMLS_DC
#define PS_CLOSE_ARGS void **mod_data TSRMLS_DC
#define PS_READ_ARGS void **mod_data, const char *key, char **val, int *vallen TSRMLS_DC
#define PS_WRITE_ARGS void **mod_data, const char *key, const char *val, const int vallen TSRMLS_DC
#define PS_DESTROY_ARGS void **mod_data, const char *key TSRMLS_DC
#define PS_GC_ARGS void **mod_data, int maxlifetime, int *nrdels TSRMLS_DC
#define PS_CREATE_SID_ARGS void **mod_data, int *newlen TSRMLS_DC

/* default create id function */
PHPAPI char *php_session_create_id(PS_CREATE_SID_ARGS);

typedef struct ps_module_struct {
	const char *s_name;
	int (*s_open)(PS_OPEN_ARGS);
	int (*s_close)(PS_CLOSE_ARGS);
	int (*s_read)(PS_READ_ARGS);
	int (*s_write)(PS_WRITE_ARGS);
	int (*s_destroy)(PS_DESTROY_ARGS);
	int (*s_gc)(PS_GC_ARGS);
	char *(*s_create_sid)(PS_CREATE_SID_ARGS);
} ps_module;

#define PS_GET_MOD_DATA() *mod_data
#define PS_SET_MOD_DATA(a) *mod_data = (a)

#define PS_OPEN_FUNC(x) 	int ps_open_##x(PS_OPEN_ARGS)
#define PS_CLOSE_FUNC(x) 	int ps_close_##x(PS_CLOSE_ARGS)
#define PS_READ_FUNC(x) 	int ps_read_##x(PS_READ_ARGS)
#define PS_WRITE_FUNC(x) 	int ps_write_##x(PS_WRITE_ARGS)
#define PS_DESTROY_FUNC(x) 	int ps_delete_##x(PS_DESTROY_ARGS)
#define PS_GC_FUNC(x) 		int ps_gc_##x(PS_GC_ARGS)
#define PS_CREATE_SID_FUNC(x)	char *ps_create_sid_##x(PS_CREATE_SID_ARGS)

#define PS_FUNCS(x) \
	PS_OPEN_FUNC(x); \
	PS_CLOSE_FUNC(x); \
	PS_READ_FUNC(x); \
	PS_WRITE_FUNC(x); \
	PS_DESTROY_FUNC(x); \
	PS_GC_FUNC(x);	\
	PS_CREATE_SID_FUNC(x)

#define PS_MOD(x) \
	#x, ps_open_##x, ps_close_##x, ps_read_##x, ps_write_##x, \
	 ps_delete_##x, ps_gc_##x, php_session_create_id

/* SID creation enabled module handler definitions */
#define PS_FUNCS_SID(x) \
	PS_OPEN_FUNC(x); \
	PS_CLOSE_FUNC(x); \
	PS_READ_FUNC(x); \
	PS_WRITE_FUNC(x); \
	PS_DESTROY_FUNC(x); \
	PS_GC_FUNC(x); \
	PS_CREATE_SID_FUNC(x)

#define PS_MOD_SID(x) \
	#x, ps_open_##x, ps_close_##x, ps_read_##x, ps_write_##x, \
	 ps_delete_##x, ps_gc_##x, ps_create_sid_##x

typedef enum {
	php_session_disabled,
	php_session_none,
	php_session_active
} php_session_status;

typedef struct _php_session_rfc1867_progress {

	size_t    sname_len;
	zval      sid;
	smart_str key;

	long      update_step;
	long      next_update;
	double    next_update_time;
	zend_bool cancel_upload;
	zend_bool apply_trans_sid;
	size_t    content_length;

	zval      *data;                 /* the array exported to session data */
	zval      *post_bytes_processed; /* data["bytes_processed"] */
	zval      *files;                /* data["files"] array */
	zval      *current_file;         /* array of currently uploading file */
	zval      *current_file_bytes_processed;
} php_session_rfc1867_progress;

typedef struct _php_ps_globals {
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
	zend_bool  cookie_httponly;
	ps_module *mod;
	ps_module *default_mod;
	void *mod_data;
	php_session_status session_status;
	long gc_probability;
	long gc_divisor;
	long gc_maxlifetime;
	int module_number;
	long cache_expire;
	union {
		zval *names[7];
		struct {
			zval *ps_open;
			zval *ps_close;
			zval *ps_read;
			zval *ps_write;
			zval *ps_destroy;
			zval *ps_gc;
			zval *ps_create_sid;
		} name;
	} mod_user_names;
	int mod_user_implemented;
	int mod_user_is_open;
	const struct ps_serializer_struct *serializer;
	zval *http_session_vars;
	zend_bool auto_start;
	zend_bool use_cookies;
	zend_bool use_only_cookies;
	zend_bool use_trans_sid;	/* contains the INI value of whether to use trans-sid */
	zend_bool apply_trans_sid;	/* whether or not to enable trans-sid for the current request */

	long hash_func;
#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH)
	php_hash_ops *hash_ops;
#endif
	long hash_bits_per_character;
	int send_cookie;
	int define_sid;
	zend_bool invalid_session_id;	/* allows the driver to report about an invalid session id and request id regeneration */

	php_session_rfc1867_progress *rfc1867_progress;
	zend_bool rfc1867_enabled; /* session.upload_progress.enabled */
	zend_bool rfc1867_cleanup; /* session.upload_progress.cleanup */
	smart_str rfc1867_prefix;  /* session.upload_progress.prefix */
	smart_str rfc1867_name;    /* session.upload_progress.name */
	long rfc1867_freq;         /* session.upload_progress.freq */
	double rfc1867_min_freq;   /* session.upload_progress.min_freq */

	zend_bool use_strict_mode; /* whether or not PHP accepts unknown session ids */
	unsigned char session_data_hash[16]; /* binary MD5 hash length */
} php_ps_globals;

typedef php_ps_globals zend_ps_globals;

extern zend_module_entry session_module_entry;
#define phpext_session_ptr &session_module_entry

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

PHPAPI void session_adapt_url(const char *, size_t, char **, size_t * TSRMLS_DC);

PHPAPI void php_add_session_var(char *name, size_t namelen TSRMLS_DC);
PHPAPI void php_set_session_var(char *name, size_t namelen, zval *state_val, php_unserialize_data_t *var_hash TSRMLS_DC);
PHPAPI int php_get_session_var(char *name, size_t namelen, zval ***state_var TSRMLS_DC);

PHPAPI int php_session_register_module(ps_module *);

PHPAPI int php_session_register_serializer(const char *name,
	        int (*encode)(PS_SERIALIZER_ENCODE_ARGS),
	        int (*decode)(PS_SERIALIZER_DECODE_ARGS));

PHPAPI void php_session_set_id(char *id TSRMLS_DC);
PHPAPI void php_session_start(TSRMLS_D);

PHPAPI ps_module *_php_find_ps_module(char *name TSRMLS_DC);
PHPAPI const ps_serializer *_php_find_ps_serializer(char *name TSRMLS_DC);

PHPAPI int php_session_valid_key(const char *key);
PHPAPI void php_session_reset_id(TSRMLS_D);

#define PS_ADD_VARL(name,namelen) do {										\
	php_add_session_var(name, namelen TSRMLS_CC);							\
} while (0)

#define PS_ADD_VAR(name) PS_ADD_VARL(name, strlen(name))

#define PS_DEL_VARL(name,namelen) do {										\
	if (PS(http_session_vars)) {											\
		zend_hash_del(Z_ARRVAL_P(PS(http_session_vars)), name, namelen+1);	\
	}																		\
} while (0)


#define PS_ENCODE_VARS 											\
	char *key;													\
	uint key_length;											\
	ulong num_key;												\
	zval **struc;

#define PS_ENCODE_LOOP(code) do {									\
		HashTable *_ht = Z_ARRVAL_P(PS(http_session_vars));			\
		int key_type;												\
																	\
		for (zend_hash_internal_pointer_reset(_ht);					\
				(key_type = zend_hash_get_current_key_ex(_ht, &key, &key_length, &num_key, 0, NULL)) != HASH_KEY_NON_EXISTENT; \
					zend_hash_move_forward(_ht)) {					\
			if (key_type == HASH_KEY_IS_LONG) {						\
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Skipping numeric key %ld", num_key);	\
				continue;											\
			}														\
			key_length--;											\
			if (php_get_session_var(key, key_length, &struc TSRMLS_CC) == SUCCESS) {	\
				code;		 										\
			} 														\
		}															\
	} while(0)

PHPAPI ZEND_EXTERN_MODULE_GLOBALS(ps)

void php_session_auto_start(void *data);
void php_session_shutdown(void *data);

#define PS_CLASS_NAME "SessionHandler"
extern zend_class_entry *php_session_class_entry;

#define PS_IFACE_NAME "SessionHandlerInterface"
extern zend_class_entry *php_session_iface_entry;

#define PS_SID_IFACE_NAME "SessionIdInterface"
extern zend_class_entry *php_session_id_iface_entry;

extern PHP_METHOD(SessionHandler, open);
extern PHP_METHOD(SessionHandler, close);
extern PHP_METHOD(SessionHandler, read);
extern PHP_METHOD(SessionHandler, write);
extern PHP_METHOD(SessionHandler, destroy);
extern PHP_METHOD(SessionHandler, gc);
extern PHP_METHOD(SessionHandler, create_sid);

#endif
