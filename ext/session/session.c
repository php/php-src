/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
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
   |          Andrei Zmievski <andrei@ispi.net>                           |
   +----------------------------------------------------------------------+
 */

#include "php.h"

#ifdef PHP_WIN32
#include "win32/time.h"
#else
#include <sys/time.h>
#endif

#include <sys/stat.h>
#include <fcntl.h>

#include "php_ini.h"
#include "SAPI.h"
#include "php_session.h"
#include "ext/standard/md5.h"
#include "ext/standard/php_var.h"
#include "ext/standard/datetime.h"
#include "ext/standard/php_lcg.h"
#include "ext/standard/url_scanner.h"
#include "ext/standard/php_output.h"
#include "ext/standard/php_rand.h"                   /* for RAND_MAX */
#include "ext/standard/info.h"

#ifdef ZTS
int ps_globals_id;
#else
static php_ps_globals ps_globals;
#endif

#include "modules.c"

function_entry session_functions[] = {
	PHP_FE(session_name, NULL)
	PHP_FE(session_module_name, NULL)
	PHP_FE(session_save_path, NULL)
	PHP_FE(session_id, NULL)
	PHP_FE(session_decode, NULL)
	PHP_FE(session_register, NULL)
	PHP_FE(session_unregister, NULL)
	PHP_FE(session_is_registered, NULL)
	PHP_FE(session_encode, NULL)
	PHP_FE(session_start, NULL)
	PHP_FE(session_destroy, NULL)
	PHP_FE(session_unset, NULL)
	PHP_FE(session_set_save_handler, NULL)
	PHP_FE(session_cache_limiter, NULL)
	PHP_FE(session_set_cookie_params, NULL)
	PHP_FE(session_get_cookie_params, NULL)
	{0}
};

static ps_module *_php_find_ps_module(char *name PSLS_DC);
static const ps_serializer *_php_find_ps_serializer(char *name PSLS_DC);

static PHP_INI_MH(OnUpdateSaveHandler)
{
	PSLS_FETCH();
	
	PS(mod) = _php_find_ps_module(new_value PSLS_CC);
	if(!PS(mod)) {
	  php_error(E_ERROR,"Cannot find save handler %s",new_value);
	}
	return SUCCESS;
}


static PHP_INI_MH(OnUpdateSerializer)
{
	PSLS_FETCH();

	PS(serializer) = _php_find_ps_serializer(new_value PSLS_CC);
	if(!PS(serializer)) {
	  php_error(E_ERROR,"Cannot find serialization handler %s",new_value);
	}	  
	return SUCCESS;
}


static PHP_INI_MH(OnUpdateStringCopy)
{
	char **p;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base;

	base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	p = (char **) (base+(size_t) mh_arg1);

	if (*p && stage != PHP_INI_STAGE_STARTUP) {
		STR_FREE(*p);
	}

	if (stage != PHP_INI_STAGE_DEACTIVATE) {
		*p = estrdup(new_value);
	}
	return SUCCESS;
}

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("session.save_path",			"/tmp",			PHP_INI_ALL, OnUpdateStringCopy,		save_path,			php_ps_globals,	ps_globals)
	STD_PHP_INI_ENTRY("session.name",				"PHPSESSID",	PHP_INI_ALL, OnUpdateStringCopy,		session_name,		php_ps_globals,	ps_globals)
	PHP_INI_ENTRY("session.save_handler",			"files",		PHP_INI_ALL, OnUpdateSaveHandler)
	STD_PHP_INI_BOOLEAN("session.auto_start",		"0",			PHP_INI_ALL, OnUpdateBool,			auto_start,			php_ps_globals,	ps_globals)
	STD_PHP_INI_ENTRY("session.gc_probability",		"1",			PHP_INI_ALL, OnUpdateInt,			gc_probability,		php_ps_globals,	ps_globals)
	STD_PHP_INI_ENTRY("session.gc_maxlifetime",		"1440",			PHP_INI_ALL, OnUpdateInt,			gc_maxlifetime,		php_ps_globals,	ps_globals)
	PHP_INI_ENTRY("session.serialize_handler",		"php",			PHP_INI_ALL, OnUpdateSerializer)
	STD_PHP_INI_ENTRY("session.cookie_lifetime",	"0",			PHP_INI_ALL, OnUpdateInt,			cookie_lifetime,	php_ps_globals,	ps_globals)
	STD_PHP_INI_ENTRY("session.cookie_path",		"/",			PHP_INI_ALL, OnUpdateStringCopy,		cookie_path,		php_ps_globals,	ps_globals)
	STD_PHP_INI_ENTRY("session.cookie_domain",		"",				PHP_INI_ALL, OnUpdateStringCopy,		cookie_domain,		php_ps_globals,	ps_globals)
	STD_PHP_INI_BOOLEAN("session.use_cookies",		"1",			PHP_INI_ALL, OnUpdateBool,			use_cookies,		php_ps_globals,	ps_globals)
	STD_PHP_INI_ENTRY("session.referer_check",		"",				PHP_INI_ALL, OnUpdateStringCopy,		extern_referer_chk,	php_ps_globals,	ps_globals)
	STD_PHP_INI_ENTRY("session.entropy_file",		"",				PHP_INI_ALL, OnUpdateStringCopy,		entropy_file,		php_ps_globals,	ps_globals)
	STD_PHP_INI_ENTRY("session.entropy_length",		"0",			PHP_INI_ALL, OnUpdateInt,			entropy_length,		php_ps_globals,	ps_globals)
	STD_PHP_INI_ENTRY("session.cache_limiter",		"nocache",		PHP_INI_ALL, OnUpdateStringCopy,		cache_limiter,		php_ps_globals,	ps_globals)
	STD_PHP_INI_ENTRY("session.cache_expire",		"180",			PHP_INI_ALL, OnUpdateInt,			cache_expire,		php_ps_globals,	ps_globals)
	/* Commented out until future discussion */
	/* PHP_INI_ENTRY("session.encode_sources", "globals,track", PHP_INI_ALL, NULL) */
PHP_INI_END()

PS_SERIALIZER_FUNCS(php);
PS_SERIALIZER_FUNCS(php_binary);
#ifdef WDDX_SERIALIZER
PS_SERIALIZER_FUNCS(wddx);
#endif


const static ps_serializer ps_serializers[] = {
#ifdef WDDX_SERIALIZER
	PS_SERIALIZER_ENTRY(wddx),
#endif
	PS_SERIALIZER_ENTRY(php),
	PS_SERIALIZER_ENTRY(php_binary),
	{0}
};

PHP_MINIT_FUNCTION(session);
PHP_RINIT_FUNCTION(session);
PHP_MSHUTDOWN_FUNCTION(session);
PHP_RSHUTDOWN_FUNCTION(session);
PHP_MINFO_FUNCTION(session);

static void php_rinit_session_globals(PSLS_D);
static void php_rshutdown_session_globals(PSLS_D);
static zend_bool _php_session_destroy(PSLS_D);

zend_module_entry session_module_entry = {
	"session",
	session_functions,
	PHP_MINIT(session), PHP_MSHUTDOWN(session),
	PHP_RINIT(session), PHP_RSHUTDOWN(session),
	PHP_MINFO(session),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SESSION
ZEND_GET_MODULE(session)
#endif

typedef struct {
	char *name;
	void (*func)(PSLS_D);
} php_session_cache_limiter;

#define CACHE_LIMITER_FUNC(name) static void _php_cache_limiter_##name(PSLS_D)
#define CACHE_LIMITER(name) { #name, _php_cache_limiter_##name },

#define ADD_COOKIE(a) sapi_add_header(a, strlen(a), 1);

#define STR_CAT(P,S,I) {\
	pval *__p = (P);\
	size_t __l = (I);\
		ulong __i = __p->value.str.len;\
		__p->value.str.len += __l;\
		if (__p->value.str.val) {\
			__p->value.str.val = (char *)erealloc(__p->value.str.val, __p->value.str.len + 1);\
		} else {\
			__p->value.str.val = emalloc(__p->value.str.len + 1);\
				*__p->value.str.val = 0;\
		}\
	memcpy(__p->value.str.val + __i, (S), __l); \
	__p->value.str.val[__p->value.str.len] = '\0'; \
}

#define MAX_STR 512

#define PS_ADD_VARL(name,namelen) \
	zend_hash_update(&PS(vars), name, namelen + 1, 0, 0, NULL)

#define PS_ADD_VAR(name) PS_ADD_VARL(name, strlen(name))

#define PS_DEL_VARL(name,namelen) \
	zend_hash_del(&PS(vars), name, namelen + 1);

#define PS_DEL_VAR(name) PS_DEL_VARL(name, strlen(name))

#define ENCODE_VARS 											\
	char *key;													\
	ulong num_key;												\
	zval **struc;												\
	ELS_FETCH();												\
	PLS_FETCH()

#define ENCODE_LOOP(code)										\
	for (zend_hash_internal_pointer_reset(&PS(vars));			\
			zend_hash_get_current_key(&PS(vars), &key, &num_key) == HASH_KEY_IS_STRING; \
			zend_hash_move_forward(&PS(vars))) {				\
		if (php_get_session_var(key, strlen(key), &struc PLS_CC PSLS_CC ELS_CC) == SUCCESS) { \
			code;		 										\
		} 														\
		efree(key);												\
	}

static void php_set_session_var(char *name, size_t namelen,
								zval *state_val PSLS_DC)
{
	zval *state_val_copy;
	PLS_FETCH();
	ELS_FETCH();

	ALLOC_ZVAL(state_val_copy);
	*state_val_copy = *state_val;
	zval_copy_ctor(state_val_copy);
	state_val_copy->refcount = 0;

	if (PG(register_globals)) {
		zend_set_hash_symbol(state_val_copy, name, namelen, 0, 2, PS(http_session_vars)->value.ht, &EG(symbol_table));
	} else {
		if (PG(register_globals)) {
			zend_set_hash_symbol(state_val_copy, name, namelen, 0, 1, &EG(symbol_table));
		}

		zend_set_hash_symbol(state_val_copy, name, namelen, 0, 1, PS(http_session_vars)->value.ht);
	}
}

static int php_get_session_var(char *name, size_t namelen, zval ***state_var PLS_DC PSLS_DC ELS_DC)
{
	HashTable *ht = &EG(symbol_table);

	if (!PG(register_globals))
		ht = PS(http_session_vars)->value.ht;

	return zend_hash_find(ht, name, namelen + 1, (void **)state_var);
}

#define PS_BIN_NR_OF_BITS 8
#define PS_BIN_UNDEF (1<<(PS_BIN_NR_OF_BITS-1))
#define PS_BIN_MAX (PS_BIN_UNDEF-1)

PS_SERIALIZER_ENCODE_FUNC(php_binary)
{
	zval *buf;
	char strbuf[MAX_STR + 1];
	ENCODE_VARS;

	buf = ecalloc(sizeof(*buf), 1);
	buf->type = IS_STRING;
	buf->refcount++;

	ENCODE_LOOP(
			size_t slen = strlen(key);

			if (slen > PS_BIN_MAX) continue;
			strbuf[0] = slen;
			memcpy(strbuf + 1, key, slen);
			
			STR_CAT(buf, strbuf, slen + 1);
			php_var_serialize(buf, struc);
		} else {
			size_t slen = strlen(key);

			if (slen > PS_BIN_MAX) continue;
			strbuf[0] = slen & PS_BIN_UNDEF;
			memcpy(strbuf + 1, key, slen);
			
			STR_CAT(buf, strbuf, slen + 1);
	);

	if (newlen) *newlen = buf->value.str.len;
	*newstr = buf->value.str.val;
	efree(buf);

	return SUCCESS;
}

PS_SERIALIZER_DECODE_FUNC(php_binary)
{
	const char *p;
	char *name;
	const char *endptr = val + vallen;
	zval *current;
	int namelen;
	int has_value;

	current = (zval *) ecalloc(sizeof(zval), 1);
	for (p = val; p < endptr; ) {
		namelen = *p & (~PS_BIN_UNDEF);
		has_value = *p & PS_BIN_UNDEF ? 0 : 1;

		name = estrndup(p + 1, namelen);
		
		p += namelen + 1;
		
		if (has_value) {
			if (php_var_unserialize(&current, &p, endptr)) {
				php_set_session_var(name, namelen, current PSLS_CC);
				zval_dtor(current);
			}
		}
		PS_ADD_VARL(name, namelen);
		efree(name);
	}
	efree(current);

	return SUCCESS;
}

#define PS_DELIMITER '|'
#define PS_UNDEF_MARKER '!'

PS_SERIALIZER_ENCODE_FUNC(php)
{
	zval *buf;
	char strbuf[MAX_STR + 1];
	ENCODE_VARS;

	buf = ecalloc(sizeof(*buf), 1);
	buf->type = IS_STRING;
	buf->refcount++;

	ENCODE_LOOP(
			size_t slen = strlen(key);

			if (slen + 1 > MAX_STR) continue;
			memcpy(strbuf, key, slen);
			strbuf[slen] = PS_DELIMITER;
			STR_CAT(buf, strbuf, slen + 1);
			
			php_var_serialize(buf, struc);
		} else {
			size_t slen = strlen(key);

			if (slen + 2 > MAX_STR) continue;
			strbuf[0] = PS_UNDEF_MARKER;
			memcpy(strbuf + 1, key, slen);
			strbuf[slen + 1] = PS_DELIMITER;
			
			STR_CAT(buf, strbuf, slen + 2);
	);

	if (newlen) *newlen = buf->value.str.len;
	*newstr = buf->value.str.val;
	efree(buf);

	return SUCCESS;
}

PS_SERIALIZER_DECODE_FUNC(php)	
{
	const char *p, *q;
	char *name;
	const char *endptr = val + vallen;
	zval *current;
	int namelen;
	int has_value;

	current = (zval *) ecalloc(sizeof(zval), 1);
	for (p = q = val; (p < endptr) && (q = memchr(p, PS_DELIMITER, endptr - p)); p = q) {
		if (p[0] == PS_UNDEF_MARKER) {
			p++;
			has_value = 0;
		} else {
			has_value = 1;
		}
		
		namelen = q - p;
		name = estrndup(p, namelen);
		q++;
		
		if (has_value) {
			if (php_var_unserialize(&current, &q, endptr)) {
				php_set_session_var(name, namelen, current PSLS_CC);
				zval_dtor(current);
			}
		}
		PS_ADD_VARL(name, namelen);
		efree(name);
	}
	efree(current);

	return SUCCESS;
}

#ifdef WDDX_SERIALIZER

PS_SERIALIZER_ENCODE_FUNC(wddx)
{
	wddx_packet *packet;
	ENCODE_VARS;

	packet = php_wddx_constructor();
	if (!packet)
		return FAILURE;

	php_wddx_packet_start(packet, NULL, 0);
	php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);
	
	ENCODE_LOOP(
		php_wddx_serialize_var(packet, *struc, key);
	);
	
	php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
	php_wddx_packet_end(packet);
	*newstr = php_wddx_gather(packet);
	php_wddx_destructor(packet);
	
	if (newlen)
		*newlen = strlen(*newstr);

	return SUCCESS;
}

PS_SERIALIZER_DECODE_FUNC(wddx)
{
	zval *retval;
	zval **ent;
	char *key;
	char tmp[128];
	ulong idx;
	int hash_type;
	int dofree = 1;
	int ret;

	if (vallen == 0) 
		return SUCCESS;
	
	MAKE_STD_ZVAL(retval);

	if ((ret = php_wddx_deserialize_ex((char *)val, vallen, retval)) == SUCCESS) {

		for (zend_hash_internal_pointer_reset(retval->value.ht);
			 zend_hash_get_current_data(retval->value.ht, (void **) &ent) == SUCCESS;
			 zend_hash_move_forward(retval->value.ht)) {
			hash_type = zend_hash_get_current_key(retval->value.ht, &key, &idx);

			switch (hash_type) {
				case HASH_KEY_IS_LONG:
					sprintf(tmp, "%ld", idx);
					key = tmp;
					dofree = 0;
					/* fallthru */
				case HASH_KEY_IS_STRING:
					php_set_session_var(key, strlen(key), *ent PSLS_CC);
					PS_ADD_VAR(key);
					if (dofree) efree(key);
					dofree = 1;
			}
		}
	}

	zval_dtor(retval);
	efree(retval);

	return ret;
}

#endif

static void php_session_track_init(void)
{
	zval **old_vars = NULL;
	PSLS_FETCH();
	ELS_FETCH();

	if (zend_hash_find(&EG(symbol_table), "HTTP_SESSION_VARS", sizeof("HTTP_SESSION_VARS"), (void **)&old_vars) == SUCCESS && (*old_vars)->type == IS_ARRAY) {
	  PS(http_session_vars) = *old_vars;
	  zend_hash_clean(PS(http_session_vars)->value.ht);
	} else {
	  if(old_vars) {
		zend_hash_del(&EG(symbol_table), "HTTP_SESSION_VARS", sizeof("HTTP_SESSION_VARS"));
	  }
	  MAKE_STD_ZVAL(PS(http_session_vars));
	  array_init(PS(http_session_vars));
	  ZEND_SET_GLOBAL_VAR_WITH_LENGTH("HTTP_SESSION_VARS", sizeof("HTTP_SESSION_VARS"), PS(http_session_vars), 1, 0);
	}
}

static char *_php_session_encode(int *newlen PSLS_DC)
{
	char *ret = NULL;

	if (PS(serializer)->encode(&ret, newlen PSLS_CC) == FAILURE)
		ret = NULL;

	return ret;
}

static void _php_session_decode(const char *val, int vallen PSLS_DC)
{
	PLS_FETCH();

	php_session_track_init();
	if (PS(serializer)->decode(val, vallen PSLS_CC) == FAILURE) {
		_php_session_destroy(PSLS_C);
		php_error(E_WARNING, "Failed to decode session object. Session has been destroyed.");
	}
}

static char *_php_create_id(int *newlen PSLS_DC)
{
	PHP_MD5_CTX context;
	unsigned char digest[16];
	char buf[256];
	struct timeval tv;
	int i;

	gettimeofday(&tv, NULL);
	PHP_MD5Init(&context);
	
	sprintf(buf, "%ld%ld%0.8f", tv.tv_sec, tv.tv_usec, php_combined_lcg() * 10);
	PHP_MD5Update(&context, buf, strlen(buf));

	if (PS(entropy_length) > 0) {
		int fd;

		fd = V_OPEN((PS(entropy_file), O_RDONLY));
		if (fd >= 0) {
			char *p;
			int n;
			
			p = emalloc(PS(entropy_length));
			n = read(fd, p, PS(entropy_length));
			if (n > 0) {
				PHP_MD5Update(&context, p, n);
			}
			efree(p);
			close(fd);
		}
	}

	PHP_MD5Final(digest, &context);

	for (i = 0; i < 16; i++)
		sprintf(buf + (i << 1), "%02x", digest[i]);
	buf[i << 1] = '\0';
	
	if (newlen) 
		*newlen = i << 1;
	return estrdup(buf);
}

static void _php_session_initialize(PSLS_D)
{
	char *val;
	int vallen;
	
	if (PS(mod)->open(&PS(mod_data), PS(save_path), PS(session_name)) == FAILURE) {
		php_error(E_ERROR, "Failed to initialize session module");
		return;
	}
	if (PS(mod)->read(&PS(mod_data), PS(id), &val, &vallen) == SUCCESS) {
		_php_session_decode(val, vallen PSLS_CC);
		efree(val);
	}
}

static void _php_session_save_current_state(PSLS_D)
{
	char *val;
	int vallen;
	int ret;
	char *variable;
	ulong num_key;
	PLS_FETCH();
	
	if (!PG(register_globals)) {
	  if(!PS(http_session_vars)) {
		return;
	  }

	  for (zend_hash_internal_pointer_reset(PS(http_session_vars)->value.ht);
		   zend_hash_get_current_key(PS(http_session_vars)->value.ht, &variable, &num_key) == HASH_KEY_IS_STRING;
		   zend_hash_move_forward(PS(http_session_vars)->value.ht)) {
		PS_ADD_VAR(variable);
	  }
	}

	val = _php_session_encode(&vallen PSLS_CC);
	if (val) {
		ret = PS(mod)->write(&PS(mod_data), PS(id), val, vallen);
		efree(val);
	} else
		ret = PS(mod)->write(&PS(mod_data), PS(id), "", 0);
	
	if (ret == FAILURE)
		php_error(E_WARNING, "Failed to write session data (%s). Please "
				"verify that the current setting of session.save_path "
				"is correct (%s)",
				PS(mod)->name,
				PS(save_path));
	
	
	PS(mod)->close(&PS(mod_data));
}

static char *month_names[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static char *week_days[] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

static void strcat_gmt(char *ubuf, time_t *when)
{
	char buf[MAX_STR];
	struct tm tm;
	
	php_gmtime_r(when, &tm);
	
	/* we know all components, thus it is safe to use sprintf */
	sprintf(buf, "%s, %d %s %d %02d:%02d:%02d GMT", week_days[tm.tm_wday], tm.tm_mday, month_names[tm.tm_mon], tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
	strcat(ubuf, buf);
}

static void last_modified(void)
{
	char *path;
	struct stat sb;
	char buf[MAX_STR + 1];
	SLS_FETCH();

	path = SG(request_info).path_translated;
	if (path) {
		if (V_STAT(path, &sb) == -1) {
			return;
		}

		strcpy(buf, "Last-Modified: ");
		strcat_gmt(buf, &sb.st_mtime);
		ADD_COOKIE(buf);
	}
}

CACHE_LIMITER_FUNC(public)
{
	char buf[MAX_STR + 1];
	time_t now;
	
	time(&now);
	now += PS(cache_expire) * 60;
	strcpy(buf, "Expires: ");
	strcat_gmt(buf, &now);
	ADD_COOKIE(buf);
	
	sprintf(buf, "Cache-Control: public, max-age=%ld", PS(cache_expire) * 60);
	ADD_COOKIE(buf);
	
	last_modified();
}
	
CACHE_LIMITER_FUNC(private)
{
	char buf[MAX_STR + 1];
	
	ADD_COOKIE("Expires: Thu, 19 Nov 1981 08:52:00 GMT");
	sprintf(buf, "Cache-Control: private, max-age=%ld, pre-check=%ld", PS(cache_expire) * 60, PS(cache_expire) * 60);
	ADD_COOKIE(buf);

	last_modified();
}

CACHE_LIMITER_FUNC(nocache)
{
	ADD_COOKIE("Expires: Thu, 19 Nov 1981 08:52:00 GMT");
	/* For HTTP/1.1 conforming clients and the rest (MSIE 5) */
	ADD_COOKIE("Cache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0");
	/* For HTTP/1.0 conforming clients */
	ADD_COOKIE("Pragma: no-cache");
}

static php_session_cache_limiter php_session_cache_limiters[] = {
	CACHE_LIMITER(public)
	CACHE_LIMITER(private)
	CACHE_LIMITER(nocache)
	{0}
};

static int _php_session_cache_limiter(PSLS_D)
{
	php_session_cache_limiter *lim;
	SLS_FETCH();

	if (SG(headers_sent)) {
		char *output_start_filename = php_get_output_start_filename();
		int output_start_lineno = php_get_output_start_lineno();

		if (output_start_filename) {
			php_error(E_WARNING, "Cannot send session cache limiter - headers already sent (output started at %s:%d)",
				output_start_filename, output_start_lineno);
		} else {
			php_error(E_WARNING, "Cannot send session cache limiter - headers already sent");
		}	
		return (-2);
	}
	
	for (lim = php_session_cache_limiters; lim->name; lim++) {
		if (!strcasecmp(lim->name, PS(cache_limiter))) {
			lim->func(PSLS_C);
			return (0);
		}
	}

	return (-1);
}

#define COOKIE_FMT 		"Set-Cookie: %s=%s"
#define COOKIE_EXPIRES	"; expires="
#define COOKIE_PATH		"; path="
#define COOKIE_DOMAIN	"; domain="

static void _php_session_send_cookie(PSLS_D)
{
	int len;
	int pathlen;
	int domainlen;
	char *cookie;
	char *date_fmt = NULL;
	SLS_FETCH();

	if (SG(headers_sent)) {
		char *output_start_filename = php_get_output_start_filename();
		int output_start_lineno = php_get_output_start_lineno();

		if (output_start_filename) {
			php_error(E_WARNING, "Cannot send session cookie - headers already sent by (output started at %s:%d)",
				output_start_filename, output_start_lineno);
		} else {
			php_error(E_WARNING, "Cannot send session cookie - headers already sent");
		}	
		return;
	}

	len = strlen(PS(session_name)) + strlen(PS(id)) + sizeof(COOKIE_FMT);
	if (PS(cookie_lifetime) > 0) {
		date_fmt = php_std_date(time(NULL) + PS(cookie_lifetime));
		len += sizeof(COOKIE_EXPIRES) + strlen(date_fmt);
	}

	pathlen = strlen(PS(cookie_path));
	if (pathlen > 0)
		len += pathlen + sizeof(COOKIE_PATH);

	domainlen = strlen(PS(cookie_domain));
	if (domainlen > 0)
		len += domainlen + sizeof(COOKIE_DOMAIN);
	
	cookie = ecalloc(len + 1, 1);
	
	len = snprintf(cookie, len, COOKIE_FMT, PS(session_name), PS(id));
	if (PS(cookie_lifetime) > 0) {
		strcat(cookie, COOKIE_EXPIRES);
		strcat(cookie, date_fmt);
		len += strlen(COOKIE_EXPIRES) + strlen(date_fmt);
		efree(date_fmt);
	}
	
	if (pathlen > 0) {
		strcat(cookie, COOKIE_PATH);
		strcat(cookie, PS(cookie_path));
	}

	if (domainlen > 0) {
		strcat(cookie, COOKIE_DOMAIN);
		strcat(cookie, PS(cookie_domain));
	}

	sapi_add_header(cookie, strlen(cookie), 0);
}

static ps_module *_php_find_ps_module(char *name PSLS_DC)
{
	ps_module *ret = NULL;
	ps_module **mod;
	ps_module **end = ps_modules + (sizeof(ps_modules)/sizeof(ps_module*));

	for (mod = ps_modules; mod < end; mod++)
		if (*mod && !strcasecmp(name, (*mod)->name)) {
			ret = *mod;
			break;
		}
	
	return ret;
}

static const ps_serializer *_php_find_ps_serializer(char *name PSLS_DC)
{
	const ps_serializer *ret = NULL;
	const ps_serializer *mod;

	for (mod = ps_serializers; mod->name; mod++)
		if (!strcasecmp(name, mod->name)) {
			ret = mod;
			break;
		}

	return ret;
}

#define PPID2SID \
		convert_to_string((*ppid)); \
		PS(id) = estrndup((*ppid)->value.str.val, (*ppid)->value.str.len)

static void _php_session_start(PSLS_D)
{
	pval **ppid;
	pval **data;
	char *p;
	int send_cookie = 1;
	int define_sid = 1;
	int module_number = PS(module_number);
	int nrand;
	int lensess;
	ELS_FETCH();

	if (PS(nr_open_sessions) != 0) 
		return;

	lensess = strlen(PS(session_name));
	

	/*
     * Cookies are preferred, because initially
	 * cookie and get variables will be available. 
	 */

	if (!PS(id)) {
		if (zend_hash_find(&EG(symbol_table), "HTTP_COOKIE_VARS",
					sizeof("HTTP_COOKIE_VARS"), (void **) &data) == SUCCESS &&
				(*data)->type == IS_ARRAY &&
				zend_hash_find((*data)->value.ht, PS(session_name),
					lensess + 1, (void **) &ppid) == SUCCESS) {
			PPID2SID;
			define_sid = 0;
			send_cookie = 0;
		}

		if (!PS(id) &&
				zend_hash_find(&EG(symbol_table), "HTTP_GET_VARS",
					sizeof("HTTP_GET_VARS"), (void **) &data) == SUCCESS &&
				(*data)->type == IS_ARRAY &&
				zend_hash_find((*data)->value.ht, PS(session_name),
					lensess + 1, (void **) &ppid) == SUCCESS) {
			PPID2SID;
		}

		if (!PS(id) &&
				zend_hash_find(&EG(symbol_table), "HTTP_POST_VARS",
					sizeof("HTTP_POST_VARS"), (void **) &data) == SUCCESS &&
				(*data)->type == IS_ARRAY &&
				zend_hash_find((*data)->value.ht, PS(session_name),
					lensess + 1, (void **) &ppid) == SUCCESS) {
			PPID2SID;
		}
	}

	/* check the REQUEST_URI symbol for a string of the form
	   '<session-name>=<session-id>' to allow URLs of the form
       http://yoursite/<session-name>=<session-id>/script.php */

	if (!PS(id) &&
			zend_hash_find(&EG(symbol_table), "REQUEST_URI",
				sizeof("REQUEST_URI"), (void **) &data) == SUCCESS &&
			(*data)->type == IS_STRING &&
			(p = strstr((*data)->value.str.val, PS(session_name))) &&
			p[lensess] == '=') {
		char *q;

		p += lensess + 1;
		if ((q = strpbrk(p, "/?\\")))
			PS(id) = estrndup(p, q - p);
	}

	/* check whether the current request was referred to by
	   an external site which invalidates the previously found id */
	
	if (PS(id) &&
			PS(extern_referer_chk)[0] != '\0' &&
			zend_hash_find(&EG(symbol_table), "HTTP_REFERER",
				sizeof("HTTP_REFERER"), (void **) &data) == SUCCESS &&
			(*data)->type == IS_STRING &&
			(*data)->value.str.len != 0 &&
			strstr((*data)->value.str.val, PS(extern_referer_chk)) == NULL) {
		efree(PS(id));
		PS(id) = NULL;
		send_cookie = 1;
		define_sid = 1;
	}
	
	if (!PS(id))
		PS(id) = _php_create_id(NULL PSLS_CC);
	
	if (!PS(use_cookies) && send_cookie) {
		define_sid = 1;
		send_cookie = 0;
	}
	
	if (send_cookie)
		_php_session_send_cookie(PSLS_C);
	
	if (define_sid) {
		char *buf;

		buf = emalloc(strlen(PS(session_name)) + strlen(PS(id)) + 5);
		sprintf(buf, "%s=%s", PS(session_name), PS(id));
		REGISTER_STRING_CONSTANT("SID", buf, 0);
	} else
		REGISTER_STRING_CONSTANT("SID", empty_string, 0);
	PS(define_sid) = define_sid;

	PS(nr_open_sessions)++;

	_php_session_cache_limiter(PSLS_C);
	_php_session_initialize(PSLS_C);

	if (PS(mod_data) && PS(gc_probability) > 0) {
		int nrdels = -1;

		srand(time(NULL));
		nrand = (int) (100.0*rand()/RAND_MAX);
		if (nrand < PS(gc_probability)) {
			PS(mod)->gc(&PS(mod_data), PS(gc_maxlifetime), &nrdels);
			if (nrdels != -1)
				php_error(E_NOTICE, "purged %d expired session objects\n", nrdels);
		}
	}
}

static zend_bool _php_session_destroy(PSLS_D)
{
	zend_bool retval = SUCCESS;

	if (PS(nr_open_sessions) == 0) {
		php_error(E_WARNING, "Trying to destroy uninitialized session");
		return FAILURE;
	}

	if (PS(mod)->destroy(&PS(mod_data), PS(id)) == FAILURE) {
		retval = FAILURE;
		php_error(E_WARNING, "Session object destruction failed");
	}
	
	php_rshutdown_session_globals(PSLS_C);
	php_rinit_session_globals(PSLS_C);

	return retval;
}


/* {{{ proto void session_set_cookie_params(int lifetime [, string path [, string domain]])
   Set session cookie parameters */
PHP_FUNCTION(session_set_cookie_params)
{
    zval **lifetime, **path, **domain;
	PSLS_FETCH();

	if (!PS(use_cookies))
		return;

    if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 3 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &lifetime, &path, &domain) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(lifetime);
	PS(cookie_lifetime) = (*lifetime)->value.lval;

	if (ZEND_NUM_ARGS() > 1) {
		convert_to_string_ex(path);
		efree(PS(cookie_path));
		PS(cookie_path) = estrndup((*path)->value.str.val,
								   (*path)->value.str.len);

		if (ZEND_NUM_ARGS() > 2) {
			convert_to_string_ex(domain);
			efree(PS(cookie_domain));
			PS(cookie_domain) = estrndup((*domain)->value.str.val,
										 (*domain)->value.str.len);
		}
	}
}
/* }}} */

/* {{{ proto array session_get_cookie_params(void)
   Return the session cookie parameters */ 
PHP_FUNCTION(session_get_cookie_params)
{
	PSLS_FETCH();

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	if (array_init(return_value) == FAILURE) {
		php_error(E_ERROR, "Cannot initialize return value from session_get_cookie_parameters");
		RETURN_FALSE;
	}

	add_assoc_long(return_value, "lifetime", PS(cookie_lifetime));
	add_assoc_string(return_value, "path", PS(cookie_path), 1);
	add_assoc_string(return_value, "domain", PS(cookie_domain), 1);
}
/* }}} */

/* {{{ proto string session_name([string newname])
   Return the current session name. If newname is given, the session name is replaced with newname */
PHP_FUNCTION(session_name)
{
	pval **p_name;
	int ac = ZEND_NUM_ARGS();
	char *old;
	PSLS_FETCH();

	old = estrdup(PS(session_name));

	if (ac < 0 || ac > 1 || zend_get_parameters_ex(ac, &p_name) == FAILURE)
		WRONG_PARAM_COUNT;

	if (ac == 1) {
		convert_to_string_ex(p_name);
		efree(PS(session_name));
		PS(session_name) = estrndup((*p_name)->value.str.val, (*p_name)->value.str.len);
	}
	
	RETVAL_STRING(old, 0);
}
/* }}} */

/* {{{ proto string session_module_name([string newname])
   Return the current module name used for accessing session data. If newname is given, the module name is replaced with newname */
PHP_FUNCTION(session_module_name)
{
	pval **p_name;
	int ac = ZEND_NUM_ARGS();
	char *old;
	PSLS_FETCH();

	old = estrdup(PS(mod)->name);

	if (ac < 0 || ac > 1 || zend_get_parameters_ex(ac, &p_name) == FAILURE)
		WRONG_PARAM_COUNT;

	if (ac == 1) {
		ps_module *tempmod;

		convert_to_string_ex(p_name);
		tempmod = _php_find_ps_module((*p_name)->value.str.val PSLS_CC);
		if (tempmod) {
			if (PS(mod_data))
				PS(mod)->close(&PS(mod_data));
			PS(mod) = tempmod;
			PS(mod_data) = NULL;
		} else {
			efree(old);
			php_error(E_ERROR, "Cannot find named PHP session module (%s)",
					(*p_name)->value.str.val);
			RETURN_FALSE;
		}
	}

	RETVAL_STRING(old, 0);
}
/* }}} */

/* {{{ proto void session_set_save_handler(string open, string close, string read, string write, string destroy, string gc)
   Sets user-level functions */
PHP_FUNCTION(session_set_save_handler)
{
	zval **args[6];
	int i;
	ps_user *mdata;
	PSLS_FETCH();

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_array_ex(6, args) == FAILURE)
		WRONG_PARAM_COUNT;
	
	if (PS(nr_open_sessions) > 0)
		RETURN_FALSE;
	
	php_alter_ini_entry("session.save_handler", sizeof("session.save_handler"), "user", sizeof("user"), PHP_INI_USER, PHP_INI_STAGE_RUNTIME);

	mdata = emalloc(sizeof(*mdata));
	
	for (i = 0; i < 6; i++) {
		convert_to_string_ex(args[i]);
		mdata->names[i] = estrdup((*args[i])->value.str.val);
	}
	
	PS(mod_data) = (void *) mdata;
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string session_save_path([string newname])
   Return the current save path passed to module_name. If newname is given, the save path is replaced with newname */
PHP_FUNCTION(session_save_path)
{
	pval **p_name;
	int ac = ZEND_NUM_ARGS();
	char *old;
	PSLS_FETCH();

	old = estrdup(PS(save_path));

	if (ac < 0 || ac > 1 || zend_get_parameters_ex(ac, &p_name) == FAILURE)
		WRONG_PARAM_COUNT;

	if (ac == 1) {
		convert_to_string_ex(p_name);
		efree(PS(save_path));
		PS(save_path) = estrndup((*p_name)->value.str.val, (*p_name)->value.str.len);
	}
	
	RETVAL_STRING(old, 0);
}
/* }}} */

/* {{{ proto string session_id([string newid])
   Return the current session id. If newid is given, the session id is replaced with newid */
PHP_FUNCTION(session_id)
{
	pval **p_name;
	int ac = ZEND_NUM_ARGS();
	char *old = empty_string;
	PSLS_FETCH();

	if (PS(id))
		old = estrdup(PS(id));

	if (ac < 0 || ac > 1 || zend_get_parameters_ex(ac, &p_name) == FAILURE)
		WRONG_PARAM_COUNT;

	if (ac == 1) {
		convert_to_string_ex(p_name);
		if (PS(id)) efree(PS(id));
		PS(id) = estrndup((*p_name)->value.str.val, (*p_name)->value.str.len);
	}
	
	RETVAL_STRING(old, 0);
}
/* }}} */

/* {{{ proto string session_cache_limiter([string new_cache_limiter])
   Return the current cache limiter. If new_cache_limited is given, the current cache_limiter is replaced with new_cache_limiter */
PHP_FUNCTION(session_cache_limiter)
{
	pval **p_cache_limiter;
	int ac = ZEND_NUM_ARGS();
	char *old;
	PSLS_FETCH();

	old = estrdup(PS(cache_limiter));

	if (ac < 0 || ac > 1 || zend_get_parameters_ex(ac, &p_cache_limiter) == FAILURE)
		WRONG_PARAM_COUNT;

	if (ac == 1) {
		convert_to_string_ex(p_cache_limiter);
		efree(PS(cache_limiter));
		PS(cache_limiter) = estrndup((*p_cache_limiter)->value.str.val, (*p_cache_limiter)->value.str.len);
	}
	
	RETVAL_STRING(old, 0);
}
/* }}} */


/* {{{ static void php_register_var(zval** entry PSLS_DC PLS_DC) */
static void php_register_var(zval** entry PSLS_DC PLS_DC)
{
	zval **value;
	
	if ((*entry)->type == IS_ARRAY) {
		zend_hash_internal_pointer_reset((*entry)->value.ht);

		while (zend_hash_get_current_data((*entry)->value.ht, (void**)&value) == SUCCESS) {
			php_register_var(value PSLS_CC PLS_CC);
			zend_hash_move_forward((*entry)->value.ht);
		}
	} else {
		convert_to_string_ex(entry);

		if (strcmp((*entry)->value.str.val, "HTTP_SESSION_VARS") != 0)
			PS_ADD_VARL((*entry)->value.str.val, (*entry)->value.str.len);
	}
}
/* }}} */


/* {{{ proto bool session_register(mixed var_names [, mixed ...])
   Adds varname(s) to the list of variables which are freezed at the session end */
PHP_FUNCTION(session_register)
{
	zval  ***args;
	int      argc = ZEND_NUM_ARGS();
	int      i;
	PSLS_FETCH();
	PLS_FETCH();

	if (argc <= 0)
		RETURN_FALSE
	else
		args = (zval ***)emalloc(argc * sizeof(zval **));
	
	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}

	if (PS(nr_open_sessions) == 0)
		_php_session_start(PSLS_C);

	for (i = 0; i < argc; i++) {
		if ((*args[i])->type == IS_ARRAY)
			SEPARATE_ZVAL(args[i]);
		php_register_var(args[i] PSLS_CC PLS_CC);
	}	
	
	efree(args);
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool session_unregister(string varname)
   Removes varname from the list of variables which are freezed at the session end */
PHP_FUNCTION(session_unregister)
{
	pval **p_name;
	int ac = ZEND_NUM_ARGS();
	PSLS_FETCH();

	if (ac != 1 || zend_get_parameters_ex(ac, &p_name) == FAILURE)
		WRONG_PARAM_COUNT;
	
	convert_to_string_ex(p_name);
	
	PS_DEL_VAR((*p_name)->value.str.val);

	RETURN_TRUE;
}
/* }}} */


/* {{{ proto bool session_is_registered(string varname)
   Checks if a variable is registered in session */
PHP_FUNCTION(session_is_registered)
{
	pval **p_name;
	pval *p_var;
	int ac = ZEND_NUM_ARGS();
	PSLS_FETCH();

	if (ac != 1 || zend_get_parameters_ex(ac, &p_name) == FAILURE)
		WRONG_PARAM_COUNT;
	
	convert_to_string_ex(p_name);
	
	if (zend_hash_find(&PS(vars), (*p_name)->value.str.val, 
				(*p_name)->value.str.len+1, (void **)&p_var) == SUCCESS)
		RETURN_TRUE
	else
		RETURN_FALSE;
}
/* }}} */


/* {{{ proto string session_encode(void)
   Serializes the current setup and returns the serialized representation */
PHP_FUNCTION(session_encode)
{
	int len;
	char *enc;
	PSLS_FETCH();

	enc = _php_session_encode(&len PSLS_CC);
	RETVAL_STRINGL(enc, len, 0);
}
/* }}} */

/* {{{ proto bool session_decode(string data)
   Deserializes data and reinitializes the variables */
PHP_FUNCTION(session_decode)
{
	pval **str;
	PSLS_FETCH();

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_string_ex(str);

	_php_session_decode((*str)->value.str.val, (*str)->value.str.len PSLS_CC);
}
/* }}} */

/* {{{ proto bool session_start(void)
   Begin session - reinitializes freezed variables, registers browsers etc */
PHP_FUNCTION(session_start)
{
	PSLS_FETCH();

	_php_session_start(PSLS_C);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool session_destroy(void)
   Destroy the current session and all data associated with it */
PHP_FUNCTION(session_destroy)
{
	PSLS_FETCH();

	if (_php_session_destroy(PSLS_C) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

#ifdef TRANS_SID
void session_adapt_uris(const char *src, uint srclen, char **new, uint *newlen)
{
	char *data;
	size_t len;
	char buf[512];
	PSLS_FETCH();

	if (PS(define_sid) && PS(nr_open_sessions) > 0) {
		snprintf(buf, sizeof(buf), "%s=%s", PS(session_name), PS(id));
		data = url_adapt(src, srclen, buf, &len);
		*new = data;
		*newlen = len;
	}
}
#endif

/* {{{ proto void session_unset(void)
   Unset all registered variables */
PHP_FUNCTION(session_unset)
{
	zval	**tmp;
	char	 *variable;
	ulong     num_key;
	PSLS_FETCH();
	
	for (zend_hash_internal_pointer_reset(&PS(vars));
			zend_hash_get_current_key(&PS(vars), &variable, &num_key) == HASH_KEY_IS_STRING;
			zend_hash_move_forward(&PS(vars))) {
		if (zend_hash_find(&EG(symbol_table), variable, strlen(variable) + 1, (void **) &tmp)
				== SUCCESS)
			zend_hash_del(&EG(symbol_table), variable, strlen(variable) + 1);
		efree(variable);
	}
}
/* }}} */

static void php_rinit_session_globals(PSLS_D)
{		
	zend_hash_init(&PS(vars), 0, NULL, NULL, 0);
	PS(define_sid) = 0;
	PS(id) = NULL;
	PS(nr_open_sessions) = 0;
	PS(mod_data) = NULL;
}

#define FREE_NULL(x) efree(x); (x) = NULL;

static void php_rshutdown_session_globals(PSLS_D)
{
	if (PS(mod_data))
		PS(mod)->close(&PS(mod_data));
	if (PS(id)) 
		efree(PS(id));
	FREE_NULL(PS(entropy_file));
	FREE_NULL(PS(extern_referer_chk));
	FREE_NULL(PS(save_path));
	FREE_NULL(PS(session_name));
	FREE_NULL(PS(cache_limiter));
	FREE_NULL(PS(cookie_path));
	FREE_NULL(PS(cookie_domain));
	zend_hash_destroy(&PS(vars));
}


PHP_RINIT_FUNCTION(session)
{
	PSLS_FETCH();

	php_rinit_session_globals(PSLS_C);

	if (PS(mod) == NULL) {
		/* current status is unusable */
		PS(nr_open_sessions) = -1;
		return SUCCESS;
	}

	if (PS(auto_start)) {
		_php_session_start(PSLS_C);
	}

	return SUCCESS;
}


PHP_RSHUTDOWN_FUNCTION(session)
{
	PSLS_FETCH();

	if (PS(nr_open_sessions) > 0) {
		_php_session_save_current_state(PSLS_C);
		PS(nr_open_sessions)--;
	}
	php_rshutdown_session_globals(PSLS_C);
	return SUCCESS;
}



PHP_MINIT_FUNCTION(session)
{
#ifdef ZTS
	php_ps_globals *ps_globals;

	ps_globals_id = ts_allocate_id(sizeof(php_ps_globals), NULL, NULL);
	ps_globals = ts_resource(ps_globals_id);
#endif

	PS(module_number) = module_number;
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(session)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}


PHP_MINFO_FUNCTION(session)
{

	php_info_print_table_start();
	php_info_print_table_row(2, "Session Support", "enabled" );
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
