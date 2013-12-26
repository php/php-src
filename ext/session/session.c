/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Andrei Zmievski <andrei@php.net>                            |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#ifdef PHP_WIN32
# include "win32/winutil.h"
# include "win32/time.h"
#else
# include <sys/time.h>
#endif

#include <sys/stat.h>
#include <fcntl.h>

#include "php_ini.h"
#include "SAPI.h"
#include "rfc1867.h"
#include "php_variables.h"
#include "php_session.h"
#include "ext/standard/md5.h"
#include "ext/standard/sha1.h"
#include "ext/standard/php_var.h"
#include "ext/date/php_date.h"
#include "ext/standard/php_lcg.h"
#include "ext/standard/url_scanner_ex.h"
#include "ext/standard/php_rand.h" /* for RAND_MAX */
#include "ext/standard/info.h"
#include "ext/standard/php_smart_str.h"
#include "ext/standard/url.h"
#include "ext/standard/basic_functions.h"

#include "mod_files.h"
#include "mod_user.h"

#ifdef HAVE_LIBMM
#include "mod_mm.h"
#endif

PHPAPI ZEND_DECLARE_MODULE_GLOBALS(ps)

static int php_session_rfc1867_callback(unsigned int event, void *event_data, void **extra TSRMLS_DC);
static int (*php_session_rfc1867_orig_callback)(unsigned int event, void *event_data, void **extra TSRMLS_DC);

/* SessionHandler class */
zend_class_entry *php_session_class_entry;

/* SessionHandlerInterface */
zend_class_entry *php_session_iface_entry;

/* SessionIdInterface */
zend_class_entry *php_session_id_iface_entry;

/* ***********
   * Helpers *
   *********** */

#define IF_SESSION_VARS() \
	if (PS(http_session_vars) && PS(http_session_vars)->type == IS_ARRAY)

#define SESSION_CHECK_ACTIVE_STATE	\
	if (PS(session_status) == php_session_active) {	\
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "A session is active. You cannot change the session module's ini settings at this time");	\
		return FAILURE;	\
	}

static void php_session_send_cookie(TSRMLS_D);

/* Dispatched by RINIT and by php_session_destroy */
static inline void php_rinit_session_globals(TSRMLS_D) /* {{{ */
{
	PS(id) = NULL;
	PS(session_status) = php_session_none;
	PS(mod_data) = NULL;
	PS(mod_user_is_open) = 0;
	/* Do NOT init PS(mod_user_names) here! */
	PS(http_session_vars) = NULL;
}
/* }}} */

/* Dispatched by RSHUTDOWN and by php_session_destroy */
static inline void php_rshutdown_session_globals(TSRMLS_D) /* {{{ */
{
	if (PS(http_session_vars)) {
		zval_ptr_dtor(&PS(http_session_vars));
		PS(http_session_vars) = NULL;
	}
	/* Do NOT destroy PS(mod_user_names) here! */
	if (PS(mod_data) || PS(mod_user_implemented)) {
		zend_try {
			PS(mod)->s_close(&PS(mod_data) TSRMLS_CC);
		} zend_end_try();
	}
	if (PS(id)) {
		efree(PS(id));
	}
}
/* }}} */

static int php_session_destroy(TSRMLS_D) /* {{{ */
{
	int retval = SUCCESS;

	if (PS(session_status) != php_session_active) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Trying to destroy uninitialized session");
		return FAILURE;
	}

	if (PS(id) && PS(mod)->s_destroy(&PS(mod_data), PS(id) TSRMLS_CC) == FAILURE) {
		retval = FAILURE;
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Session object destruction failed");
	}

	php_rshutdown_session_globals(TSRMLS_C);
	php_rinit_session_globals(TSRMLS_C);

	return retval;
}
/* }}} */

PHPAPI void php_add_session_var(char *name, size_t namelen TSRMLS_DC) /* {{{ */
{
	zval **sym_track = NULL;

	IF_SESSION_VARS() {
		zend_hash_find(Z_ARRVAL_P(PS(http_session_vars)), name, namelen + 1, (void *) &sym_track);
	} else {
		return;
	}

	if (sym_track == NULL) {
		zval *empty_var;

		ALLOC_INIT_ZVAL(empty_var);
		ZEND_SET_SYMBOL_WITH_LENGTH(Z_ARRVAL_P(PS(http_session_vars)), name, namelen+1, empty_var, 1, 0);
	}
}
/* }}} */

PHPAPI void php_set_session_var(char *name, size_t namelen, zval *state_val, php_unserialize_data_t *var_hash TSRMLS_DC) /* {{{ */
{
	IF_SESSION_VARS() {
		zend_set_hash_symbol(state_val, name, namelen, PZVAL_IS_REF(state_val), 1, Z_ARRVAL_P(PS(http_session_vars)));
	}
}
/* }}} */

PHPAPI int php_get_session_var(char *name, size_t namelen, zval ***state_var TSRMLS_DC) /* {{{ */
{
	int ret = FAILURE;

	IF_SESSION_VARS() {
		ret = zend_hash_find(Z_ARRVAL_P(PS(http_session_vars)), name, namelen + 1, (void **) state_var);
	}
	return ret;
}
/* }}} */

static void php_session_track_init(TSRMLS_D) /* {{{ */
{
	zval *session_vars = NULL;

	/* Unconditionally destroy existing array -- possible dirty data */
	zend_delete_global_variable("_SESSION", sizeof("_SESSION")-1 TSRMLS_CC);

	if (PS(http_session_vars)) {
		zval_ptr_dtor(&PS(http_session_vars));
	}

	MAKE_STD_ZVAL(session_vars);
	array_init(session_vars);
	PS(http_session_vars) = session_vars;

	ZEND_SET_GLOBAL_VAR_WITH_LENGTH("_SESSION", sizeof("_SESSION"), PS(http_session_vars), 2, 1);
}
/* }}} */

static char *php_session_encode(int *newlen TSRMLS_DC) /* {{{ */
{
	char *ret = NULL;

	IF_SESSION_VARS() {
		if (!PS(serializer)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown session.serialize_handler. Failed to encode session object");
			ret = NULL;
		} else if (PS(serializer)->encode(&ret, newlen TSRMLS_CC) == FAILURE) {
			ret = NULL;
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot encode non-existent session");
	}
	return ret;
}
/* }}} */

static void php_session_decode(const char *val, int vallen TSRMLS_DC) /* {{{ */
{
	if (!PS(serializer)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown session.serialize_handler. Failed to decode session object");
		return;
	}
	if (PS(serializer)->decode(val, vallen TSRMLS_CC) == FAILURE) {
		php_session_destroy(TSRMLS_C);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to decode session object. Session has been destroyed");
	}
}
/* }}} */

/*
 * Note that we cannot use the BASE64 alphabet here, because
 * it contains "/" and "+": both are unacceptable for simple inclusion
 * into URLs.
 */

static char hexconvtab[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,-";

enum {
	PS_HASH_FUNC_MD5,
	PS_HASH_FUNC_SHA1,
	PS_HASH_FUNC_OTHER
};

/* returns a pointer to the byte after the last valid character in out */
static char *bin_to_readable(char *in, size_t inlen, char *out, char nbits) /* {{{ */
{
	unsigned char *p, *q;
	unsigned short w;
	int mask;
	int have;

	p = (unsigned char *) in;
	q = (unsigned char *)in + inlen;

	w = 0;
	have = 0;
	mask = (1 << nbits) - 1;

	while (1) {
		if (have < nbits) {
			if (p < q) {
				w |= *p++ << have;
				have += 8;
			} else {
				/* consumed everything? */
				if (have == 0) break;
				/* No? We need a final round */
				have = nbits;
			}
		}

		/* consume nbits */
		*out++ = hexconvtab[w & mask];
		w >>= nbits;
		have -= nbits;
	}

	*out = '\0';
	return out;
}
/* }}} */

PHPAPI char *php_session_create_id(PS_CREATE_SID_ARGS) /* {{{ */
{
	PHP_MD5_CTX md5_context;
	PHP_SHA1_CTX sha1_context;
#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH)
	void *hash_context = NULL;
#endif
	unsigned char *digest;
	int digest_len;
	int j;
	char *buf, *outid;
	struct timeval tv;
	zval **array;
	zval **token;
	char *remote_addr = NULL;

	gettimeofday(&tv, NULL);

	if (zend_hash_find(&EG(symbol_table), "_SERVER", sizeof("_SERVER"), (void **) &array) == SUCCESS &&
		Z_TYPE_PP(array) == IS_ARRAY &&
		zend_hash_find(Z_ARRVAL_PP(array), "REMOTE_ADDR", sizeof("REMOTE_ADDR"), (void **) &token) == SUCCESS
	) {
		remote_addr = Z_STRVAL_PP(token);
	}

	/* maximum 15+19+19+10 bytes */
	spprintf(&buf, 0, "%.15s%ld%ld%0.8F", remote_addr ? remote_addr : "", tv.tv_sec, (long int)tv.tv_usec, php_combined_lcg(TSRMLS_C) * 10);

	switch (PS(hash_func)) {
		case PS_HASH_FUNC_MD5:
			PHP_MD5Init(&md5_context);
			PHP_MD5Update(&md5_context, (unsigned char *) buf, strlen(buf));
			digest_len = 16;
			break;
		case PS_HASH_FUNC_SHA1:
			PHP_SHA1Init(&sha1_context);
			PHP_SHA1Update(&sha1_context, (unsigned char *) buf, strlen(buf));
			digest_len = 20;
			break;
#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH)
		case PS_HASH_FUNC_OTHER:
			if (!PS(hash_ops)) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid session hash function");
				efree(buf);
				return NULL;
			}

			hash_context = emalloc(PS(hash_ops)->context_size);
			PS(hash_ops)->hash_init(hash_context);
			PS(hash_ops)->hash_update(hash_context, (unsigned char *) buf, strlen(buf));
			digest_len = PS(hash_ops)->digest_size;
			break;
#endif /* HAVE_HASH_EXT */
		default:
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid session hash function");
			efree(buf);
			return NULL;
	}
	efree(buf);

	if (PS(entropy_length) > 0) {
#ifdef PHP_WIN32
		unsigned char rbuf[2048];
		size_t toread = PS(entropy_length);

		if (php_win32_get_random_bytes(rbuf, MIN(toread, sizeof(rbuf))) == SUCCESS){

			switch (PS(hash_func)) {
				case PS_HASH_FUNC_MD5:
					PHP_MD5Update(&md5_context, rbuf, toread);
					break;
				case PS_HASH_FUNC_SHA1:
					PHP_SHA1Update(&sha1_context, rbuf, toread);
					break;
# if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH)
				case PS_HASH_FUNC_OTHER:
					PS(hash_ops)->hash_update(hash_context, rbuf, toread);
					break;
# endif /* HAVE_HASH_EXT */
			}
		}
#else
		int fd;

		fd = VCWD_OPEN(PS(entropy_file), O_RDONLY);
		if (fd >= 0) {
			unsigned char rbuf[2048];
			int n;
			int to_read = PS(entropy_length);

			while (to_read > 0) {
				n = read(fd, rbuf, MIN(to_read, sizeof(rbuf)));
				if (n <= 0) break;

				switch (PS(hash_func)) {
					case PS_HASH_FUNC_MD5:
						PHP_MD5Update(&md5_context, rbuf, n);
						break;
					case PS_HASH_FUNC_SHA1:
						PHP_SHA1Update(&sha1_context, rbuf, n);
						break;
#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH)
					case PS_HASH_FUNC_OTHER:
						PS(hash_ops)->hash_update(hash_context, rbuf, n);
						break;
#endif /* HAVE_HASH_EXT */
				}
				to_read -= n;
			}
			close(fd);
		}
#endif
	}

	digest = emalloc(digest_len + 1);
	switch (PS(hash_func)) {
		case PS_HASH_FUNC_MD5:
			PHP_MD5Final(digest, &md5_context);
			break;
		case PS_HASH_FUNC_SHA1:
			PHP_SHA1Final(digest, &sha1_context);
			break;
#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH)
		case PS_HASH_FUNC_OTHER:
			PS(hash_ops)->hash_final(digest, hash_context);
			efree(hash_context);
			break;
#endif /* HAVE_HASH_EXT */
	}

	if (PS(hash_bits_per_character) < 4
			|| PS(hash_bits_per_character) > 6) {
		PS(hash_bits_per_character) = 4;

		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The ini setting hash_bits_per_character is out of range (should be 4, 5, or 6) - using 4 for now");
	}
	
	outid = emalloc((size_t)((digest_len + 2) * ((8.0f / PS(hash_bits_per_character)) + 0.5)));
	j = (int) (bin_to_readable((char *)digest, digest_len, outid, (char)PS(hash_bits_per_character)) - outid);
	efree(digest);

	if (newlen) {
		*newlen = j;
	}

	return outid;
}
/* }}} */

/* Default session id char validation function allowed by ps_modules.
 * If you change the logic here, please also update the error message in
 * ps_modules appropriately */
PHPAPI int php_session_valid_key(const char *key) /* {{{ */
{
	size_t len;
	const char *p;
	char c;
	int ret = SUCCESS;

	for (p = key; (c = *p); p++) {
		/* valid characters are a..z,A..Z,0..9 */
		if (!((c >= 'a' && c <= 'z')
				|| (c >= 'A' && c <= 'Z')
				|| (c >= '0' && c <= '9')
				|| c == ','
				|| c == '-')) {
			ret = FAILURE;
			break;
		}
	}

	len = p - key;

	/* Somewhat arbitrary length limit here, but should be way more than
	   anyone needs and avoids file-level warnings later on if we exceed MAX_PATH */
	if (len == 0 || len > 128) {
		ret = FAILURE;
	}

	return ret;
}
/* }}} */

static void php_session_initialize(TSRMLS_D) /* {{{ */
{
	char *val = NULL;
	int vallen;

	if (!PS(mod)) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "No storage module chosen - failed to initialize session");
		return;
	}

	/* Open session handler first */
	if (PS(mod)->s_open(&PS(mod_data), PS(save_path), PS(session_name) TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Failed to initialize storage module: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		return;
	}

	/* If there is no ID, use session module to create one */
	if (!PS(id)) {
		PS(id) = PS(mod)->s_create_sid(&PS(mod_data), NULL TSRMLS_CC);
		if (!PS(id)) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Failed to create session ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
			return;
		}
		if (PS(use_cookies)) {
			PS(send_cookie) = 1;
		}
	}

	php_session_reset_id(TSRMLS_C);
	PS(session_status) = php_session_active;

	/* Read data */
	php_session_track_init(TSRMLS_C);
	if (PS(mod)->s_read(&PS(mod_data), PS(id), &val, &vallen TSRMLS_CC) == FAILURE) {
		/* Some broken save handler implementation returns FAILURE for non-existent session ID */
		/* It's better to rase error for this, but disabled error for better compatibility */
		/*
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Failed to read session data: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		*/
	}
	if (val) {
		PHP_MD5_CTX context;

		/* Store read data's MD5 hash */
		PHP_MD5Init(&context);
		PHP_MD5Update(&context, val, vallen);
		PHP_MD5Final(PS(session_data_hash), &context);

		php_session_decode(val, vallen TSRMLS_CC);
		str_efree(val);
	} else {
		memset(PS(session_data_hash),'\0', 16);
	}

	if (!PS(use_cookies) && PS(send_cookie)) {
		if (PS(use_trans_sid) && !PS(use_only_cookies)) {
			PS(apply_trans_sid) = 1;
		}
		PS(send_cookie) = 0;
	}
}
/* }}} */

static void php_session_save_current_state(TSRMLS_D) /* {{{ */
{
	int ret = FAILURE;

	IF_SESSION_VARS() {
 		if (PS(mod_data) || PS(mod_user_implemented)) {
			char *val;
			int vallen;

			val = php_session_encode(&vallen TSRMLS_CC);
			if (val) {
				PHP_MD5_CTX context;
				unsigned char digest[16];

				/* Generate data's MD5 hash */
				PHP_MD5Init(&context);
				PHP_MD5Update(&context, val, vallen);
				PHP_MD5Final(digest, &context);
				/* Write only when save is required */
				if (memcmp(digest, PS(session_data_hash), 16)) {
					ret = PS(mod)->s_write(&PS(mod_data), PS(id), val, vallen TSRMLS_CC);
				} else {
					ret = SUCCESS;
				}
				efree(val);
			} else {
				ret = PS(mod)->s_write(&PS(mod_data), PS(id), "", 0 TSRMLS_CC);
			}
		}

		if (ret == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to write session data (%s). Please "
					"verify that the current setting of session.save_path "
					"is correct (%s)",
					PS(mod)->s_name,
					PS(save_path));
		}
	}

	if (PS(mod_data) || PS(mod_user_implemented)) {
		PS(mod)->s_close(&PS(mod_data) TSRMLS_CC);
	}
}
/* }}} */

/* *************************
   * INI Settings/Handlers *
   ************************* */

static PHP_INI_MH(OnUpdateSaveHandler) /* {{{ */
{
	ps_module *tmp;
	SESSION_CHECK_ACTIVE_STATE;

	tmp = _php_find_ps_module(new_value TSRMLS_CC);

	if (PG(modules_activated) && !tmp) {
		int err_type;

		if (stage == ZEND_INI_STAGE_RUNTIME) {
			err_type = E_WARNING;
		} else {
			err_type = E_ERROR;
		}

		/* Do not output error when restoring ini options. */
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			php_error_docref(NULL TSRMLS_CC, err_type, "Cannot find save handler '%s'", new_value);
		}
		return FAILURE;
	}

	PS(default_mod) = PS(mod);
	PS(mod) = tmp;

	return SUCCESS;
}
/* }}} */

static PHP_INI_MH(OnUpdateSerializer) /* {{{ */
{
	const ps_serializer *tmp;
	SESSION_CHECK_ACTIVE_STATE;

	tmp = _php_find_ps_serializer(new_value TSRMLS_CC);

	if (PG(modules_activated) && !tmp) {
		int err_type;

		if (stage == ZEND_INI_STAGE_RUNTIME) {
			err_type = E_WARNING;
		} else {
			err_type = E_ERROR;
		}

		/* Do not output error when restoring ini options. */
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			php_error_docref(NULL TSRMLS_CC, err_type, "Cannot find serialization handler '%s'", new_value);
		}
		return FAILURE;
	}
	PS(serializer) = tmp;

	return SUCCESS;
}
/* }}} */

static PHP_INI_MH(OnUpdateTransSid) /* {{{ */
{
	SESSION_CHECK_ACTIVE_STATE;

	if (!strncasecmp(new_value, "on", sizeof("on"))) {
		PS(use_trans_sid) = (zend_bool) 1;
	} else {
		PS(use_trans_sid) = (zend_bool) atoi(new_value);
	}

	return SUCCESS;
}
/* }}} */

static PHP_INI_MH(OnUpdateSaveDir) /* {{{ */
{
	/* Only do the safemode/open_basedir check at runtime */
	if (stage == PHP_INI_STAGE_RUNTIME || stage == PHP_INI_STAGE_HTACCESS) {
		char *p;

		if (memchr(new_value, '\0', new_value_length) != NULL) {
			return FAILURE;
		}

		/* we do not use zend_memrchr() since path can contain ; itself */
		if ((p = strchr(new_value, ';'))) {
			char *p2;
			p++;
			if ((p2 = strchr(p, ';'))) {
				p = p2 + 1;
			}
		} else {
			p = new_value;
		}

		if (PG(open_basedir) && *p && php_check_open_basedir(p TSRMLS_CC)) {
			return FAILURE;
		}
	}

	OnUpdateString(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

static PHP_INI_MH(OnUpdateName) /* {{{ */
{
	/* Numeric session.name won't work at all */
	if (PG(modules_activated) &&
		(!new_value_length || is_numeric_string(new_value, new_value_length, NULL, NULL, 0))) {
		int err_type;

		if (stage == ZEND_INI_STAGE_RUNTIME) {
			err_type = E_WARNING;
		} else {
			err_type = E_ERROR;
		}

		/* Do not output error when restoring ini options. */
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			php_error_docref(NULL TSRMLS_CC, err_type, "session.name cannot be a numeric or empty '%s'", new_value);
		}
		return FAILURE;
	}

	OnUpdateStringUnempty(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

static PHP_INI_MH(OnUpdateHashFunc) /* {{{ */
{
	long val;
	char *endptr = NULL;

#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH)
	PS(hash_ops) = NULL;
#endif

	val = strtol(new_value, &endptr, 10);
	if (endptr && (*endptr == '\0')) {
		/* Numeric value */
		PS(hash_func) = val ? 1 : 0;

		return SUCCESS;
	}

	if (new_value_length == (sizeof("md5") - 1) &&
		strncasecmp(new_value, "md5", sizeof("md5") - 1) == 0) {
		PS(hash_func) = PS_HASH_FUNC_MD5;

		return SUCCESS;
	}

	if (new_value_length == (sizeof("sha1") - 1) &&
		strncasecmp(new_value, "sha1", sizeof("sha1") - 1) == 0) {
		PS(hash_func) = PS_HASH_FUNC_SHA1;

		return SUCCESS;
	}

#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH) /* {{{ */
{
	php_hash_ops *ops = (php_hash_ops*)php_hash_fetch_ops(new_value, new_value_length);

	if (ops) {
		PS(hash_func) = PS_HASH_FUNC_OTHER;
		PS(hash_ops) = ops;

		return SUCCESS;
	}
}
#endif /* HAVE_HASH_EXT }}} */

	php_error_docref(NULL TSRMLS_CC, E_WARNING, "session.configuration 'session.hash_function' must be existing hash function. %s does not exist.", new_value);
	return FAILURE;
}
/* }}} */

static PHP_INI_MH(OnUpdateRfc1867Freq) /* {{{ */
{
	int tmp;
	tmp = zend_atoi(new_value, new_value_length);
	if(tmp < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "session.upload_progress.freq must be greater than or equal to zero");
		return FAILURE;
	}
	if(new_value_length > 0 && new_value[new_value_length-1] == '%') {
		if(tmp > 100) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "session.upload_progress.freq cannot be over 100%%");
			return FAILURE;
		}
		PS(rfc1867_freq) = -tmp;
	} else {
		PS(rfc1867_freq) = tmp;
	}
	return SUCCESS;
} /* }}} */

static ZEND_INI_MH(OnUpdateSmartStr) /* {{{ */
{
	smart_str *p;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base;

	base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	p = (smart_str *) (base+(size_t) mh_arg1);

	smart_str_sets(p, new_value);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("session.save_path",          "",          PHP_INI_ALL, OnUpdateSaveDir,save_path,          php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.name",               "PHPSESSID", PHP_INI_ALL, OnUpdateName, session_name,       php_ps_globals,    ps_globals)
	PHP_INI_ENTRY("session.save_handler",           "files",     PHP_INI_ALL, OnUpdateSaveHandler)
	STD_PHP_INI_BOOLEAN("session.auto_start",       "0",         PHP_INI_PERDIR, OnUpdateBool,   auto_start,         php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.gc_probability",     "1",         PHP_INI_ALL, OnUpdateLong,   gc_probability,     php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.gc_divisor",         "100",       PHP_INI_ALL, OnUpdateLong,   gc_divisor,         php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.gc_maxlifetime",     "1440",      PHP_INI_ALL, OnUpdateLong,   gc_maxlifetime,     php_ps_globals,    ps_globals)
	PHP_INI_ENTRY("session.serialize_handler",      "php",       PHP_INI_ALL, OnUpdateSerializer)
	STD_PHP_INI_ENTRY("session.cookie_lifetime",    "0",         PHP_INI_ALL, OnUpdateLong,   cookie_lifetime,    php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.cookie_path",        "/",         PHP_INI_ALL, OnUpdateString, cookie_path,        php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.cookie_domain",      "",          PHP_INI_ALL, OnUpdateString, cookie_domain,      php_ps_globals,    ps_globals)
	STD_PHP_INI_BOOLEAN("session.cookie_secure",    "",          PHP_INI_ALL, OnUpdateBool,   cookie_secure,      php_ps_globals,    ps_globals)
	STD_PHP_INI_BOOLEAN("session.cookie_httponly",  "",          PHP_INI_ALL, OnUpdateBool,   cookie_httponly,    php_ps_globals,    ps_globals)
	STD_PHP_INI_BOOLEAN("session.use_cookies",      "1",         PHP_INI_ALL, OnUpdateBool,   use_cookies,        php_ps_globals,    ps_globals)
	STD_PHP_INI_BOOLEAN("session.use_only_cookies", "1",         PHP_INI_ALL, OnUpdateBool,   use_only_cookies,   php_ps_globals,    ps_globals)
	STD_PHP_INI_BOOLEAN("session.use_strict_mode",  "0",         PHP_INI_ALL, OnUpdateBool,   use_strict_mode,    php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.referer_check",      "",          PHP_INI_ALL, OnUpdateString, extern_referer_chk, php_ps_globals,    ps_globals)
#if HAVE_DEV_URANDOM
	STD_PHP_INI_ENTRY("session.entropy_file",       "/dev/urandom",          PHP_INI_ALL, OnUpdateString, entropy_file,       php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.entropy_length",     "32",         PHP_INI_ALL, OnUpdateLong,   entropy_length,     php_ps_globals,    ps_globals)
#elif HAVE_DEV_ARANDOM
	STD_PHP_INI_ENTRY("session.entropy_file",       "/dev/arandom",          PHP_INI_ALL, OnUpdateString, entropy_file,       php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.entropy_length",     "32",         PHP_INI_ALL, OnUpdateLong,   entropy_length,     php_ps_globals,    ps_globals)
#else
	STD_PHP_INI_ENTRY("session.entropy_file",       "",          PHP_INI_ALL, OnUpdateString, entropy_file,       php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.entropy_length",     "0",         PHP_INI_ALL, OnUpdateLong,   entropy_length,     php_ps_globals,    ps_globals)
#endif
	STD_PHP_INI_ENTRY("session.cache_limiter",      "nocache",   PHP_INI_ALL, OnUpdateString, cache_limiter,      php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.cache_expire",       "180",       PHP_INI_ALL, OnUpdateLong,   cache_expire,       php_ps_globals,    ps_globals)
	PHP_INI_ENTRY("session.use_trans_sid",          "0",         PHP_INI_ALL, OnUpdateTransSid)
	PHP_INI_ENTRY("session.hash_function",          "0",         PHP_INI_ALL, OnUpdateHashFunc)
	STD_PHP_INI_ENTRY("session.hash_bits_per_character", "4",    PHP_INI_ALL, OnUpdateLong,   hash_bits_per_character, php_ps_globals, ps_globals)

	/* Upload progress */
	STD_PHP_INI_BOOLEAN("session.upload_progress.enabled",
	                                                "1",     ZEND_INI_PERDIR, OnUpdateBool,        rfc1867_enabled, php_ps_globals, ps_globals)
	STD_PHP_INI_BOOLEAN("session.upload_progress.cleanup",
	                                                "1",     ZEND_INI_PERDIR, OnUpdateBool,        rfc1867_cleanup, php_ps_globals, ps_globals)
	STD_PHP_INI_ENTRY("session.upload_progress.prefix",
	                                     "upload_progress_", ZEND_INI_PERDIR, OnUpdateSmartStr,      rfc1867_prefix,  php_ps_globals, ps_globals)
	STD_PHP_INI_ENTRY("session.upload_progress.name",
	                          "PHP_SESSION_UPLOAD_PROGRESS", ZEND_INI_PERDIR, OnUpdateSmartStr,      rfc1867_name,    php_ps_globals, ps_globals)
	STD_PHP_INI_ENTRY("session.upload_progress.freq",  "1%", ZEND_INI_PERDIR, OnUpdateRfc1867Freq, rfc1867_freq,    php_ps_globals, ps_globals)
	STD_PHP_INI_ENTRY("session.upload_progress.min_freq",
	                                                   "1",  ZEND_INI_PERDIR, OnUpdateReal,        rfc1867_min_freq,php_ps_globals, ps_globals)

	/* Commented out until future discussion */
	/* PHP_INI_ENTRY("session.encode_sources", "globals,track", PHP_INI_ALL, NULL) */
PHP_INI_END()
/* }}} */

/* ***************
   * Serializers *
   *************** */
PS_SERIALIZER_ENCODE_FUNC(php_serialize) /* {{{ */
{
	smart_str buf = {0};
	php_serialize_data_t var_hash;

	PHP_VAR_SERIALIZE_INIT(var_hash);
	php_var_serialize(&buf, &PS(http_session_vars), &var_hash TSRMLS_CC);
	PHP_VAR_SERIALIZE_DESTROY(var_hash);
	if (newlen) {
		*newlen = buf.len;
	}
	smart_str_0(&buf);
	*newstr = buf.c;
	return SUCCESS;
}
/* }}} */

PS_SERIALIZER_DECODE_FUNC(php_serialize) /* {{{ */
{
	const char *endptr = val + vallen;
	zval *session_vars;
	php_unserialize_data_t var_hash;

	PHP_VAR_UNSERIALIZE_INIT(var_hash);
	ALLOC_INIT_ZVAL(session_vars);
	php_var_unserialize(&session_vars, &val, endptr, &var_hash TSRMLS_CC);
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	if (PS(http_session_vars)) {
		zval_ptr_dtor(&PS(http_session_vars));
	}
	if (Z_TYPE_P(session_vars) == IS_NULL) {
		array_init(session_vars);
	}
	PS(http_session_vars) = session_vars;
	ZEND_SET_GLOBAL_VAR_WITH_LENGTH("_SESSION", sizeof("_SESSION"), PS(http_session_vars), 2, 1);
	return SUCCESS;
}
/* }}} */

#define PS_BIN_NR_OF_BITS 8
#define PS_BIN_UNDEF (1<<(PS_BIN_NR_OF_BITS-1))
#define PS_BIN_MAX (PS_BIN_UNDEF-1)

PS_SERIALIZER_ENCODE_FUNC(php_binary) /* {{{ */
{
	smart_str buf = {0};
	php_serialize_data_t var_hash;
	PS_ENCODE_VARS;

	PHP_VAR_SERIALIZE_INIT(var_hash);

	PS_ENCODE_LOOP(
			if (key_length > PS_BIN_MAX) continue;
			smart_str_appendc(&buf, (unsigned char) key_length);
			smart_str_appendl(&buf, key, key_length);
			php_var_serialize(&buf, struc, &var_hash TSRMLS_CC);
		} else {
			if (key_length > PS_BIN_MAX) continue;
			smart_str_appendc(&buf, (unsigned char) (key_length & PS_BIN_UNDEF));
			smart_str_appendl(&buf, key, key_length);
	);

	if (newlen) {
		*newlen = buf.len;
	}
	smart_str_0(&buf);
	*newstr = buf.c;
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	return SUCCESS;
}
/* }}} */

PS_SERIALIZER_DECODE_FUNC(php_binary) /* {{{ */
{
	const char *p;
	char *name;
	const char *endptr = val + vallen;
	zval *current;
	int namelen;
	int has_value;
	php_unserialize_data_t var_hash;

	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	for (p = val; p < endptr; ) {
		zval **tmp;
		namelen = ((unsigned char)(*p)) & (~PS_BIN_UNDEF);

		if (namelen < 0 || namelen > PS_BIN_MAX || (p + namelen) >= endptr) {
			return FAILURE;
		}

		has_value = *p & PS_BIN_UNDEF ? 0 : 1;

		name = estrndup(p + 1, namelen);

		p += namelen + 1;

		if (zend_hash_find(&EG(symbol_table), name, namelen + 1, (void **) &tmp) == SUCCESS) {
			if ((Z_TYPE_PP(tmp) == IS_ARRAY && Z_ARRVAL_PP(tmp) == &EG(symbol_table)) || *tmp == PS(http_session_vars)) {
				efree(name);
				continue;
			}
		}

		if (has_value) {
			ALLOC_INIT_ZVAL(current);
			if (php_var_unserialize(&current, (const unsigned char **) &p, (const unsigned char *) endptr, &var_hash TSRMLS_CC)) {
				php_set_session_var(name, namelen, current, &var_hash  TSRMLS_CC);
			}
			zval_ptr_dtor(&current);
		}
		PS_ADD_VARL(name, namelen);
		efree(name);
	}

	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);

	return SUCCESS;
}
/* }}} */

#define PS_DELIMITER '|'
#define PS_UNDEF_MARKER '!'

PS_SERIALIZER_ENCODE_FUNC(php) /* {{{ */
{
	smart_str buf = {0};
	php_serialize_data_t var_hash;
	PS_ENCODE_VARS;

	PHP_VAR_SERIALIZE_INIT(var_hash);

	PS_ENCODE_LOOP(
			smart_str_appendl(&buf, key, key_length);
			if (memchr(key, PS_DELIMITER, key_length) || memchr(key, PS_UNDEF_MARKER, key_length)) {
				PHP_VAR_SERIALIZE_DESTROY(var_hash);
				smart_str_free(&buf);
				return FAILURE;
			}
			smart_str_appendc(&buf, PS_DELIMITER);

			php_var_serialize(&buf, struc, &var_hash TSRMLS_CC);
		} else {
			smart_str_appendc(&buf, PS_UNDEF_MARKER);
			smart_str_appendl(&buf, key, key_length);
			smart_str_appendc(&buf, PS_DELIMITER);
	);

	if (newlen) {
		*newlen = buf.len;
	}
	smart_str_0(&buf);
	*newstr = buf.c;

	PHP_VAR_SERIALIZE_DESTROY(var_hash);
	return SUCCESS;
}
/* }}} */

PS_SERIALIZER_DECODE_FUNC(php) /* {{{ */
{
	const char *p, *q;
	char *name;
	const char *endptr = val + vallen;
	zval *current;
	int namelen;
	int has_value;
	php_unserialize_data_t var_hash;

	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	p = val;

	while (p < endptr) {
		zval **tmp;
		q = p;
		while (*q != PS_DELIMITER) {
			if (++q >= endptr) goto break_outer_loop;
		}
		if (p[0] == PS_UNDEF_MARKER) {
			p++;
			has_value = 0;
		} else {
			has_value = 1;
		}

		namelen = q - p;
		name = estrndup(p, namelen);
		q++;

		if (zend_hash_find(&EG(symbol_table), name, namelen + 1, (void **) &tmp) == SUCCESS) {
			if ((Z_TYPE_PP(tmp) == IS_ARRAY && Z_ARRVAL_PP(tmp) == &EG(symbol_table)) || *tmp == PS(http_session_vars)) {
				goto skip;
			}
		}

		if (has_value) {
			ALLOC_INIT_ZVAL(current);
			if (php_var_unserialize(&current, (const unsigned char **) &q, (const unsigned char *) endptr, &var_hash TSRMLS_CC)) {
				php_set_session_var(name, namelen, current, &var_hash  TSRMLS_CC);
			}
			zval_ptr_dtor(&current);
		}
		PS_ADD_VARL(name, namelen);
skip:
		efree(name);

		p = q;
	}
break_outer_loop:

	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);

	return SUCCESS;
}
/* }}} */

#define MAX_SERIALIZERS 32
#define PREDEFINED_SERIALIZERS 3

static ps_serializer ps_serializers[MAX_SERIALIZERS + 1] = {
	PS_SERIALIZER_ENTRY(php_serialize),
	PS_SERIALIZER_ENTRY(php),
	PS_SERIALIZER_ENTRY(php_binary)
};

PHPAPI int php_session_register_serializer(const char *name, int (*encode)(PS_SERIALIZER_ENCODE_ARGS), int (*decode)(PS_SERIALIZER_DECODE_ARGS)) /* {{{ */
{
	int ret = -1;
	int i;

	for (i = 0; i < MAX_SERIALIZERS; i++) {
		if (ps_serializers[i].name == NULL) {
			ps_serializers[i].name = name;
			ps_serializers[i].encode = encode;
			ps_serializers[i].decode = decode;
			ps_serializers[i + 1].name = NULL;
			ret = 0;
			break;
		}
	}
	return ret;
}
/* }}} */

/* *******************
   * Storage Modules *
   ******************* */

#define MAX_MODULES 10
#define PREDEFINED_MODULES 2

static ps_module *ps_modules[MAX_MODULES + 1] = {
	ps_files_ptr,
	ps_user_ptr
};

PHPAPI int php_session_register_module(ps_module *ptr) /* {{{ */
{
	int ret = -1;
	int i;

	for (i = 0; i < MAX_MODULES; i++) {
		if (!ps_modules[i]) {
			ps_modules[i] = ptr;
			ret = 0;
			break;
		}
	}
	return ret;
}
/* }}} */

/* ******************
   * Cache Limiters *
   ****************** */

typedef struct {
	char *name;
	void (*func)(TSRMLS_D);
} php_session_cache_limiter_t;

#define CACHE_LIMITER(name) _php_cache_limiter_##name
#define CACHE_LIMITER_FUNC(name) static void CACHE_LIMITER(name)(TSRMLS_D)
#define CACHE_LIMITER_ENTRY(name) { #name, CACHE_LIMITER(name) },
#define ADD_HEADER(a) sapi_add_header(a, strlen(a), 1);
#define MAX_STR 512

static char *month_names[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static char *week_days[] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

static inline void strcpy_gmt(char *ubuf, time_t *when) /* {{{ */
{
	char buf[MAX_STR];
	struct tm tm, *res;
	int n;

	res = php_gmtime_r(when, &tm);

	if (!res) {
		ubuf[0] = '\0';
		return;
	}

	n = slprintf(buf, sizeof(buf), "%s, %02d %s %d %02d:%02d:%02d GMT", /* SAFE */
				week_days[tm.tm_wday], tm.tm_mday,
				month_names[tm.tm_mon], tm.tm_year + 1900,
				tm.tm_hour, tm.tm_min,
				tm.tm_sec);
	memcpy(ubuf, buf, n);
	ubuf[n] = '\0';
}
/* }}} */

static inline void last_modified(TSRMLS_D) /* {{{ */
{
	const char *path;
	struct stat sb;
	char buf[MAX_STR + 1];

	path = SG(request_info).path_translated;
	if (path) {
		if (VCWD_STAT(path, &sb) == -1) {
			return;
		}

#define LAST_MODIFIED "Last-Modified: "
		memcpy(buf, LAST_MODIFIED, sizeof(LAST_MODIFIED) - 1);
		strcpy_gmt(buf + sizeof(LAST_MODIFIED) - 1, &sb.st_mtime);
		ADD_HEADER(buf);
	}
}
/* }}} */

#define EXPIRES "Expires: "
CACHE_LIMITER_FUNC(public) /* {{{ */
{
	char buf[MAX_STR + 1];
	struct timeval tv;
	time_t now;

	gettimeofday(&tv, NULL);
	now = tv.tv_sec + PS(cache_expire) * 60;
	memcpy(buf, EXPIRES, sizeof(EXPIRES) - 1);
	strcpy_gmt(buf + sizeof(EXPIRES) - 1, &now);
	ADD_HEADER(buf);

	snprintf(buf, sizeof(buf) , "Cache-Control: public, max-age=%ld", PS(cache_expire) * 60); /* SAFE */
	ADD_HEADER(buf);

	last_modified(TSRMLS_C);
}
/* }}} */

CACHE_LIMITER_FUNC(private_no_expire) /* {{{ */
{
	char buf[MAX_STR + 1];

	snprintf(buf, sizeof(buf), "Cache-Control: private, max-age=%ld, pre-check=%ld", PS(cache_expire) * 60, PS(cache_expire) * 60); /* SAFE */
	ADD_HEADER(buf);

	last_modified(TSRMLS_C);
}
/* }}} */

CACHE_LIMITER_FUNC(private) /* {{{ */
{
	ADD_HEADER("Expires: Thu, 19 Nov 1981 08:52:00 GMT");
	CACHE_LIMITER(private_no_expire)(TSRMLS_C);
}
/* }}} */

CACHE_LIMITER_FUNC(nocache) /* {{{ */
{
	ADD_HEADER("Expires: Thu, 19 Nov 1981 08:52:00 GMT");

	/* For HTTP/1.1 conforming clients and the rest (MSIE 5) */
	ADD_HEADER("Cache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0");

	/* For HTTP/1.0 conforming clients */
	ADD_HEADER("Pragma: no-cache");
}
/* }}} */

static php_session_cache_limiter_t php_session_cache_limiters[] = {
	CACHE_LIMITER_ENTRY(public)
	CACHE_LIMITER_ENTRY(private)
	CACHE_LIMITER_ENTRY(private_no_expire)
	CACHE_LIMITER_ENTRY(nocache)
	{0}
};

static int php_session_cache_limiter(TSRMLS_D) /* {{{ */
{
	php_session_cache_limiter_t *lim;

	if (PS(cache_limiter)[0] == '\0') return 0;

	if (SG(headers_sent)) {
		const char *output_start_filename = php_output_get_start_filename(TSRMLS_C);
		int output_start_lineno = php_output_get_start_lineno(TSRMLS_C);

		if (output_start_filename) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot send session cache limiter - headers already sent (output started at %s:%d)", output_start_filename, output_start_lineno);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot send session cache limiter - headers already sent");
		}
		return -2;
	}

	for (lim = php_session_cache_limiters; lim->name; lim++) {
		if (!strcasecmp(lim->name, PS(cache_limiter))) {
			lim->func(TSRMLS_C);
			return 0;
		}
	}

	return -1;
}
/* }}} */

/* *********************
   * Cookie Management *
   ********************* */

#define COOKIE_SET_COOKIE "Set-Cookie: "
#define COOKIE_EXPIRES	"; expires="
#define COOKIE_MAX_AGE	"; Max-Age="
#define COOKIE_PATH		"; path="
#define COOKIE_DOMAIN	"; domain="
#define COOKIE_SECURE	"; secure"
#define COOKIE_HTTPONLY	"; HttpOnly"

static void php_session_send_cookie(TSRMLS_D) /* {{{ */
{
	smart_str ncookie = {0};
	char *date_fmt = NULL;
	char *e_session_name, *e_id;

	if (SG(headers_sent)) {
		const char *output_start_filename = php_output_get_start_filename(TSRMLS_C);
		int output_start_lineno = php_output_get_start_lineno(TSRMLS_C);

		if (output_start_filename) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot send session cookie - headers already sent by (output started at %s:%d)", output_start_filename, output_start_lineno);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot send session cookie - headers already sent");
		}
		return;
	}

	/* URL encode session_name and id because they might be user supplied */
	e_session_name = php_url_encode(PS(session_name), strlen(PS(session_name)), NULL);
	e_id = php_url_encode(PS(id), strlen(PS(id)), NULL);

	smart_str_appends(&ncookie, COOKIE_SET_COOKIE);
	smart_str_appends(&ncookie, e_session_name);
	smart_str_appendc(&ncookie, '=');
	smart_str_appends(&ncookie, e_id);

	efree(e_session_name);
	efree(e_id);

	if (PS(cookie_lifetime) > 0) {
		struct timeval tv;
		time_t t;

		gettimeofday(&tv, NULL);
		t = tv.tv_sec + PS(cookie_lifetime);

		if (t > 0) {
			date_fmt = php_format_date("D, d-M-Y H:i:s T", sizeof("D, d-M-Y H:i:s T")-1, t, 0 TSRMLS_CC);
			smart_str_appends(&ncookie, COOKIE_EXPIRES);
			smart_str_appends(&ncookie, date_fmt);
			efree(date_fmt);

			smart_str_appends(&ncookie, COOKIE_MAX_AGE);
			smart_str_append_long(&ncookie, PS(cookie_lifetime));
		}
	}

	if (PS(cookie_path)[0]) {
		smart_str_appends(&ncookie, COOKIE_PATH);
		smart_str_appends(&ncookie, PS(cookie_path));
	}

	if (PS(cookie_domain)[0]) {
		smart_str_appends(&ncookie, COOKIE_DOMAIN);
		smart_str_appends(&ncookie, PS(cookie_domain));
	}

	if (PS(cookie_secure)) {
		smart_str_appends(&ncookie, COOKIE_SECURE);
	}

	if (PS(cookie_httponly)) {
		smart_str_appends(&ncookie, COOKIE_HTTPONLY);
	}

	smart_str_0(&ncookie);

	/*	'replace' must be 0 here, else a previous Set-Cookie
		header, probably sent with setcookie() will be replaced! */
	sapi_add_header_ex(ncookie.c, ncookie.len, 0, 0 TSRMLS_CC);
}
/* }}} */

PHPAPI ps_module *_php_find_ps_module(char *name TSRMLS_DC) /* {{{ */
{
	ps_module *ret = NULL;
	ps_module **mod;
	int i;

	for (i = 0, mod = ps_modules; i < MAX_MODULES; i++, mod++) {
		if (*mod && !strcasecmp(name, (*mod)->s_name)) {
			ret = *mod;
			break;
		}
	}
	return ret;
}
/* }}} */

PHPAPI const ps_serializer *_php_find_ps_serializer(char *name TSRMLS_DC) /* {{{ */
{
	const ps_serializer *ret = NULL;
	const ps_serializer *mod;

	for (mod = ps_serializers; mod->name; mod++) {
		if (!strcasecmp(name, mod->name)) {
			ret = mod;
			break;
		}
	}
	return ret;
}
/* }}} */

#define PPID2SID \
		convert_to_string((*ppid)); \
		PS(id) = estrndup(Z_STRVAL_PP(ppid), Z_STRLEN_PP(ppid))

PHPAPI void php_session_reset_id(TSRMLS_D) /* {{{ */
{
	int module_number = PS(module_number);

	if (!PS(id)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot set session ID - session ID is not initialized");
		return;
	}

	if (PS(use_cookies) && PS(send_cookie)) {
		php_session_send_cookie(TSRMLS_C);
		PS(send_cookie) = 0;
	}

	/* if the SID constant exists, destroy it. */
	zend_hash_del(EG(zend_constants), "sid", sizeof("sid"));

	if (PS(define_sid)) {
		smart_str var = {0};

		smart_str_appends(&var, PS(session_name));
		smart_str_appendc(&var, '=');
		smart_str_appends(&var, PS(id));
		smart_str_0(&var);
		REGISTER_STRINGL_CONSTANT("SID", var.c, var.len, 0);
	} else {
		REGISTER_STRINGL_CONSTANT("SID", STR_EMPTY_ALLOC(), 0, 0);
	}

	if (PS(apply_trans_sid)) {
		php_url_scanner_reset_vars(TSRMLS_C);
		php_url_scanner_add_var(PS(session_name), strlen(PS(session_name)), PS(id), strlen(PS(id)), 1 TSRMLS_CC);
	}
}
/* }}} */

PHPAPI void php_session_start(TSRMLS_D) /* {{{ */
{
	zval **ppid;
	zval **data;
	char *p, *value;
	int nrand;
	int lensess;

	if (PS(use_only_cookies)) {
		PS(apply_trans_sid) = 0;
	} else {
		PS(apply_trans_sid) = PS(use_trans_sid);
	}

	switch (PS(session_status)) {
		case php_session_active:
			php_error(E_NOTICE, "A session had already been started - ignoring session_start()");
			return;
			break;

		case php_session_disabled:
			value = zend_ini_string("session.save_handler", sizeof("session.save_handler"), 0);
			if (!PS(mod) && value) {
				PS(mod) = _php_find_ps_module(value TSRMLS_CC);
				if (!PS(mod)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot find save handler '%s' - session startup failed", value);
					return;
				}
			}
			value = zend_ini_string("session.serialize_handler", sizeof("session.serialize_handler"), 0);
			if (!PS(serializer) && value) {
				PS(serializer) = _php_find_ps_serializer(value TSRMLS_CC);
				if (!PS(serializer)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot find serialization handler '%s' - session startup failed", value);
					return;
				}
			}
			PS(session_status) = php_session_none;
			/* fallthrough */

		default:
		case php_session_none:
			PS(define_sid) = 1;
			PS(send_cookie) = 1;
	}

	lensess = strlen(PS(session_name));

	/* Cookies are preferred, because initially
	 * cookie and get variables will be available. */

	if (!PS(id)) {
		if (PS(use_cookies) && zend_hash_find(&EG(symbol_table), "_COOKIE", sizeof("_COOKIE"), (void **) &data) == SUCCESS &&
				Z_TYPE_PP(data) == IS_ARRAY &&
				zend_hash_find(Z_ARRVAL_PP(data), PS(session_name), lensess + 1, (void **) &ppid) == SUCCESS
		) {
			PPID2SID;
			PS(apply_trans_sid) = 0;
			PS(send_cookie) = 0;
			PS(define_sid) = 0;
		}

		if (!PS(use_only_cookies) && !PS(id) &&
				zend_hash_find(&EG(symbol_table), "_GET", sizeof("_GET"), (void **) &data) == SUCCESS &&
				Z_TYPE_PP(data) == IS_ARRAY &&
				zend_hash_find(Z_ARRVAL_PP(data), PS(session_name), lensess + 1, (void **) &ppid) == SUCCESS
		) {
			PPID2SID;
			PS(send_cookie) = 0;
		}

		if (!PS(use_only_cookies) && !PS(id) &&
				zend_hash_find(&EG(symbol_table), "_POST", sizeof("_POST"), (void **) &data) == SUCCESS &&
				Z_TYPE_PP(data) == IS_ARRAY &&
				zend_hash_find(Z_ARRVAL_PP(data), PS(session_name), lensess + 1, (void **) &ppid) == SUCCESS
		) {
			PPID2SID;
			PS(send_cookie) = 0;
		}
	}

	/* Check the REQUEST_URI symbol for a string of the form
	 * '<session-name>=<session-id>' to allow URLs of the form
	 * http://yoursite/<session-name>=<session-id>/script.php */

	if (!PS(use_only_cookies) && !PS(id) && PG(http_globals)[TRACK_VARS_SERVER] &&
			zend_hash_find(Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_SERVER]), "REQUEST_URI", sizeof("REQUEST_URI"), (void **) &data) == SUCCESS &&
			Z_TYPE_PP(data) == IS_STRING &&
			(p = strstr(Z_STRVAL_PP(data), PS(session_name))) &&
			p[lensess] == '='
	) {
		char *q;

		p += lensess + 1;
		if ((q = strpbrk(p, "/?\\"))) {
			PS(id) = estrndup(p, q - p);
			PS(send_cookie) = 0;
		}
	}

	/* Check whether the current request was referred to by
	 * an external site which invalidates the previously found id. */

	if (PS(id) &&
			PS(extern_referer_chk)[0] != '\0' &&
			PG(http_globals)[TRACK_VARS_SERVER] &&
			zend_hash_find(Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_SERVER]), "HTTP_REFERER", sizeof("HTTP_REFERER"), (void **) &data) == SUCCESS &&
			Z_TYPE_PP(data) == IS_STRING &&
			Z_STRLEN_PP(data) != 0 &&
			strstr(Z_STRVAL_PP(data), PS(extern_referer_chk)) == NULL
	) {
		efree(PS(id));
		PS(id) = NULL;
		PS(send_cookie) = 1;
		if (PS(use_trans_sid) && !PS(use_only_cookies)) {
			PS(apply_trans_sid) = 1;
		}
	}

	/* Finally check session id for dangarous characters
	 * Security note: session id may be embedded in HTML pages.*/
	if (PS(id) && strpbrk(PS(id), "\r\n\t <>'\"\\")) {
		efree(PS(id));
		PS(id) = NULL;
	}

	php_session_initialize(TSRMLS_C);
	php_session_cache_limiter(TSRMLS_C);

	if ((PS(mod_data) || PS(mod_user_implemented)) && PS(gc_probability) > 0) {
		int nrdels = -1;

		nrand = (int) ((float) PS(gc_divisor) * php_combined_lcg(TSRMLS_C));
		if (nrand < PS(gc_probability)) {
			PS(mod)->s_gc(&PS(mod_data), PS(gc_maxlifetime), &nrdels TSRMLS_CC);
#ifdef SESSION_DEBUG
			if (nrdels != -1) {
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "purged %d expired session objects", nrdels);
			}
#endif
		}
	}
}
/* }}} */

static void php_session_flush(TSRMLS_D) /* {{{ */
{
	if (PS(session_status) == php_session_active) {
		PS(session_status) = php_session_none;
		php_session_save_current_state(TSRMLS_C);
	}
}
/* }}} */

static void php_session_abort(TSRMLS_D) /* {{{ */
{
	if (PS(session_status) == php_session_active) {
		PS(session_status) = php_session_none;
		if (PS(mod_data) || PS(mod_user_implemented)) {
			PS(mod)->s_close(&PS(mod_data) TSRMLS_CC);
		}
	}
}
/* }}} */

static void php_session_reset(TSRMLS_D) /* {{{ */
{
	if (PS(session_status) == php_session_active) {
		php_session_initialize(TSRMLS_C);
	}
}
/* }}} */


PHPAPI void session_adapt_url(const char *url, size_t urllen, char **new, size_t *newlen TSRMLS_DC) /* {{{ */
{
	if (PS(apply_trans_sid) && (PS(session_status) == php_session_active)) {
		*new = php_url_scanner_adapt_single_url(url, urllen, PS(session_name), PS(id), newlen TSRMLS_CC);
	}
}
/* }}} */

/* ********************************
   * Userspace exported functions *
   ******************************** */

/* {{{ proto void session_set_cookie_params(int lifetime [, string path [, string domain [, bool secure[, bool httponly]]]])
   Set session cookie parameters */
static PHP_FUNCTION(session_set_cookie_params)
{
	zval **lifetime = NULL;
	char *path = NULL, *domain = NULL;
	int path_len, domain_len, argc = ZEND_NUM_ARGS();
	zend_bool secure = 0, httponly = 0;

	if (!PS(use_cookies) ||
		zend_parse_parameters(argc TSRMLS_CC, "Z|ssbb", &lifetime, &path, &path_len, &domain, &domain_len, &secure, &httponly) == FAILURE) {
		return;
	}

	convert_to_string_ex(lifetime);

	zend_alter_ini_entry("session.cookie_lifetime", sizeof("session.cookie_lifetime"), Z_STRVAL_PP(lifetime), Z_STRLEN_PP(lifetime), PHP_INI_USER, PHP_INI_STAGE_RUNTIME);

	if (path) {
		zend_alter_ini_entry("session.cookie_path", sizeof("session.cookie_path"), path, path_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}
	if (domain) {
		zend_alter_ini_entry("session.cookie_domain", sizeof("session.cookie_domain"), domain, domain_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}

	if (argc > 3) {
		zend_alter_ini_entry("session.cookie_secure", sizeof("session.cookie_secure"), secure ? "1" : "0", 1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}
	if (argc > 4) {
		zend_alter_ini_entry("session.cookie_httponly", sizeof("session.cookie_httponly"), httponly ? "1" : "0", 1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}
}
/* }}} */

/* {{{ proto array session_get_cookie_params(void)
   Return the session cookie parameters */
static PHP_FUNCTION(session_get_cookie_params)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);

	add_assoc_long(return_value, "lifetime", PS(cookie_lifetime));
	add_assoc_string(return_value, "path", PS(cookie_path), 1);
	add_assoc_string(return_value, "domain", PS(cookie_domain), 1);
	add_assoc_bool(return_value, "secure", PS(cookie_secure));
	add_assoc_bool(return_value, "httponly", PS(cookie_httponly));
}
/* }}} */

/* {{{ proto string session_name([string newname])
   Return the current session name. If newname is given, the session name is replaced with newname */
static PHP_FUNCTION(session_name)
{
	char *name = NULL;
	int name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
		return;
	}

	RETVAL_STRING(PS(session_name), 1);

	if (name) {
		zend_alter_ini_entry("session.name", sizeof("session.name"), name, name_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}
}
/* }}} */

/* {{{ proto string session_module_name([string newname])
   Return the current module name used for accessing session data. If newname is given, the module name is replaced with newname */
static PHP_FUNCTION(session_module_name)
{
	char *name = NULL;
	int name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
		return;
	}

	/* Set return_value to current module name */
	if (PS(mod) && PS(mod)->s_name) {
		RETVAL_STRING(safe_estrdup(PS(mod)->s_name), 0);
	} else {
		RETVAL_EMPTY_STRING();
	}

	if (name) {
		if (!_php_find_ps_module(name TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot find named PHP session module (%s)", name);

			zval_dtor(return_value);
			RETURN_FALSE;
		}
		if (PS(mod_data) || PS(mod_user_implemented)) {
			PS(mod)->s_close(&PS(mod_data) TSRMLS_CC);
		}
		PS(mod_data) = NULL;

		zend_alter_ini_entry("session.save_handler", sizeof("session.save_handler"), name, name_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}
}
/* }}} */

/* {{{ proto mixed session_serializer_name([string newname])
   Return the current serializer name used for encode/decode session data. If newname is given, the serialzer name is replaced with newname and return bool */
static PHP_FUNCTION(session_serializer_name)
{
	char *name = NULL;
	int name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
		return;
	}

	/* Return serializer name */
	if (!name) {
		RETURN_STRING(zend_ini_string("session.serialize_handler", sizeof("session.serialize_handler"), 0), 1);
	}

	/* Set serializer name */
	if (zend_alter_ini_entry("session.serialize_handler", sizeof("session.serialize_handler"), name, name_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void session_set_save_handler(string open, string close, string read, string write, string destroy, string gc, string create_sid)
   Sets user-level functions */
static PHP_FUNCTION(session_set_save_handler)
{
	zval ***args = NULL;
	int i, num_args, argc = ZEND_NUM_ARGS();
	char *name;

	if (PS(session_status) != php_session_none) {
		RETURN_FALSE;
	}

	if (argc > 0 && argc <= 2) {
		zval *obj = NULL, *callback = NULL;
		zend_uint func_name_len;
		char *func_name;
		HashPosition pos;
		zend_function *default_mptr, *current_mptr;
		ulong func_index;
		php_shutdown_function_entry shutdown_function_entry;
		zend_bool register_shutdown = 1;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|b", &obj, php_session_iface_entry, &register_shutdown) == FAILURE) {
			RETURN_FALSE;
		}

		/* Find implemented methods - SessionHandlerInterface */
		zend_hash_internal_pointer_reset_ex(&php_session_iface_entry->function_table, &pos);
		i = 0;
		while (zend_hash_get_current_data_ex(&php_session_iface_entry->function_table, (void **) &default_mptr, &pos) == SUCCESS) {
			zend_hash_get_current_key_ex(&php_session_iface_entry->function_table, &func_name, &func_name_len, &func_index, 0, &pos);

			if (zend_hash_find(&Z_OBJCE_P(obj)->function_table, func_name, func_name_len, (void **)&current_mptr) == SUCCESS) {
				if (PS(mod_user_names).names[i] != NULL) {
					zval_ptr_dtor(&PS(mod_user_names).names[i]);
				}

				MAKE_STD_ZVAL(callback);
				array_init_size(callback, 2);
				Z_ADDREF_P(obj);
				add_next_index_zval(callback, obj);
				add_next_index_stringl(callback, func_name, func_name_len - 1, 1);
				PS(mod_user_names).names[i] = callback;
			} else {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Session handler's function table is corrupt");
				RETURN_FALSE;
			}

			zend_hash_move_forward_ex(&php_session_iface_entry->function_table, &pos);
			++i;
		}

		/* Find implemented methods - SessionIdInterface (optional) */
		zend_hash_internal_pointer_reset_ex(&php_session_id_iface_entry->function_table, &pos);
		while (zend_hash_get_current_data_ex(&php_session_id_iface_entry->function_table, (void **) &default_mptr, &pos) == SUCCESS) {
			zend_hash_get_current_key_ex(&php_session_id_iface_entry->function_table, &func_name, &func_name_len, &func_index, 0, &pos);

			if (zend_hash_find(&Z_OBJCE_P(obj)->function_table, func_name, func_name_len, (void **)&current_mptr) == SUCCESS) {
				if (PS(mod_user_names).names[i] != NULL) {
					zval_ptr_dtor(&PS(mod_user_names).names[i]);
				}

				MAKE_STD_ZVAL(callback);
				array_init_size(callback, 2);
				Z_ADDREF_P(obj);
				add_next_index_zval(callback, obj);
				add_next_index_stringl(callback, func_name, func_name_len - 1, 1);
				PS(mod_user_names).names[i] = callback;
			}

			zend_hash_move_forward_ex(&php_session_id_iface_entry->function_table, &pos);
			++i;
		}

		if (register_shutdown) {
			/* create shutdown function */
			shutdown_function_entry.arg_count = 1;
			shutdown_function_entry.arguments = (zval **) safe_emalloc(sizeof(zval *), 1, 0);

			MAKE_STD_ZVAL(callback);
			ZVAL_STRING(callback, "session_register_shutdown", 1);
			shutdown_function_entry.arguments[0] = callback;

			/* add shutdown function, removing the old one if it exists */
			if (!register_user_shutdown_function("session_shutdown", sizeof("session_shutdown"), &shutdown_function_entry TSRMLS_CC)) {
				zval_ptr_dtor(&callback);
				efree(shutdown_function_entry.arguments);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to register session shutdown function");
				RETURN_FALSE;
			}
		} else {
			/* remove shutdown function */
			remove_user_shutdown_function("session_shutdown", sizeof("session_shutdown") TSRMLS_CC);
		}

		if (PS(mod) && PS(session_status) == php_session_none && PS(mod) != &ps_mod_user) {
			zend_alter_ini_entry("session.save_handler", sizeof("session.save_handler"), "user", sizeof("user")-1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		}

		RETURN_TRUE;
	}

	if (argc != 6 && argc != 7) {
		WRONG_PARAM_COUNT;
	}

	if (zend_parse_parameters(argc TSRMLS_CC, "+", &args, &num_args) == FAILURE) {
		return;
	}

	/* remove shutdown function */
	remove_user_shutdown_function("session_shutdown", sizeof("session_shutdown") TSRMLS_CC);

	/* at this point argc can only be 6 or 7 */
	for (i = 0; i < argc; i++) {
		if (!zend_is_callable(*args[i], 0, &name TSRMLS_CC)) {
			efree(args);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument %d is not a valid callback", i+1);
			efree(name);
			RETURN_FALSE;
		}
		efree(name);
	}
	
	if (PS(mod) && PS(mod) != &ps_mod_user) {
		zend_alter_ini_entry("session.save_handler", sizeof("session.save_handler"), "user", sizeof("user")-1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}

	for (i = 0; i < argc; i++) {
		if (PS(mod_user_names).names[i] != NULL) {
			zval_ptr_dtor(&PS(mod_user_names).names[i]);
		}
		Z_ADDREF_PP(args[i]);
		PS(mod_user_names).names[i] = *args[i];
	}

	efree(args);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string session_save_path([string newname])
   Return the current save path passed to module_name. If newname is given, the save path is replaced with newname */
static PHP_FUNCTION(session_save_path)
{
	char *name = NULL;
	int name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
		return;
	}

	RETVAL_STRING(PS(save_path), 1);

	if (name) {
		if (memchr(name, '\0', name_len) != NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The save_path cannot contain NULL characters");
			zval_dtor(return_value);
			RETURN_FALSE;
		}
		zend_alter_ini_entry("session.save_path", sizeof("session.save_path"), name, name_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}
}
/* }}} */

/* {{{ proto string session_id([string newid])
   Return the current session id. If newid is given, the session id is replaced with newid */
static PHP_FUNCTION(session_id)
{
	char *name = NULL;
	int name_len, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
		return;
	}

	if (PS(id)) {
		RETVAL_STRING(PS(id), 1);
	} else {
		RETVAL_EMPTY_STRING();
	}

	if (name) {
		if (PS(id)) {
			efree(PS(id));
		}
		PS(id) = estrndup(name, name_len);
	}
}
/* }}} */

/* {{{ proto bool session_regenerate_id([bool delete_old_session])
   Update the current session id with a newly generated one. If delete_old_session is set to true, remove the old session. */
static PHP_FUNCTION(session_regenerate_id)
{
	zend_bool del_ses = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &del_ses) == FAILURE) {
		return;
	}

	if (SG(headers_sent) && PS(use_cookies)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot regenerate session id - headers already sent");
		RETURN_FALSE;
	}

	if (PS(session_status) == php_session_active) {
		if (PS(id)) {
			if (del_ses && PS(mod)->s_destroy(&PS(mod_data), PS(id) TSRMLS_CC) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Session object destruction failed");
				RETURN_FALSE;
			}
			efree(PS(id));
			PS(id) = NULL;
		}

		PS(id) = PS(mod)->s_create_sid(&PS(mod_data), NULL TSRMLS_CC);
		if (PS(id)) {
			PS(send_cookie) = 1;
			php_session_reset_id(TSRMLS_C);
			RETURN_TRUE;
		} else {
			PS(id) = STR_EMPTY_ALLOC();
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string session_cache_limiter([string new_cache_limiter])
   Return the current cache limiter. If new_cache_limited is given, the current cache_limiter is replaced with new_cache_limiter */
static PHP_FUNCTION(session_cache_limiter)
{
	char *limiter = NULL;
	int limiter_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &limiter, &limiter_len) == FAILURE) {
		return;
	}

	RETVAL_STRING(PS(cache_limiter), 1);

	if (limiter) {
		zend_alter_ini_entry("session.cache_limiter", sizeof("session.cache_limiter"), limiter, limiter_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}
}
/* }}} */

/* {{{ proto int session_cache_expire([int new_cache_expire])
   Return the current cache expire. If new_cache_expire is given, the current cache_expire is replaced with new_cache_expire */
static PHP_FUNCTION(session_cache_expire)
{
	zval **expires = NULL;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "|Z", &expires) == FAILURE) {
		return;
	}

	RETVAL_LONG(PS(cache_expire));

	if (argc == 1) {
		convert_to_string_ex(expires);
		zend_alter_ini_entry("session.cache_expire", sizeof("session.cache_expire"), Z_STRVAL_PP(expires), Z_STRLEN_PP(expires), ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
	}
}
/* }}} */

/* {{{ proto string session_encode(void)
   Serializes the current setup and returns the serialized representation */
static PHP_FUNCTION(session_encode)
{
	int len;
	char *enc;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	enc = php_session_encode(&len TSRMLS_CC);
	if (enc == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL(enc, len, 0);
}
/* }}} */

/* {{{ proto bool session_decode(string data)
   Deserializes data and reinitializes the variables */
static PHP_FUNCTION(session_decode)
{
	char *str;
	int str_len;

	if (PS(session_status) == php_session_none) {
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}

	php_session_decode(str, str_len TSRMLS_CC);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool session_start(void)
   Begin session - reinitializes freezed variables, registers browsers etc */
static PHP_FUNCTION(session_start)
{
	/* skipping check for non-zero args for performance reasons here ?*/
	php_session_start(TSRMLS_C);

	if (PS(session_status) != php_session_active) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool session_destroy(void)
   Destroy the current session and all data associated with it */
static PHP_FUNCTION(session_destroy)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(php_session_destroy(TSRMLS_C) == SUCCESS);
}
/* }}} */

/* {{{ proto void session_unset(void)
   Unset all registered variables */
static PHP_FUNCTION(session_unset)
{
	if (PS(session_status) == php_session_none) {
		RETURN_FALSE;
	}

	IF_SESSION_VARS() {
		HashTable *ht_sess_var;

		SEPARATE_ZVAL_IF_NOT_REF(&PS(http_session_vars));
		ht_sess_var = Z_ARRVAL_P(PS(http_session_vars));

		/* Clean $_SESSION. */
		zend_hash_clean(ht_sess_var);
	}
}
/* }}} */

/* {{{ proto void session_write_close(void)
   Write session data and end session */
static PHP_FUNCTION(session_write_close)
{
	php_session_flush(TSRMLS_C);
}
/* }}} */

/* {{{ proto void session_abort(void)
   Abort session and end session. Session data will not be written */
static PHP_FUNCTION(session_abort)
{
	php_session_abort(TSRMLS_C);
}
/* }}} */

/* {{{ proto void session_reset(void)
   Reset session data from saved session data */
static PHP_FUNCTION(session_reset)
{
	php_session_reset(TSRMLS_C);
}
/* }}} */

/* {{{ proto int session_status(void)
   Returns the current session status */
static PHP_FUNCTION(session_status)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(PS(session_status));
}
/* }}} */

/* {{{ proto int session_gc([int maxlifetime])
   Execute garbage collection returns number of deleted data */
static PHP_FUNCTION(session_gc)
{
	int nrdels = -1;
	long maxlifetime = PS(gc_maxlifetime);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &maxlifetime) == FAILURE) {
		return;
	}

	/* Session must be active to have PS(mod) */
	if (PS(session_status) != php_session_active) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Trying to garbage collect without active session");
		RETURN_FALSE;
	}

	if (!PS(mod) || !PS(mod)->s_gc) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Session save handler does not have gc()");
		RETURN_FALSE;
	}
	PS(mod)->s_gc(&PS(mod_data), maxlifetime, &nrdels TSRMLS_CC);

	if (nrdels < 0) {
		/* Files save handler return -1 if there is not a permission to remove.
		   Save handlder should return negative nrdels when something wrong. */
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Session gc failed. Check permission or session storage");
		RETURN_FALSE;
	}
	RETURN_LONG((long)nrdels);
}
/* }}} */

/* {{{ proto void session_register_shutdown(void)
   Registers session_write_close() as a shutdown function */
static PHP_FUNCTION(session_register_shutdown)
{
	php_shutdown_function_entry shutdown_function_entry;
	zval *callback;

	/* This function is registered itself as a shutdown function by
	 * session_set_save_handler($obj). The reason we now register another
	 * shutdown function is in case the user registered their own shutdown
	 * function after calling session_set_save_handler(), which expects
	 * the session still to be available.
	 */

	shutdown_function_entry.arg_count = 1;
	shutdown_function_entry.arguments = (zval **) safe_emalloc(sizeof(zval *), 1, 0);

	MAKE_STD_ZVAL(callback);
	ZVAL_STRING(callback, "session_write_close", 1);
	shutdown_function_entry.arguments[0] = callback;

	if (!append_user_shutdown_function(shutdown_function_entry TSRMLS_CC)) {
		zval_ptr_dtor(&callback);
		efree(shutdown_function_entry.arguments);

		/* Unable to register shutdown function, presumably because of lack
		 * of memory, so flush the session now. It would be done in rshutdown
		 * anyway but the handler will have had it's dtor called by then.
		 * If the user does have a later shutdown function which needs the
		 * session then tough luck.
		 */
		php_session_flush(TSRMLS_C);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to register session flush function");
	}
}
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_session_name, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_module_name, 0, 0, 0)
	ZEND_ARG_INFO(0, module)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_serializer_name, 0, 0, 0)
	ZEND_ARG_INFO(0, module)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_save_path, 0, 0, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_id, 0, 0, 0)
	ZEND_ARG_INFO(0, id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_regenerate_id, 0, 0, 0)
	ZEND_ARG_INFO(0, delete_old_session)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_session_void, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_set_save_handler, 0, 0, 1)
	ZEND_ARG_INFO(0, open)
	ZEND_ARG_INFO(0, close)
	ZEND_ARG_INFO(0, read)
	ZEND_ARG_INFO(0, write)
	ZEND_ARG_INFO(0, destroy)
	ZEND_ARG_INFO(0, gc)
	ZEND_ARG_INFO(0, create_sid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_cache_limiter, 0, 0, 0)
	ZEND_ARG_INFO(0, cache_limiter)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_cache_expire, 0, 0, 0)
	ZEND_ARG_INFO(0, new_cache_expire)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_set_cookie_params, 0, 0, 1)
	ZEND_ARG_INFO(0, lifetime)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, secure)
	ZEND_ARG_INFO(0, httponly)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_session_gc, 0)
	ZEND_ARG_INFO(0, maxlifetime)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_session_class_open, 0)
	ZEND_ARG_INFO(0, save_path)
	ZEND_ARG_INFO(0, session_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_session_class_close, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_session_class_read, 0)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_session_class_write, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_session_class_destroy, 0)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_session_class_gc, 0)
	ZEND_ARG_INFO(0, maxlifetime)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_session_class_create_sid, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ session_functions[]
 */
static const zend_function_entry session_functions[] = {
	PHP_FE(session_name,              arginfo_session_name)
	PHP_FE(session_module_name,       arginfo_session_module_name)
	PHP_FE(session_serializer_name,   arginfo_session_serializer_name)
	PHP_FE(session_save_path,         arginfo_session_save_path)
	PHP_FE(session_id,                arginfo_session_id)
	PHP_FE(session_regenerate_id,     arginfo_session_regenerate_id)
	PHP_FE(session_decode,            arginfo_session_decode)
	PHP_FE(session_encode,            arginfo_session_void)
	PHP_FE(session_start,             arginfo_session_void)
	PHP_FE(session_destroy,           arginfo_session_void)
	PHP_FE(session_unset,             arginfo_session_void)
	PHP_FE(session_set_save_handler,  arginfo_session_set_save_handler)
	PHP_FE(session_cache_limiter,     arginfo_session_cache_limiter)
	PHP_FE(session_cache_expire,      arginfo_session_cache_expire)
	PHP_FE(session_set_cookie_params, arginfo_session_set_cookie_params)
	PHP_FE(session_get_cookie_params, arginfo_session_void)
	PHP_FE(session_write_close,       arginfo_session_void)
	PHP_FE(session_abort,             arginfo_session_void)
	PHP_FE(session_reset,             arginfo_session_void)
	PHP_FE(session_status,            arginfo_session_void)
	PHP_FE(session_gc,                arginfo_session_gc)
	PHP_FE(session_register_shutdown, arginfo_session_void)
	PHP_FALIAS(session_commit, session_write_close, arginfo_session_void)
	PHP_FE_END
};
/* }}} */

/* {{{ SessionHandlerInterface functions[]
*/
static const zend_function_entry php_session_iface_functions[] = {
	PHP_ABSTRACT_ME(SessionHandlerInterface, open, arginfo_session_class_open)
	PHP_ABSTRACT_ME(SessionHandlerInterface, close, arginfo_session_class_close)
	PHP_ABSTRACT_ME(SessionHandlerInterface, read, arginfo_session_class_read)
	PHP_ABSTRACT_ME(SessionHandlerInterface, write, arginfo_session_class_write)
	PHP_ABSTRACT_ME(SessionHandlerInterface, destroy, arginfo_session_class_destroy)
	PHP_ABSTRACT_ME(SessionHandlerInterface, gc, arginfo_session_class_gc)
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ SessionIdInterface functions[]
*/
static const zend_function_entry php_session_id_iface_functions[] = {
	PHP_ABSTRACT_ME(SessionIdInterface, create_sid, arginfo_session_class_create_sid)
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ SessionHandler functions[]
 */
static const zend_function_entry php_session_class_functions[] = {
	PHP_ME(SessionHandler, open, arginfo_session_class_open, ZEND_ACC_PUBLIC)
	PHP_ME(SessionHandler, close, arginfo_session_class_close, ZEND_ACC_PUBLIC)
	PHP_ME(SessionHandler, read, arginfo_session_class_read, ZEND_ACC_PUBLIC)
	PHP_ME(SessionHandler, write, arginfo_session_class_write, ZEND_ACC_PUBLIC)
	PHP_ME(SessionHandler, destroy, arginfo_session_class_destroy, ZEND_ACC_PUBLIC)
	PHP_ME(SessionHandler, gc, arginfo_session_class_gc, ZEND_ACC_PUBLIC)
	PHP_ME(SessionHandler, create_sid, arginfo_session_class_create_sid, ZEND_ACC_PUBLIC)
	{ NULL, NULL, NULL }
};
/* }}} */

/* ********************************
   * Module Setup and Destruction *
   ******************************** */

static int php_rinit_session(zend_bool auto_start TSRMLS_DC) /* {{{ */
{
	php_rinit_session_globals(TSRMLS_C);

	if (PS(mod) == NULL) {
		char *value;

		value = zend_ini_string("session.save_handler", sizeof("session.save_handler"), 0);
		if (value) {
			PS(mod) = _php_find_ps_module(value TSRMLS_CC);
		}
	}

	if (PS(serializer) == NULL) {
		char *value;

		value = zend_ini_string("session.serialize_handler", sizeof("session.serialize_handler"), 0);
		if (value) {
			PS(serializer) = _php_find_ps_serializer(value TSRMLS_CC);
		}
	}

	if (PS(mod) == NULL || PS(serializer) == NULL) {
		/* current status is unusable */
		PS(session_status) = php_session_disabled;
		return SUCCESS;
	}

	if (auto_start) {
		php_session_start(TSRMLS_C);
	}

	return SUCCESS;
} /* }}} */

static PHP_RINIT_FUNCTION(session) /* {{{ */
{
	return php_rinit_session(PS(auto_start) TSRMLS_CC);
}
/* }}} */

static PHP_RSHUTDOWN_FUNCTION(session) /* {{{ */
{
	int i;

	zend_try {
		php_session_flush(TSRMLS_C);
	} zend_end_try();
	php_rshutdown_session_globals(TSRMLS_C);

	/* this should NOT be done in php_rshutdown_session_globals() */
	for (i = 0; i < 7; i++) {
		if (PS(mod_user_names).names[i] != NULL) {
			zval_ptr_dtor(&PS(mod_user_names).names[i]);
			PS(mod_user_names).names[i] = NULL;
		}
	}

	return SUCCESS;
}
/* }}} */

static PHP_GINIT_FUNCTION(ps) /* {{{ */
{
	int i;

	ps_globals->save_path = NULL;
	ps_globals->session_name = NULL;
	ps_globals->id = NULL;
	ps_globals->mod = NULL;
	ps_globals->serializer = NULL;
	ps_globals->mod_data = NULL;
	ps_globals->session_status = php_session_none;
	ps_globals->default_mod = NULL;
	ps_globals->mod_user_implemented = 0;
	ps_globals->mod_user_is_open = 0;
	for (i = 0; i < 7; i++) {
		ps_globals->mod_user_names.names[i] = NULL;
	}
	ps_globals->http_session_vars = NULL;
}
/* }}} */

static PHP_MINIT_FUNCTION(session) /* {{{ */
{
	zend_class_entry ce;

	zend_register_auto_global("_SESSION", sizeof("_SESSION")-1, 0, NULL TSRMLS_CC);

	PS(module_number) = module_number; /* if we really need this var we need to init it in zts mode as well! */

	PS(session_status) = php_session_none;
	REGISTER_INI_ENTRIES();

#ifdef HAVE_LIBMM
	PHP_MINIT(ps_mm) (INIT_FUNC_ARGS_PASSTHRU);
#endif
	php_session_rfc1867_orig_callback = php_rfc1867_callback;
	php_rfc1867_callback = php_session_rfc1867_callback;

	/* Register interfaces */
	INIT_CLASS_ENTRY(ce, PS_IFACE_NAME, php_session_iface_functions);
	php_session_iface_entry = zend_register_internal_class(&ce TSRMLS_CC);
	php_session_iface_entry->ce_flags |= ZEND_ACC_INTERFACE;

	INIT_CLASS_ENTRY(ce, PS_SID_IFACE_NAME, php_session_id_iface_functions);
	php_session_id_iface_entry = zend_register_internal_class(&ce TSRMLS_CC);
	php_session_id_iface_entry->ce_flags |= ZEND_ACC_INTERFACE;

	/* Register base class */
	INIT_CLASS_ENTRY(ce, PS_CLASS_NAME, php_session_class_functions);
	php_session_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(php_session_class_entry TSRMLS_CC, 1, php_session_iface_entry);
	zend_class_implements(php_session_class_entry TSRMLS_CC, 1, php_session_id_iface_entry);

	REGISTER_LONG_CONSTANT("PHP_SESSION_DISABLED", php_session_disabled, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_SESSION_NONE", php_session_none, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_SESSION_ACTIVE", php_session_active, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

static PHP_MSHUTDOWN_FUNCTION(session) /* {{{ */
{
	UNREGISTER_INI_ENTRIES();

#ifdef HAVE_LIBMM
	PHP_MSHUTDOWN(ps_mm) (SHUTDOWN_FUNC_ARGS_PASSTHRU);
#endif

	/* reset rfc1867 callbacks */
	php_session_rfc1867_orig_callback = NULL;
	if (php_rfc1867_callback == php_session_rfc1867_callback) {
		php_rfc1867_callback = NULL;
	}

	ps_serializers[PREDEFINED_SERIALIZERS].name = NULL;
	memset(&ps_modules[PREDEFINED_MODULES], 0, (MAX_MODULES-PREDEFINED_MODULES)*sizeof(ps_module *));

	return SUCCESS;
}
/* }}} */

static PHP_MINFO_FUNCTION(session) /* {{{ */
{
	ps_module **mod;
	ps_serializer *ser;
	smart_str save_handlers = {0};
	smart_str ser_handlers = {0};
	int i;

	/* Get save handlers */
	for (i = 0, mod = ps_modules; i < MAX_MODULES; i++, mod++) {
		if (*mod && (*mod)->s_name) {
			smart_str_appends(&save_handlers, (*mod)->s_name);
			smart_str_appendc(&save_handlers, ' ');
		}
	}

	/* Get serializer handlers */
	for (i = 0, ser = ps_serializers; i < MAX_SERIALIZERS; i++, ser++) {
		if (ser && ser->name) {
			smart_str_appends(&ser_handlers, ser->name);
			smart_str_appendc(&ser_handlers, ' ');
		}
	}

	php_info_print_table_start();
	php_info_print_table_row(2, "Session Support", "enabled" );

	if (save_handlers.c) {
		smart_str_0(&save_handlers);
		php_info_print_table_row(2, "Registered save handlers", save_handlers.c);
		smart_str_free(&save_handlers);
	} else {
		php_info_print_table_row(2, "Registered save handlers", "none");
	}

	if (ser_handlers.c) {
		smart_str_0(&ser_handlers);
		php_info_print_table_row(2, "Registered serializer handlers", ser_handlers.c);
		smart_str_free(&ser_handlers);
	} else {
		php_info_print_table_row(2, "Registered serializer handlers", "none");
	}

	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

static const zend_module_dep session_deps[] = { /* {{{ */
	ZEND_MOD_OPTIONAL("hash")
	ZEND_MOD_REQUIRED("spl")
	ZEND_MOD_END
};
/* }}} */

/* ************************
   * Upload hook handling *
   ************************ */

static zend_bool early_find_sid_in(zval *dest, int where, php_session_rfc1867_progress *progress TSRMLS_DC) /* {{{ */
{
	zval **ppid;

	if (!PG(http_globals)[where]) {
		return 0;
	}

	if (zend_hash_find(Z_ARRVAL_P(PG(http_globals)[where]), PS(session_name), progress->sname_len+1, (void **)&ppid) == SUCCESS
			&& Z_TYPE_PP(ppid) == IS_STRING) {
		zval_dtor(dest);
		ZVAL_ZVAL(dest, *ppid, 1, 0);
		return 1;
	}

	return 0;
} /* }}} */

static void php_session_rfc1867_early_find_sid(php_session_rfc1867_progress *progress TSRMLS_DC) /* {{{ */
{

	if (PS(use_cookies)) {
		sapi_module.treat_data(PARSE_COOKIE, NULL, NULL TSRMLS_CC);
		if (early_find_sid_in(&progress->sid, TRACK_VARS_COOKIE, progress TSRMLS_CC)) {
			progress->apply_trans_sid = 0;
			return;
		}
	}
	if (PS(use_only_cookies)) {
		return;
	}
	sapi_module.treat_data(PARSE_GET, NULL, NULL TSRMLS_CC);
	early_find_sid_in(&progress->sid, TRACK_VARS_GET, progress TSRMLS_CC);
} /* }}} */

static zend_bool php_check_cancel_upload(php_session_rfc1867_progress *progress TSRMLS_DC) /* {{{ */
{
	zval **progress_ary, **cancel_upload;

	if (zend_symtable_find(Z_ARRVAL_P(PS(http_session_vars)), progress->key.c, progress->key.len+1, (void**)&progress_ary) != SUCCESS) {
		return 0;
	}
	if (Z_TYPE_PP(progress_ary) != IS_ARRAY) {
		return 0;
	}
	if (zend_hash_find(Z_ARRVAL_PP(progress_ary), "cancel_upload", sizeof("cancel_upload"), (void**)&cancel_upload) != SUCCESS) {
		return 0;
	}
	return Z_TYPE_PP(cancel_upload) == IS_BOOL && Z_LVAL_PP(cancel_upload);
} /* }}} */

static void php_session_rfc1867_update(php_session_rfc1867_progress *progress, int force_update TSRMLS_DC) /* {{{ */
{
	if (!force_update) {
		if (Z_LVAL_P(progress->post_bytes_processed) < progress->next_update) {
			return;
		}
#ifdef HAVE_GETTIMEOFDAY
		if (PS(rfc1867_min_freq) > 0.0) {
			struct timeval tv = {0};
			double dtv;
			gettimeofday(&tv, NULL);
			dtv = (double) tv.tv_sec + tv.tv_usec / 1000000.0;
			if (dtv < progress->next_update_time) {
				return;
			}
			progress->next_update_time = dtv + PS(rfc1867_min_freq);
		}
#endif
		progress->next_update = Z_LVAL_P(progress->post_bytes_processed) + progress->update_step;
	}

	php_session_initialize(TSRMLS_C);
	PS(session_status) = php_session_active;
	IF_SESSION_VARS() {
		progress->cancel_upload |= php_check_cancel_upload(progress TSRMLS_CC);
		ZEND_SET_SYMBOL_WITH_LENGTH(Z_ARRVAL_P(PS(http_session_vars)), progress->key.c, progress->key.len+1, progress->data, 2, 0);
	}
	php_session_flush(TSRMLS_C);
} /* }}} */

static void php_session_rfc1867_cleanup(php_session_rfc1867_progress *progress TSRMLS_DC) /* {{{ */
{
	php_session_initialize(TSRMLS_C);
	PS(session_status) = php_session_active;
	IF_SESSION_VARS() {
		zend_hash_del(Z_ARRVAL_P(PS(http_session_vars)), progress->key.c, progress->key.len+1);
	}
	php_session_flush(TSRMLS_C);
} /* }}} */

static int php_session_rfc1867_callback(unsigned int event, void *event_data, void **extra TSRMLS_DC) /* {{{ */
{
	php_session_rfc1867_progress *progress;
	int retval = SUCCESS;

	if (php_session_rfc1867_orig_callback) {
		retval = php_session_rfc1867_orig_callback(event, event_data, extra TSRMLS_CC);
	}
	if (!PS(rfc1867_enabled)) {
		return retval;
	}

	progress = PS(rfc1867_progress);

	switch(event) {
		case MULTIPART_EVENT_START: {
			multipart_event_start *data = (multipart_event_start *) event_data;
			progress = ecalloc(1, sizeof(php_session_rfc1867_progress));
			progress->content_length = data->content_length;
			progress->sname_len  = strlen(PS(session_name));
			PS(rfc1867_progress) = progress;
		}
		break;
		case MULTIPART_EVENT_FORMDATA: {
			multipart_event_formdata *data = (multipart_event_formdata *) event_data;
			size_t value_len;

			if (Z_TYPE(progress->sid) && progress->key.c) {
				break;
			}

			/* orig callback may have modified *data->newlength */
			if (data->newlength) {
				value_len = *data->newlength;
			} else {
				value_len = data->length;
			}

			if (data->name && data->value && value_len) {
				size_t name_len = strlen(data->name);

				if (name_len == progress->sname_len && memcmp(data->name, PS(session_name), name_len) == 0) {
					zval_dtor(&progress->sid);
					ZVAL_STRINGL(&progress->sid, (*data->value), value_len, 1);

				} else if (name_len == PS(rfc1867_name).len && memcmp(data->name, PS(rfc1867_name).c, name_len) == 0) {
					smart_str_free(&progress->key);
					smart_str_appendl(&progress->key, PS(rfc1867_prefix).c, PS(rfc1867_prefix).len);
					smart_str_appendl(&progress->key, *data->value, value_len);
					smart_str_0(&progress->key);

					progress->apply_trans_sid = PS(use_trans_sid);
					php_session_rfc1867_early_find_sid(progress TSRMLS_CC);
				}
			}
		}
		break;
		case MULTIPART_EVENT_FILE_START: {
			multipart_event_file_start *data = (multipart_event_file_start *) event_data;

			/* Do nothing when $_POST["PHP_SESSION_UPLOAD_PROGRESS"] is not set 
			 * or when we have no session id */
			if (!Z_TYPE(progress->sid) || !progress->key.c) {
				break;
			}
			
			/* First FILE_START event, initializing data */
			if (!progress->data) {

				if (PS(rfc1867_freq) >= 0) {
					progress->update_step = PS(rfc1867_freq);
				} else if (PS(rfc1867_freq) < 0) { /* % of total size */
					progress->update_step = progress->content_length * -PS(rfc1867_freq) / 100;
				}
				progress->next_update = 0;
				progress->next_update_time = 0.0;

				ALLOC_INIT_ZVAL(progress->data);
				array_init(progress->data);

				ALLOC_INIT_ZVAL(progress->post_bytes_processed);
				ZVAL_LONG(progress->post_bytes_processed, data->post_bytes_processed);

				ALLOC_INIT_ZVAL(progress->files);
				array_init(progress->files);

				add_assoc_long_ex(progress->data, "start_time",      sizeof("start_time"),      (long)sapi_get_request_time(TSRMLS_C));
				add_assoc_long_ex(progress->data, "content_length",  sizeof("content_length"),  progress->content_length);
				add_assoc_zval_ex(progress->data, "bytes_processed", sizeof("bytes_processed"), progress->post_bytes_processed);
				add_assoc_bool_ex(progress->data, "done",            sizeof("done"),            0);
				add_assoc_zval_ex(progress->data, "files",           sizeof("files"),           progress->files);

				php_rinit_session(0 TSRMLS_CC);
				PS(id) = estrndup(Z_STRVAL(progress->sid), Z_STRLEN(progress->sid));
				PS(apply_trans_sid) = progress->apply_trans_sid;
				PS(send_cookie) = 0;
			}

			ALLOC_INIT_ZVAL(progress->current_file);
			array_init(progress->current_file);

			ALLOC_INIT_ZVAL(progress->current_file_bytes_processed);
			ZVAL_LONG(progress->current_file_bytes_processed, 0);

			/* Each uploaded file has its own array. Trying to make it close to $_FILES entries. */
			add_assoc_string_ex(progress->current_file, "field_name",    sizeof("field_name"),      data->name, 1);
			add_assoc_string_ex(progress->current_file, "name",          sizeof("name"),            *data->filename, 1);
			add_assoc_null_ex(progress->current_file, "tmp_name",        sizeof("tmp_name"));
			add_assoc_long_ex(progress->current_file, "error",           sizeof("error"),           0);

			add_assoc_bool_ex(progress->current_file, "done",            sizeof("done"),            0);
			add_assoc_long_ex(progress->current_file, "start_time",      sizeof("start_time"),      (long)time(NULL));
			add_assoc_zval_ex(progress->current_file, "bytes_processed", sizeof("bytes_processed"), progress->current_file_bytes_processed);

			add_next_index_zval(progress->files, progress->current_file);
			
			Z_LVAL_P(progress->post_bytes_processed) = data->post_bytes_processed;

			php_session_rfc1867_update(progress, 0 TSRMLS_CC);
		}
		break;
		case MULTIPART_EVENT_FILE_DATA: {
			multipart_event_file_data *data = (multipart_event_file_data *) event_data;

			if (!Z_TYPE(progress->sid) || !progress->key.c) {
				break;
			}
			
			Z_LVAL_P(progress->current_file_bytes_processed) = data->offset + data->length;
			Z_LVAL_P(progress->post_bytes_processed) = data->post_bytes_processed;

			php_session_rfc1867_update(progress, 0 TSRMLS_CC);
		}
		break;
		case MULTIPART_EVENT_FILE_END: {
			multipart_event_file_end *data = (multipart_event_file_end *) event_data;

			if (!Z_TYPE(progress->sid) || !progress->key.c) {
				break;
			}
			
			if (data->temp_filename) {
				add_assoc_string_ex(progress->current_file, "tmp_name",  sizeof("tmp_name"), data->temp_filename, 1);
			}
			add_assoc_long_ex(progress->current_file, "error", sizeof("error"), data->cancel_upload);
			add_assoc_bool_ex(progress->current_file, "done",  sizeof("done"),  1);

			Z_LVAL_P(progress->post_bytes_processed) = data->post_bytes_processed;

			php_session_rfc1867_update(progress, 0 TSRMLS_CC);
		}
		break;
		case MULTIPART_EVENT_END: {
			multipart_event_end *data = (multipart_event_end *) event_data;

			if (Z_TYPE(progress->sid) && progress->key.c) {
				if (PS(rfc1867_cleanup)) {
					php_session_rfc1867_cleanup(progress TSRMLS_CC);
				} else {
					add_assoc_bool_ex(progress->data, "done", sizeof("done"), 1);
					Z_LVAL_P(progress->post_bytes_processed) = data->post_bytes_processed;
					php_session_rfc1867_update(progress, 1 TSRMLS_CC);
				}
				php_rshutdown_session_globals(TSRMLS_C);
			}

			if (progress->data) {
				zval_ptr_dtor(&progress->data);
			}
			zval_dtor(&progress->sid);
			smart_str_free(&progress->key);
			efree(progress);
			progress = NULL;
			PS(rfc1867_progress) = NULL;
		}
		break;
	}

	if (progress && progress->cancel_upload) {
		return FAILURE;
	}
	return retval;

} /* }}} */

zend_module_entry session_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	session_deps,
	"session",
	session_functions,
	PHP_MINIT(session), PHP_MSHUTDOWN(session),
	PHP_RINIT(session), PHP_RSHUTDOWN(session),
	PHP_MINFO(session),
	NO_VERSION_YET,
	PHP_MODULE_GLOBALS(ps),
	PHP_GINIT(ps),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_SESSION
ZEND_GET_MODULE(session)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
