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
   | Authors: Sascha Schumann <ss@2ns.de>                                 |
   |          Andrei Zmievski <andrei@ispi.net>                           |
   +----------------------------------------------------------------------+
 */

#if !(WIN32|WINNT)
#include <sys/time.h>
#else
#include "win32/time.h"
#endif

#include <fcntl.h>

#include "php.h"
#include "php_ini.h"
#include "SAPI.h"

#include "php_session.h"
#include "ext/standard/md5.h"
#include "ext/standard/php3_var.h"
#include "ext/standard/datetime.h"
#include "ext/standard/php_lcg.h"
#include "ext/standard/url_scanner.h"

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
	{0}
};

PHP_INI_BEGIN()
	PHP_INI_ENTRY("session.save_path", "/tmp", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("session.name", "PHPSESSID", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("session.save_handler", "files", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("session.auto_start", "0", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("session.gc_probability", "1", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("session.gc_maxlifetime", "1440", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("session.lifetime", "0", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("session.serialize_handler", "php", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("session.extern_referer_check", "", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("session.entropy_file", "", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("session.entropy_length", "0", PHP_INI_ALL, NULL)
PHP_INI_END()

PS_SERIALIZER_FUNCS(php);
#ifdef WDDX_SERIALIZER
PS_SERIALIZER_FUNCS(wddx);
#endif


const static ps_serializer ps_serializers[] = {
#ifdef WDDX_SERIALIZER
	PS_SERIALIZER_ENTRY(wddx),
#endif
	PS_SERIALIZER_ENTRY(php),
	{0}
};

PHP_MINIT_FUNCTION(session);
PHP_RINIT_FUNCTION(session);
PHP_MSHUTDOWN_FUNCTION(session);
PHP_RSHUTDOWN_FUNCTION(session);
PHP_MINFO_FUNCTION(session);

static void php_rinit_session_globals(PSLS_D);
static void php_rshutdown_session_globals(PSLS_D);

zend_module_entry session_module_entry = {
	"session",
	session_functions,
	PHP_MINIT(session), PHP_MSHUTDOWN(session),
	PHP_RINIT(session), PHP_RSHUTDOWN(session),
	PHP_MINFO(session),
	STANDARD_MODULE_PROPERTIES,
};

#define STR_CAT(P,S,I) {\
	pval *__p = (P);\
		ulong __i = __p->value.str.len;\
		__p->value.str.len += (I);\
		if (__p->value.str.val) {\
			__p->value.str.val = (char *)erealloc(__p->value.str.val, __p->value.str.len + 1);\
		} else {\
			__p->value.str.val = emalloc(__p->value.str.len + 1);\
				*__p->value.str.val = 0;\
		}\
	strcat(__p->value.str.val + __i, (S));\
}

#define MAX_STR 512
#define STD_FMT "%s|"
#define NOTFOUND_FMT "!%s|"

#define PS_ADD_VARL(name,namelen) \
	zend_hash_update(&PS(vars), name, namelen + 1, 0, 0, NULL)

#define PS_ADD_VAR(name) PS_ADD_VARL(name, strlen(name))

#define PS_DEL_VARL(name,namelen) \
	zend_hash_del(&PS(vars), name, namelen + 1);

#define PS_DEL_VAR(name) PS_DEL_VARL(name, strlen(name))

PS_SERIALIZER_ENCODE_FUNC(php)
{
	pval *buf;
	pval **struc;
	char *key;
	char strbuf[MAX_STR + 1];
	ELS_FETCH();

	buf = ecalloc(sizeof(*buf), 1);
	buf->type = IS_STRING;
	buf->refcount++;

	for(zend_hash_internal_pointer_reset(&PS(vars));
			zend_hash_get_current_key(&PS(vars), &key, NULL) == HASH_KEY_IS_STRING;
			zend_hash_move_forward(&PS(vars))) {
		if(zend_hash_find(&EG(symbol_table), key, strlen(key) + 1, (void **) &struc) 
				== SUCCESS) {
			snprintf(strbuf, MAX_STR, STD_FMT, key);
			STR_CAT(buf, strbuf, strlen(strbuf));
			php_var_serialize(buf, struc);
		} else {
			snprintf(strbuf, MAX_STR, NOTFOUND_FMT, key);
			STR_CAT(buf, strbuf, strlen(strbuf));
		}
		efree(key);
	}

	if(newlen) *newlen = buf->value.str.len;
	*newstr = buf->value.str.val;
	efree(buf);

	return SUCCESS;
}

PS_SERIALIZER_DECODE_FUNC(php)	
{
	const char *p, *q;
	char *name;
	const char *endptr = val + vallen;
	pval *current;
	int namelen;
	int has_value;
	ELS_FETCH();

	for(p = q = val; (p < endptr) && (q = strchr(p, '|')); p = q) {
		if(p[0] == '!') {
			p++;
			has_value = 0;
		} else {
			has_value = 1;
		}
		
		namelen = q - p;
		name = estrndup(p, namelen);
		q++;
		
		if(has_value) {
			current = (pval *) ecalloc(sizeof(pval), 1);

			if(php_var_unserialize(&current, &q, endptr)) {
				zend_hash_update(&EG(symbol_table), name, namelen + 1,
						&current, sizeof(current), NULL);
			} else {
				efree(current);
			}
		}
		PS_ADD_VAR(name);
		efree(name);
	}

	return SUCCESS;
}

#ifdef WDDX_SERIALIZER

PS_SERIALIZER_ENCODE_FUNC(wddx)
{
	wddx_packet *packet;
	char *key;
	zval **struc;
	ELS_FETCH();

	packet = _php_wddx_constructor();
	if(!packet) return FAILURE;

	_php_wddx_packet_start(packet, NULL);
	_php_wddx_add_chunk(packet, WDDX_STRUCT_S);
	
	for(zend_hash_internal_pointer_reset(&PS(vars));
			zend_hash_get_current_key(&PS(vars), &key, NULL) == HASH_KEY_IS_STRING;
			zend_hash_move_forward(&PS(vars))) {
		if(zend_hash_find(&EG(symbol_table), key, strlen(key) + 1, (void **) &struc) == SUCCESS) {
			_php_wddx_serialize_var(packet, *struc, key);
		}
		efree(key);
	}
	
	_php_wddx_add_chunk(packet, WDDX_STRUCT_E);
	_php_wddx_packet_end(packet);
	*newstr = _php_wddx_gather(packet);
	_php_wddx_destructor(packet);
	
	if(newlen) *newlen = strlen(*newstr);

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
	ELS_FETCH();

	if(vallen == 0) return FAILURE;
	
	MAKE_STD_ZVAL(retval);

	_php_wddx_deserialize_ex(val, vallen, retval);

	for(zend_hash_internal_pointer_reset(retval->value.ht);
			zend_hash_get_current_data(retval->value.ht, (void **) &ent) == SUCCESS;
			zend_hash_move_forward(retval->value.ht)) {
		hash_type = zend_hash_get_current_key(retval->value.ht, &key, &idx);

		switch(hash_type) {
			case HASH_KEY_IS_LONG:
				sprintf(tmp, "%ld", idx);
				key = tmp;
				dofree = 0;
			case HASH_KEY_IS_STRING:
				zval_add_ref(ent);
				zend_hash_update(&EG(symbol_table), key, strlen(key) + 1,
						ent, sizeof(ent), NULL);
				PS_ADD_VAR(key);
				if(dofree) efree(key);
				dofree = 1;
		}
	}

	zval_dtor(retval);
	efree(retval);

	return SUCCESS;
}

#endif

static char *_php_session_encode(int *newlen PSLS_DC)
{
	char *ret = NULL;

	if(PS(serializer)->encode(&ret, newlen PSLS_CC) == FAILURE) {
		ret = NULL;
	}

	return ret;
}

static void _php_session_decode(const char *val, int vallen PSLS_DC)
{
	PS(serializer)->decode(val, vallen PSLS_CC);
}

static char *_php_create_id(int *newlen PSLS_DC)
{
	PHP3_MD5_CTX context;
	unsigned char digest[16];
	char buf[256];
	struct timeval tv;
	int i;

	gettimeofday(&tv, NULL);
	PHP3_MD5Init(&context);
	
	sprintf(buf, "%ld%ld%0.8f", tv.tv_sec, tv.tv_usec, php_combined_lcg() * 10);
	PHP3_MD5Update(&context, buf, strlen(buf));

	if(PS(entropy_length) > 0) {
		int fd;

		fd = open(PS(entropy_file), O_RDONLY);
		if(fd >= 0) {
			char *p;
			int n;
			
			p = emalloc(PS(entropy_length));
			n = read(fd, p, PS(entropy_length));
			if(n > 0) {
				PHP3_MD5Update(&context, p, n);
			}
			efree(p);
			close(fd);
		}
	}

	PHP3_MD5Final(digest, &context);

	for(i = 0; i < 16; i++)
		sprintf(buf + (i << 1), "%02x", digest[i]);
	buf[i << 1] = '\0';
	
	if(newlen) *newlen = i << 1;
	return estrdup(buf);
}

static void _php_session_initialize(PSLS_D)
{
	char *val;
	int vallen;
	
	if(PS(mod)->open(&PS(mod_data), PS(save_path), PS(session_name)) == FAILURE) {
		php_error(E_ERROR, "failed to initialize session module");
		return;
	}
	if(PS(mod)->read(&PS(mod_data), PS(id), &val, &vallen) == SUCCESS) {
		_php_session_decode(val, vallen PSLS_CC);
		efree(val);
	}
}

static void _php_session_save_current_state(PSLS_D)
{
	char *val;
	int vallen;
	
	val = _php_session_encode(&vallen PSLS_CC);
	if(val) {
		PS(mod)->write(&PS(mod_data), PS(id), val, vallen);
		efree(val);
	} else {
		PS(mod)->write(&PS(mod_data), PS(id), "", 0);
	}
	PS(mod)->close(&PS(mod_data));
}

#define COOKIE_FMT 		"Set-cookie: %s=%s"
#define COOKIE_EXPIRES	"; expires="

static void _php_session_send_cookie(PSLS_D)
{
	int   len;
	char *cookie;
	char *date_fmt = NULL;

	len = strlen(PS(session_name)) + strlen(PS(id)) + sizeof(COOKIE_FMT);
	if (PS(lifetime) > 0) {
		date_fmt = php3_std_date(time(NULL) + PS(lifetime));
		len += sizeof(COOKIE_EXPIRES) + strlen(date_fmt);
	}
	cookie = ecalloc(len + 1, 1);
	
	len = snprintf(cookie, len, COOKIE_FMT, PS(session_name), PS(id));
	if (PS(lifetime) > 0) {
		strcat(cookie, COOKIE_EXPIRES);
		strcat(cookie, date_fmt);
		len += strlen(COOKIE_EXPIRES) + strlen(date_fmt);
		efree(date_fmt);
	}

	sapi_add_header(cookie, len);
}

static ps_module *_php_find_ps_module(char *name PSLS_DC)
{
	ps_module *ret = NULL;
	ps_module **mod;
	ps_module **end = ps_modules + (sizeof(ps_modules)/sizeof(ps_module*));

	for(mod = ps_modules; mod < end; mod++) {
		if(*mod && !strcasecmp(name, (*mod)->name)) {
			ret = *mod;
			break;
		}
	}
	
	return ret;
}

static const ps_serializer *_php_find_ps_serializer(char *name PSLS_DC)
{
	const ps_serializer *ret = NULL;
	const ps_serializer *mod;

	for(mod = ps_serializers; mod->name; mod++) {
		if(!strcasecmp(name, mod->name)) {
			ret = mod;
			break;
		}
	}

	return ret;
}

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

	if (PS(nr_open_sessions) != 0) return;

	lensess = strlen(PS(session_name));
	
	/* check whether a symbol with the name of the session exists
	   in the global symbol table */
	
	if(!PS(id) &&
			zend_hash_find(&EG(symbol_table), PS(session_name),
				lensess + 1, (void **) &ppid) == SUCCESS) {
		convert_to_string((*ppid));
		PS(id) = estrndup((*ppid)->value.str.val, (*ppid)->value.str.len);
		send_cookie = 0;
	}

	/* if the previous section was successful, we check whether
	   a symbol with the name of the session exists in the global
	   HTTP_COOKIE_VARS array */

	if(!send_cookie &&
			zend_hash_find(&EG(symbol_table), "HTTP_COOKIE_VARS",
				sizeof("HTTP_COOKIE_VARS"), (void **) &data) == SUCCESS &&
			(*data)->type == IS_ARRAY &&
			zend_hash_find((*data)->value.ht, PS(session_name),
				lensess + 1, (void **) &ppid) == SUCCESS) {
		define_sid = 0;
	}

	/* check the REQUEST_URI symbol for a string of the form
	   '<session-name>=<session-id>' to allow URLs of the form
       http://yoursite/<session-name>=<session-id>/script.php */

	if(!PS(id) &&
			zend_hash_find(&EG(symbol_table), "REQUEST_URI", 
				sizeof("REQUEST_URI"), (void **) &data) == SUCCESS &&
			(*data)->type == IS_STRING &&
			(p = strstr((*data)->value.str.val, PS(session_name))) &&
			p[lensess] == '=') {
		char *q;

		p += lensess + 1;
		if((q = strpbrk(p, "/?\\")))
			PS(id) = estrndup(p, q - p);
	}

	/* check whether the current request was referred to by
	   an external site which invalidates the previously found id */
	
	if(PS(id) && 
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
	
	if(!PS(id)) {
		PS(id) = _php_create_id(NULL PSLS_CC);
	}

	if(send_cookie) {
		_php_session_send_cookie(PSLS_C);
	}
	
	if(define_sid) {
		char *buf;

		buf = emalloc(strlen(PS(session_name)) + strlen(PS(id)) + 5);
		sprintf(buf, "%s=%s", PS(session_name), PS(id));
		REGISTER_STRING_CONSTANT("SID", buf, 0);
	} else {
		REGISTER_STRING_CONSTANT("SID", empty_string, 0);
	}
	PS(define_sid) = define_sid;

	PS(nr_open_sessions)++;

	_php_session_initialize(PSLS_C);

	if(PS(mod_data) && PS(gc_probability) > 0) {
		srand(time(NULL));
		nrand = (int) (100.0*rand()/RAND_MAX);
		if(nrand < PS(gc_probability)) 
			PS(mod)->gc(&PS(mod_data), PS(gc_maxlifetime));
	}
}

static void _php_session_destroy(PSLS_D)
{
	if(PS(nr_open_sessions) == 0)
	{
		php_error(E_WARNING, "Trying to destroy uninitialized session");
		return;
	}

	PS(mod)->destroy(&PS(mod_data), PS(id));
	php_rshutdown_session_globals(PSLS_C);
	php_rinit_session_globals(PSLS_C);
}

/* {{{ proto string session_name([string newname])
   return the current session name. if newname is given, the session name is replaced with newname */
PHP_FUNCTION(session_name)
{
	pval **p_name;
	int ac = ARG_COUNT(ht);
	char *old;
	PSLS_FETCH();

	old = estrdup(PS(session_name));

	if(ac < 0 || ac > 1 || getParametersEx(ac, &p_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if(ac == 1) {
		convert_to_string_ex(p_name);
		efree(PS(session_name));
		PS(session_name) = estrndup((*p_name)->value.str.val, (*p_name)->value.str.len);
	}
	
	RETVAL_STRING(old, 0);
}
/* }}} */

/* {{{ proto string session_module_name([string newname])
   return the current module name used for accessing session data. if newname is given, the module name is replaced with newname */
PHP_FUNCTION(session_module_name)
{
	pval **p_name;
	int ac = ARG_COUNT(ht);
	char *old;
	PSLS_FETCH();

	old = estrdup(PS(mod)->name);

	if(ac < 0 || ac > 1 || getParametersEx(ac, &p_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if(ac == 1) {
		ps_module *tempmod;

		convert_to_string_ex(p_name);
		tempmod = _php_find_ps_module((*p_name)->value.str.val PSLS_CC);
		if(tempmod) {
			if(PS(mod_data))
				PS(mod)->close(&PS(mod_data));
			PS(mod_data) = tempmod;
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
   sets user-level functions */
PHP_FUNCTION(session_set_save_handler)
{
	zval **args[6];
	int i;
	ps_user *mdata;
	PSLS_FETCH();

	if(ARG_COUNT(ht) != 6 || getParametersArrayEx(6, args) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if(PS(nr_open_sessions) > 0) {
		RETURN_FALSE;
	}
	
	PS(mod) = _php_find_ps_module("user" PSLS_CC);

	mdata = emalloc(sizeof *mdata);
	
	for(i = 0; i < 6; i++) {
		convert_to_string_ex(args[i]);
		mdata->names[i] = estrdup((*args[i])->value.str.val);
	}
	
	PS(mod_data) = (void *) mdata;
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string session_save_path([string newname])
   return the current save path passed to module_name. if newname is given, the save path is replaced with newname */
PHP_FUNCTION(session_save_path)
{
	pval **p_name;
	int ac = ARG_COUNT(ht);
	char *old;
	PSLS_FETCH();

	old = estrdup(PS(save_path));

	if(ac < 0 || ac > 1 || getParametersEx(ac, &p_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if(ac == 1) {
		convert_to_string_ex(p_name);
		efree(PS(save_path));
		PS(save_path) = estrndup((*p_name)->value.str.val, (*p_name)->value.str.len);
	}
	
	RETVAL_STRING(old, 0);
}
/* }}} */

/* {{{ proto string session_id([string newid])
   return the current session id. if newid is given, the session id is replaced with newid */
PHP_FUNCTION(session_id)
{
	pval **p_name;
	int ac = ARG_COUNT(ht);
	char *old = empty_string;
	PSLS_FETCH();

	if(PS(id))
		old = estrdup(PS(id));

	if(ac < 0 || ac > 1 || getParametersEx(ac, &p_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if(ac == 1) {
		convert_to_string_ex(p_name);
		if(PS(id)) efree(PS(id));
		PS(id) = estrndup((*p_name)->value.str.val, (*p_name)->value.str.len);
	}
	
	RETVAL_STRING(old, 0);
}
/* }}} */

/* {{{ proto session_register(string varname)
   adds varname to the list of variables which are freezed at the session end */
PHP_FUNCTION(session_register)
{
	pval **p_name;
	int ac = ARG_COUNT(ht);
	PSLS_FETCH();

	if(ac != 1 || getParametersEx(ac, &p_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(p_name);
	
	if(!PS(nr_open_sessions)) _php_session_start(PSLS_C);
	PS_ADD_VAR((*p_name)->value.str.val);
}
/* }}} */

/* {{{ proto session_unregister(string varname)
   removes varname from the list of variables which are freezed at the session end */
PHP_FUNCTION(session_unregister)
{
	pval **p_name;
	int ac = ARG_COUNT(ht);
	PSLS_FETCH();

	if(ac != 1 || getParametersEx(ac, &p_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(p_name);
	
	PS_DEL_VAR((*p_name)->value.str.val);
}
/* }}} */


/* {{{ proto bool session_is_registered(string varname)
   checks if a variable is registered in session */
PHP_FUNCTION(session_is_registered)
{
	pval **p_name;
	pval *p_var;
	int ac = ARG_COUNT(ht);
	PSLS_FETCH();

	if(ac != 1 || getParametersEx(ac, &p_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(p_name);
	
	if (zend_hash_find(&PS(vars), (*p_name)->value.str.val, (*p_name)->value.str.len+1,
					   (void **)&p_var) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto string session_encode()
   serializes the current setup and returns the serialized representation */
PHP_FUNCTION(session_encode)
{
	int len;
	char *enc;
	PSLS_FETCH();

	enc = _php_session_encode(&len PSLS_CC);
	RETVAL_STRINGL(enc, len, 0);
}
/* }}} */

/* {{{ proto session_decode(string data)
   deserializes data and reinitializes the variables */
PHP_FUNCTION(session_decode)
{
	pval **str;
	PSLS_FETCH();

	if(ARG_COUNT(ht) != 1 || getParametersEx(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(str);

	_php_session_decode((*str)->value.str.val, (*str)->value.str.len PSLS_CC);
}
/* }}} */

/* {{{ proto session_start()
   Begin session - reinitializes freezed variables, registers browsers etc */
PHP_FUNCTION(session_start)
{
	PSLS_FETCH();

	_php_session_start(PSLS_C);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto session_destroy()
   Destroy the current session and all data associated with it */
PHP_FUNCTION(session_destroy)
{
	PSLS_FETCH();
		
	_php_session_destroy(PSLS_C);
}
/* }}} */

#ifdef TRANS_SID
void session_adapt_uris(const char *src, uint srclen, char **new, uint *newlen)
{
	char *data;
	size_t len;
	char buf[512];
	PSLS_FETCH();

	if(PS(define_sid) && PS(nr_open_sessions) > 0) {
		snprintf(buf, sizeof(buf), "%s=%s", PS(session_name), PS(id));
		data = url_adapt(src, srclen, buf, &len);
		*new = data;
		*newlen = len;
	}
}
#endif

/* {{{ proto session_unset()
   Unset all registered variables */
PHP_FUNCTION(session_unset)
{
	zval	**tmp;
	char	 *variable;
	ELS_FETCH();
	PSLS_FETCH();
	
	for(zend_hash_internal_pointer_reset(&PS(vars));
			zend_hash_get_current_key(&PS(vars), &variable, NULL) == HASH_KEY_IS_STRING;
			zend_hash_move_forward(&PS(vars))) {
		if(zend_hash_find(&EG(symbol_table), variable, strlen(variable) + 1, (void **) &tmp)
				== SUCCESS) {
			zend_hash_del(&EG(symbol_table), variable, strlen(variable) + 1);
		}
		efree(variable);
	}
}
/* }}} */

static void php_rinit_session_globals(PSLS_D)
{
	PS(mod) = _php_find_ps_module(INI_STR("session.save_handler") PSLS_CC);
	PS(serializer) = \
		_php_find_ps_serializer(INI_STR("session.serialize_handler") PSLS_CC);
		
	zend_hash_init(&PS(vars), 0, NULL, NULL, 0);
	PS(define_sid) = 0;
	PS(save_path) = estrdup(INI_STR("session.save_path"));
	PS(session_name) = estrdup(INI_STR("session.name"));
	PS(entropy_file) = estrdup(INI_STR("session.entropy_file"));
	PS(entropy_length) = INI_INT("session.entropy_length");
	PS(gc_probability) = INI_INT("session.gc_probability");
	PS(gc_maxlifetime) = INI_INT("session.gc_maxlifetime");
	PS(extern_referer_chk) = estrdup(INI_STR("session.extern_referer_check"));
	PS(id) = NULL;
	PS(lifetime) = INI_INT("session.lifetime");
	PS(nr_open_sessions) = 0;
	PS(mod_data) = NULL;
}

static void php_rshutdown_session_globals(PSLS_D)
{
	if(PS(mod_data))
		PS(mod)->close(&PS(mod_data));
	if(PS(entropy_file)) efree(PS(entropy_file));
	if(PS(extern_referer_chk)) efree(PS(extern_referer_chk));
	if(PS(save_path)) efree(PS(save_path));
	if(PS(session_name)) efree(PS(session_name));
	if(PS(id)) efree(PS(id));
	zend_hash_destroy(&PS(vars));
}

PHP_RINIT_FUNCTION(session)
{
	PSLS_FETCH();

	php_rinit_session_globals(PSLS_C);

	if(PS(mod) == NULL) {
		/* current status is unusable */
		PS(nr_open_sessions) = -1;
		return FAILURE;
	}

	if(INI_INT("session.auto_start")) {
		_php_session_start(PSLS_C);
	}

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(session)
{
	PSLS_FETCH();

	if(PS(nr_open_sessions) > 0) {
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
	DISPLAY_INI_ENTRIES();
}
