/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
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
#include "zend_smart_str.h"
#include "ext/standard/url.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/head.h"

#include "mod_files.h"
#include "mod_user.h"

#ifdef HAVE_LIBMM
#include "mod_mm.h"
#endif

PHPAPI ZEND_DECLARE_MODULE_GLOBALS(ps)

static int php_session_rfc1867_callback(unsigned int event, void *event_data, void **extra);
static int (*php_session_rfc1867_orig_callback)(unsigned int event, void *event_data, void **extra);
static void php_session_track_init(void);

/* SessionHandler class */
zend_class_entry *php_session_class_entry;

/* SessionHandlerInterface */
zend_class_entry *php_session_iface_entry;

/* SessionIdInterface */
zend_class_entry *php_session_id_iface_entry;

/* SessionUpdateTimestampHandler class */
zend_class_entry *php_session_update_timestamp_class_entry;

/* SessionUpdateTimestampInterface */
zend_class_entry *php_session_update_timestamp_iface_entry;

/* ***********
   * Helpers *
   *********** */

#define IF_SESSION_VARS() \
	if (Z_ISREF_P(&PS(http_session_vars)) && Z_TYPE_P(Z_REFVAL(PS(http_session_vars))) == IS_ARRAY)

#define SESSION_CHECK_ACTIVE_STATE	\
	if (PS(session_status) == php_session_active) {	\
		php_error_docref(NULL, E_WARNING, "A session is active. You cannot change the session module's ini settings at this time");	\
		return FAILURE;	\
	}

#define APPLY_TRANS_SID (PS(use_trans_sid) && !PS(use_only_cookies))

static void php_session_send_cookie(void);

/* Dispatched by RINIT and by php_session_destroy */
static inline void php_rinit_session_globals(void) /* {{{ */
{
	/* Do NOT init PS(mod_user_names) here! */
	PS(id) = NULL;
	PS(session_status) = php_session_none;
	PS(mod_data) = NULL;
	PS(mod_user_is_open) = 0;
	PS(define_sid) = 1;
	PS(session_vars) = NULL;
	ZVAL_UNDEF(&PS(http_session_vars));
}
/* }}} */

/* Dispatched by RSHUTDOWN and by php_session_destroy */
static inline void php_rshutdown_session_globals(void) /* {{{ */
{
	/* Do NOT destroy PS(mod_user_names) here! */
	if (!Z_ISUNDEF(PS(http_session_vars))) {
		zval_ptr_dtor(&PS(http_session_vars));
		ZVAL_UNDEF(&PS(http_session_vars));
	}
	if (PS(mod_data) || PS(mod_user_implemented)) {
		zend_try {
			PS(mod)->s_close(&PS(mod_data));
		} zend_end_try();
	}
	if (PS(id)) {
		zend_string_release(PS(id));
		PS(id) = NULL;
	}
	if (PS(session_vars)) {
		zend_string_release(PS(session_vars));
		PS(session_vars) = NULL;
	}
}
/* }}} */

static int php_session_destroy(void) /* {{{ */
{
	int retval = SUCCESS;

	if (PS(session_status) != php_session_active) {
		php_error_docref(NULL, E_WARNING, "Trying to destroy uninitialized session");
		return FAILURE;
	}

	if (PS(id) && PS(mod)->s_destroy(&PS(mod_data), PS(id)) == FAILURE) {
		retval = FAILURE;
		php_error_docref(NULL, E_WARNING, "Session object destruction failed");
	}

	php_rshutdown_session_globals();
	php_rinit_session_globals();

	return retval;
}
/* }}} */

PHPAPI void php_add_session_var(zend_string *name) /* {{{ */
{
	IF_SESSION_VARS() {
		zval *sess_var = Z_REFVAL(PS(http_session_vars));
		SEPARATE_ARRAY(sess_var);
		if (!zend_hash_exists(Z_ARRVAL_P(sess_var), name)) {
			zval empty_var;
			ZVAL_NULL(&empty_var);
			zend_hash_update(Z_ARRVAL_P(sess_var), name, &empty_var);
		}
	}
}
/* }}} */

PHPAPI zval* php_set_session_var(zend_string *name, zval *state_val, php_unserialize_data_t *var_hash) /* {{{ */
{
	IF_SESSION_VARS() {
		zval *sess_var = Z_REFVAL(PS(http_session_vars));
		SEPARATE_ARRAY(sess_var);
		return zend_hash_update(Z_ARRVAL_P(sess_var), name, state_val);
	}
	return NULL;
}
/* }}} */

PHPAPI zval* php_get_session_var(zend_string *name) /* {{{ */
{
	IF_SESSION_VARS() {
		return zend_hash_find(Z_ARRVAL_P(Z_REFVAL(PS(http_session_vars))), name);
	}
	return NULL;
}
/* }}} */

static void php_session_track_init(void) /* {{{ */
{
	zval session_vars;
	zend_string *var_name = zend_string_init("_SESSION", sizeof("_SESSION") - 1, 0);
	/* Unconditionally destroy existing array -- possible dirty data */
	zend_delete_global_variable(var_name);

	if (!Z_ISUNDEF(PS(http_session_vars))) {
		zval_ptr_dtor(&PS(http_session_vars));
	}

	array_init(&session_vars);
	ZVAL_NEW_REF(&PS(http_session_vars), &session_vars);
	Z_ADDREF_P(&PS(http_session_vars));
	zend_hash_update_ind(&EG(symbol_table), var_name, &PS(http_session_vars));
	zend_string_release(var_name);
}
/* }}} */

static zend_string *php_session_encode(void) /* {{{ */
{
	IF_SESSION_VARS() {
		if (!PS(serializer)) {
			php_error_docref(NULL, E_WARNING, "Unknown session.serialize_handler. Failed to encode session object");
			return NULL;
		}
		return PS(serializer)->encode();
	} else {
		php_error_docref(NULL, E_WARNING, "Cannot encode non-existent session");
	}
	return NULL;
}
/* }}} */

static int php_session_decode(zend_string *data) /* {{{ */
{
	if (!PS(serializer)) {
		php_error_docref(NULL, E_WARNING, "Unknown session.serialize_handler. Failed to decode session object");
		return FAILURE;
	}
	if (PS(serializer)->decode(ZSTR_VAL(data), ZSTR_LEN(data)) == FAILURE) {
		php_session_destroy();
		php_session_track_init();
		php_error_docref(NULL, E_WARNING, "Failed to decode session object. Session has been destroyed");
		return FAILURE;
	}
	return SUCCESS;
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

	p = (unsigned char *)in;
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

PHPAPI zend_string *php_session_create_id(PS_CREATE_SID_ARGS) /* {{{ */
{
	PHP_MD5_CTX md5_context;
	PHP_SHA1_CTX sha1_context;
#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH)
	void *hash_context = NULL;
#endif
	unsigned char *digest;
	size_t digest_len;
	char *buf;
	struct timeval tv;
	zval *array;
	zval *token;
	zend_string *outid;
	char *remote_addr = NULL;

	gettimeofday(&tv, NULL);

	if ((array = zend_hash_str_find(&EG(symbol_table), "_SERVER", sizeof("_SERVER") - 1)) &&
		Z_TYPE_P(array) == IS_ARRAY &&
		(token = zend_hash_str_find(Z_ARRVAL_P(array), "REMOTE_ADDR", sizeof("REMOTE_ADDR") - 1)) &&
		Z_TYPE_P(token) == IS_STRING
	) {
		remote_addr = Z_STRVAL_P(token);
	}

	/* maximum 15+19+19+10 bytes */
	spprintf(&buf, 0, "%.15s%ld" ZEND_LONG_FMT "%0.8F", remote_addr ? remote_addr : "", tv.tv_sec, (zend_long)tv.tv_usec, php_combined_lcg() * 10);

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
				efree(buf);
				php_error_docref(NULL, E_ERROR, "Invalid session hash function");
				return NULL;
			}

			hash_context = emalloc(PS(hash_ops)->context_size);
			PS(hash_ops)->hash_init(hash_context);
			PS(hash_ops)->hash_update(hash_context, (unsigned char *) buf, strlen(buf));
			digest_len = PS(hash_ops)->digest_size;
			break;
#endif /* HAVE_HASH_EXT */
		default:
			efree(buf);
			php_error_docref(NULL, E_ERROR, "Invalid session hash function");
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

		php_error_docref(NULL, E_WARNING, "The ini setting hash_bits_per_character is out of range (should be 4, 5, or 6) - using 4 for now");
	}

	outid = zend_string_alloc((digest_len + 2) * ((8.0f / PS(hash_bits_per_character) + 0.5)), 0);
	ZSTR_LEN(outid) = (size_t)(bin_to_readable((char *)digest, digest_len, ZSTR_VAL(outid), (char)PS(hash_bits_per_character)) - (char *)&ZSTR_VAL(outid));
	efree(digest);

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


static void php_session_gc(void) /* {{{ */
{
	int nrand;

	/* GC must be done before reading session data. */
	if ((PS(mod_data) || PS(mod_user_implemented)) && PS(gc_probability) > 0) {
		int nrdels = -1;

		nrand = (int) ((float) PS(gc_divisor) * php_combined_lcg());
		if (nrand < PS(gc_probability)) {
			PS(mod)->s_gc(&PS(mod_data), PS(gc_maxlifetime), &nrdels);
#ifdef SESSION_DEBUG
			if (nrdels != -1) {
				php_error_docref(NULL, E_NOTICE, "purged %d expired session objects", nrdels);
			}
#endif
		}
	}
} /* }}} */

static void php_session_initialize(void) /* {{{ */
{
	zend_string *val = NULL;

	if (!PS(mod)) {
		php_error_docref(NULL, E_ERROR, "No storage module chosen - failed to initialize session");
		return;
	}

	/* Open session handler first */
	if (PS(mod)->s_open(&PS(mod_data), PS(save_path), PS(session_name)) == FAILURE
		/* || PS(mod_data) == NULL */ /* FIXME: open must set valid PS(mod_data) with success */
	) {
		php_error_docref(NULL, E_ERROR, "Failed to initialize storage module: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		return;
	}

	/* If there is no ID, use session module to create one */
	if (!PS(id)) {
		PS(id) = PS(mod)->s_create_sid(&PS(mod_data));
		if (!PS(id)) {
			php_error_docref(NULL, E_ERROR, "Failed to create session ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
			return;
		}
		if (PS(use_cookies)) {
			PS(send_cookie) = 1;
		}
	} else if (PS(use_strict_mode) && PS(mod)->s_validate_sid &&
		PS(mod)->s_validate_sid(&PS(mod_data), PS(id)) == FAILURE) {
		if (PS(id)) {
			zend_string_release(PS(id));
		}
		PS(id) = PS(mod)->s_create_sid(&PS(mod_data));
		if (!PS(id)) {
			PS(id) = php_session_create_id(NULL);
		}
		if (PS(use_cookies)) {
			PS(send_cookie) = 1;
		}
	}

	php_session_reset_id();
	PS(session_status) = php_session_active;

	/* Read data */
	php_session_track_init();
	if (PS(mod)->s_read(&PS(mod_data), PS(id), &val, PS(gc_maxlifetime)) == FAILURE) {
		/* Some broken save handler implementation returns FAILURE for non-existent session ID */
		/* It's better to raise error for this, but disabled error for better compatibility */
		/*
		php_error_docref(NULL, E_NOTICE, "Failed to read session data: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		*/
	}

	/* GC must be done after read */
	php_session_gc();

	if (PS(session_vars)) {
		zend_string_release(PS(session_vars));
		PS(session_vars) = NULL;
	}
	if (val) {
		if (PS(lazy_write)) {
			PS(session_vars) = zend_string_copy(val);
		}
		php_session_decode(val);
		zend_string_release(val);
	}
}
/* }}} */

static void php_session_save_current_state(int write) /* {{{ */
{
	int ret = FAILURE;

	if (write) {
		IF_SESSION_VARS() {
			if (PS(mod_data) || PS(mod_user_implemented)) {
				zend_string *val;

				val = php_session_encode();
				if (val) {
					if (PS(lazy_write) && PS(session_vars)
						&& PS(mod)->s_update_timestamp
						&& PS(mod)->s_update_timestamp != php_session_update_timestamp
						&& ZSTR_LEN(val) == ZSTR_LEN(PS(session_vars))
						&& !memcmp(ZSTR_VAL(val), ZSTR_VAL(PS(session_vars)), ZSTR_LEN(val))
					) {
						ret = PS(mod)->s_update_timestamp(&PS(mod_data), PS(id), val, PS(gc_maxlifetime));
					} else {
						ret = PS(mod)->s_write(&PS(mod_data), PS(id), val, PS(gc_maxlifetime));
					}
					zend_string_release(val);
				} else {
					ret = PS(mod)->s_write(&PS(mod_data), PS(id), ZSTR_EMPTY_ALLOC(), PS(gc_maxlifetime));
				}
			}

			if ((ret == FAILURE) && !EG(exception)) {
				php_error_docref(NULL, E_WARNING, "Failed to write session data (%s). Please "
								 "verify that the current setting of session.save_path "
								 "is correct (%s)",
								 PS(mod)->s_name,
								 PS(save_path));
			}
		}
	}

	if (PS(mod_data) || PS(mod_user_implemented)) {
		PS(mod)->s_close(&PS(mod_data));
	}
}
/* }}} */

static void php_session_normalize_vars() /* {{{ */
{
	PS_ENCODE_VARS;

	IF_SESSION_VARS() {
		PS_ENCODE_LOOP(
			if (Z_TYPE_P(struc) == IS_PTR) {
				zval *zv = (zval *)Z_PTR_P(struc);
				ZVAL_COPY_VALUE(struc, zv);
				ZVAL_UNDEF(zv);
			}
		);
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

	tmp = _php_find_ps_module(ZSTR_VAL(new_value));

	if (PG(modules_activated) && !tmp) {
		int err_type;

		if (stage == ZEND_INI_STAGE_RUNTIME) {
			err_type = E_WARNING;
		} else {
			err_type = E_ERROR;
		}

		/* Do not output error when restoring ini options. */
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			php_error_docref(NULL, err_type, "Cannot find save handler '%s'", ZSTR_VAL(new_value));
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

	tmp = _php_find_ps_serializer(ZSTR_VAL(new_value));

	if (PG(modules_activated) && !tmp) {
		int err_type;

		if (stage == ZEND_INI_STAGE_RUNTIME) {
			err_type = E_WARNING;
		} else {
			err_type = E_ERROR;
		}

		/* Do not output error when restoring ini options. */
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			php_error_docref(NULL, err_type, "Cannot find serialization handler '%s'", ZSTR_VAL(new_value));
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

	if (!strncasecmp(ZSTR_VAL(new_value), "on", sizeof("on"))) {
		PS(use_trans_sid) = (zend_bool) 1;
	} else {
		PS(use_trans_sid) = (zend_bool) atoi(ZSTR_VAL(new_value));
	}

	return SUCCESS;
}
/* }}} */

static PHP_INI_MH(OnUpdateSaveDir) /* {{{ */
{
	/* Only do the safemode/open_basedir check at runtime */
	if (stage == PHP_INI_STAGE_RUNTIME || stage == PHP_INI_STAGE_HTACCESS) {
		char *p;

		if (memchr(ZSTR_VAL(new_value), '\0', ZSTR_LEN(new_value)) != NULL) {
			return FAILURE;
		}

		/* we do not use zend_memrchr() since path can contain ; itself */
		if ((p = strchr(ZSTR_VAL(new_value), ';'))) {
			char *p2;
			p++;
			if ((p2 = strchr(p, ';'))) {
				p = p2 + 1;
			}
		} else {
			p = ZSTR_VAL(new_value);
		}

		if (PG(open_basedir) && *p && php_check_open_basedir(p)) {
			return FAILURE;
		}
	}

	OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	return SUCCESS;
}
/* }}} */

static PHP_INI_MH(OnUpdateName) /* {{{ */
{
	/* Numeric session.name won't work at all */
	if ((!ZSTR_LEN(new_value) || is_numeric_string(ZSTR_VAL(new_value), ZSTR_LEN(new_value), NULL, NULL, 0))) {
		int err_type;

		if (stage == ZEND_INI_STAGE_RUNTIME || stage == ZEND_INI_STAGE_ACTIVATE || stage == ZEND_INI_STAGE_STARTUP) {
			err_type = E_WARNING;
		} else {
			err_type = E_ERROR;
		}

		/* Do not output error when restoring ini options. */
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			php_error_docref(NULL, err_type, "session.name cannot be a numeric or empty '%s'", ZSTR_VAL(new_value));
		}
		return FAILURE;
	}

	OnUpdateStringUnempty(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	return SUCCESS;
}
/* }}} */

static PHP_INI_MH(OnUpdateHashFunc) /* {{{ */
{
	zend_long val;
	char *endptr = NULL;

#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH)
	PS(hash_ops) = NULL;
#endif

	val = ZEND_STRTOL(ZSTR_VAL(new_value), &endptr, 10);
	if (endptr && (*endptr == '\0')) {
		/* Numeric value */
		PS(hash_func) = val ? 1 : 0;

		return SUCCESS;
	}

	if (ZSTR_LEN(new_value) == (sizeof("md5") - 1) &&
		strncasecmp(ZSTR_VAL(new_value), "md5", sizeof("md5") - 1) == 0) {
		PS(hash_func) = PS_HASH_FUNC_MD5;

		return SUCCESS;
	}

	if (ZSTR_LEN(new_value) == (sizeof("sha1") - 1) &&
		strncasecmp(ZSTR_VAL(new_value), "sha1", sizeof("sha1") - 1) == 0) {
		PS(hash_func) = PS_HASH_FUNC_SHA1;

		return SUCCESS;
	}

#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH) /* {{{ */
{
	php_hash_ops *ops = (php_hash_ops*)php_hash_fetch_ops(ZSTR_VAL(new_value), ZSTR_LEN(new_value));

	if (ops) {
		PS(hash_func) = PS_HASH_FUNC_OTHER;
		PS(hash_ops) = ops;

		return SUCCESS;
	}
}
#endif /* HAVE_HASH_EXT }}} */

	php_error_docref(NULL, E_WARNING, "session.configuration 'session.hash_function' must be existing hash function. %s does not exist.", ZSTR_VAL(new_value));
	return FAILURE;
}
/* }}} */

static PHP_INI_MH(OnUpdateRfc1867Freq) /* {{{ */
{
	int tmp;
	tmp = zend_atoi(ZSTR_VAL(new_value), (int)ZSTR_LEN(new_value));
	if(tmp < 0) {
		php_error_docref(NULL, E_WARNING, "session.upload_progress.freq must be greater than or equal to zero");
		return FAILURE;
	}
	if(ZSTR_LEN(new_value) > 0 && ZSTR_VAL(new_value)[ZSTR_LEN(new_value)-1] == '%') {
		if(tmp > 100) {
			php_error_docref(NULL, E_WARNING, "session.upload_progress.freq cannot be over 100%%");
			return FAILURE;
		}
		PS(rfc1867_freq) = -tmp;
	} else {
		PS(rfc1867_freq) = tmp;
	}
	return SUCCESS;
} /* }}} */

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
	STD_PHP_INI_BOOLEAN("session.lazy_write",       "1",         PHP_INI_ALL, OnUpdateBool,   lazy_write,         php_ps_globals,    ps_globals)

	/* Upload progress */
	STD_PHP_INI_BOOLEAN("session.upload_progress.enabled",
	                                                "1",     ZEND_INI_PERDIR, OnUpdateBool,        rfc1867_enabled, php_ps_globals, ps_globals)
	STD_PHP_INI_BOOLEAN("session.upload_progress.cleanup",
	                                                "1",     ZEND_INI_PERDIR, OnUpdateBool,        rfc1867_cleanup, php_ps_globals, ps_globals)
	STD_PHP_INI_ENTRY("session.upload_progress.prefix",
	                                     "upload_progress_", ZEND_INI_PERDIR, OnUpdateString,      rfc1867_prefix,  php_ps_globals, ps_globals)
	STD_PHP_INI_ENTRY("session.upload_progress.name",
	                          "PHP_SESSION_UPLOAD_PROGRESS", ZEND_INI_PERDIR, OnUpdateString,      rfc1867_name,    php_ps_globals, ps_globals)
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

	IF_SESSION_VARS() {
		PHP_VAR_SERIALIZE_INIT(var_hash);
		php_var_serialize(&buf, Z_REFVAL(PS(http_session_vars)), &var_hash);
		PHP_VAR_SERIALIZE_DESTROY(var_hash);
	}
	return buf.s;
}
/* }}} */

PS_SERIALIZER_DECODE_FUNC(php_serialize) /* {{{ */
{
	const char *endptr = val + vallen;
	zval session_vars;
	php_unserialize_data_t var_hash;
	int result;
	zend_string *var_name = zend_string_init("_SESSION", sizeof("_SESSION") - 1, 0);

	ZVAL_NULL(&session_vars);
	PHP_VAR_UNSERIALIZE_INIT(var_hash);
	result = php_var_unserialize(
		&session_vars, (const unsigned char **)&val, (const unsigned char *)endptr, &var_hash);
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	if (!result) {
		zval_ptr_dtor(&session_vars);
		ZVAL_NULL(&session_vars);
	}

	if (!Z_ISUNDEF(PS(http_session_vars))) {
		zval_ptr_dtor(&PS(http_session_vars));
	}
	if (Z_TYPE(session_vars) == IS_NULL) {
		array_init(&session_vars);
	}
	ZVAL_NEW_REF(&PS(http_session_vars), &session_vars);
	Z_ADDREF_P(&PS(http_session_vars));
	zend_hash_update_ind(&EG(symbol_table), var_name, &PS(http_session_vars));
	zend_string_release(var_name);
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
			if (ZSTR_LEN(key) > PS_BIN_MAX) continue;
			smart_str_appendc(&buf, (unsigned char)ZSTR_LEN(key));
			smart_str_appendl(&buf, ZSTR_VAL(key), ZSTR_LEN(key));
			php_var_serialize(&buf, struc, &var_hash);
		} else {
			if (ZSTR_LEN(key) > PS_BIN_MAX) continue;
			smart_str_appendc(&buf, (unsigned char) (ZSTR_LEN(key) & PS_BIN_UNDEF));
			smart_str_appendl(&buf, ZSTR_VAL(key), ZSTR_LEN(key));
	);

	smart_str_0(&buf);
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	return buf.s;
}
/* }}} */

PS_SERIALIZER_DECODE_FUNC(php_binary) /* {{{ */
{
	const char *p;
	const char *endptr = val + vallen;
	int has_value;
	int namelen;
	zend_string *name;
	php_unserialize_data_t var_hash;
	int skip = 0;

	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	for (p = val; p < endptr; ) {
		zval *tmp;
		skip = 0;
		namelen = ((unsigned char)(*p)) & (~PS_BIN_UNDEF);

		if (namelen < 0 || namelen > PS_BIN_MAX || (p + namelen) >= endptr) {
			PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
			return FAILURE;
		}

		has_value = *p & PS_BIN_UNDEF ? 0 : 1;

		name = zend_string_init(p + 1, namelen, 0);

		p += namelen + 1;

		if ((tmp = zend_hash_find(&EG(symbol_table), name))) {
			if ((Z_TYPE_P(tmp) == IS_ARRAY &&
				Z_ARRVAL_P(tmp) == &EG(symbol_table)) || tmp == &PS(http_session_vars)) {
				skip = 1;
			}
		}

		if (has_value) {
			zval *current, rv;
			current = var_tmp_var(&var_hash);
			if (php_var_unserialize(current, (const unsigned char **) &p, (const unsigned char *) endptr, &var_hash)) {
				ZVAL_PTR(&rv, current);
				if (!skip) {
					php_set_session_var(name, &rv, &var_hash);
				}
			} else {
				zend_string_release(name);
				php_session_normalize_vars();
				PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
				return FAILURE;
			}
		} else {
			PS_ADD_VARL(name);
		}
		zend_string_release(name);
	}

	php_session_normalize_vars();
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
			smart_str_appendl(&buf, ZSTR_VAL(key), ZSTR_LEN(key));
			if (memchr(ZSTR_VAL(key), PS_DELIMITER, ZSTR_LEN(key)) || memchr(ZSTR_VAL(key), PS_UNDEF_MARKER, ZSTR_LEN(key))) {
				PHP_VAR_SERIALIZE_DESTROY(var_hash);
				smart_str_free(&buf);
				return NULL;
			}
			smart_str_appendc(&buf, PS_DELIMITER);

			php_var_serialize(&buf, struc, &var_hash);
		} else {
			smart_str_appendc(&buf, PS_UNDEF_MARKER);
			smart_str_appendl(&buf, ZSTR_VAL(key), ZSTR_LEN(key));
			smart_str_appendc(&buf, PS_DELIMITER);
	);

	smart_str_0(&buf);

	PHP_VAR_SERIALIZE_DESTROY(var_hash);
	return buf.s;
}
/* }}} */

PS_SERIALIZER_DECODE_FUNC(php) /* {{{ */
{
	const char *p, *q;
	const char *endptr = val + vallen;
	ptrdiff_t namelen;
	zend_string *name;
	int has_value, retval = SUCCESS;
	php_unserialize_data_t var_hash;
	int skip = 0;

	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	p = val;

	while (p < endptr) {
		zval *tmp;
		q = p;
		skip = 0;
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
		name = zend_string_init(p, namelen, 0);
		q++;

		if ((tmp = zend_hash_find(&EG(symbol_table), name))) {
			if ((Z_TYPE_P(tmp) == IS_ARRAY &&
				Z_ARRVAL_P(tmp) == &EG(symbol_table)) || tmp == &PS(http_session_vars)) {
				skip = 1;
			}
		}

		if (has_value) {
			zval *current, rv;
			current = var_tmp_var(&var_hash);
			if (php_var_unserialize(current, (const unsigned char **)&q, (const unsigned char *)endptr, &var_hash)) {
				ZVAL_PTR(&rv, current);
				if (!skip) {
					php_set_session_var(name, &rv, &var_hash);
				}
			} else {
				zend_string_release(name);
				retval = FAILURE;
				goto break_outer_loop;
			}
		} else {
			if(!skip) {
				PS_ADD_VARL(name);
			}
		}
		zend_string_release(name);

		p = q;
	}
break_outer_loop:
	php_session_normalize_vars();

	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);

	return retval;
}
/* }}} */

#define MAX_SERIALIZERS 32
#define PREDEFINED_SERIALIZERS 3

static ps_serializer ps_serializers[MAX_SERIALIZERS + 1] = {
	PS_SERIALIZER_ENTRY(php_serialize),
	PS_SERIALIZER_ENTRY(php),
	PS_SERIALIZER_ENTRY(php_binary)
};

PHPAPI int php_session_register_serializer(const char *name, zend_string *(*encode)(PS_SERIALIZER_ENCODE_ARGS), int (*decode)(PS_SERIALIZER_DECODE_ARGS)) /* {{{ */
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

#define MAX_MODULES 32
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

/* Dummy PS module function */
PHPAPI int php_session_validate_sid(PS_VALIDATE_SID_ARGS) {
	return SUCCESS;
}

/* Dummy PS module function */
PHPAPI int php_session_update_timestamp(PS_UPDATE_TIMESTAMP_ARGS) {
	return SUCCESS;
}


/* ******************
   * Cache Limiters *
   ****************** */

typedef struct {
	char *name;
	void (*func)(void);
} php_session_cache_limiter_t;

#define CACHE_LIMITER(name) _php_cache_limiter_##name
#define CACHE_LIMITER_FUNC(name) static void CACHE_LIMITER(name)(void)
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

static inline void last_modified(void) /* {{{ */
{
	const char *path;
	zend_stat_t sb;
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

	snprintf(buf, sizeof(buf) , "Cache-Control: public, max-age=" ZEND_LONG_FMT, PS(cache_expire) * 60); /* SAFE */
	ADD_HEADER(buf);

	last_modified();
}
/* }}} */

CACHE_LIMITER_FUNC(private_no_expire) /* {{{ */
{
	char buf[MAX_STR + 1];

	snprintf(buf, sizeof(buf), "Cache-Control: private, max-age=" ZEND_LONG_FMT, PS(cache_expire) * 60); /* SAFE */
	ADD_HEADER(buf);

	last_modified();
}
/* }}} */

CACHE_LIMITER_FUNC(private) /* {{{ */
{
	ADD_HEADER("Expires: Thu, 19 Nov 1981 08:52:00 GMT");
	CACHE_LIMITER(private_no_expire)();
}
/* }}} */

CACHE_LIMITER_FUNC(nocache) /* {{{ */
{
	ADD_HEADER("Expires: Thu, 19 Nov 1981 08:52:00 GMT");

	/* For HTTP/1.1 conforming clients */
	ADD_HEADER("Cache-Control: no-store, no-cache, must-revalidate");

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

static int php_session_cache_limiter(void) /* {{{ */
{
	php_session_cache_limiter_t *lim;

	if (PS(cache_limiter)[0] == '\0') return 0;

	if (SG(headers_sent)) {
		const char *output_start_filename = php_output_get_start_filename();
		int output_start_lineno = php_output_get_start_lineno();

		if (output_start_filename) {
			php_error_docref(NULL, E_WARNING, "Cannot send session cache limiter - headers already sent (output started at %s:%d)", output_start_filename, output_start_lineno);
		} else {
			php_error_docref(NULL, E_WARNING, "Cannot send session cache limiter - headers already sent");
		}
		return -2;
	}

	for (lim = php_session_cache_limiters; lim->name; lim++) {
		if (!strcasecmp(lim->name, PS(cache_limiter))) {
			lim->func();
			return 0;
		}
	}

	return -1;
}
/* }}} */

/* *********************
   * Cookie Management *
   ********************* */

/*
 * Remove already sent session ID cookie.
 * It must be directly removed from SG(sapi_header) because sapi_add_header_ex()
 * removes all of matching cookie. i.e. It deletes all of Set-Cookie headers.
 */
static void php_session_remove_cookie(void) {
	sapi_header_struct *header;
	zend_llist *l = &SG(sapi_headers).headers;
	zend_llist_element *next;
	zend_llist_element *current;
	char *session_cookie;
	zend_string *e_session_name;
	size_t session_cookie_len;
	size_t len = sizeof("Set-Cookie")-1;

	e_session_name = php_url_encode(PS(session_name), strlen(PS(session_name)));
	spprintf(&session_cookie, 0, "Set-Cookie: %s=", ZSTR_VAL(e_session_name));
	zend_string_free(e_session_name);

	session_cookie_len = strlen(session_cookie);
	current = l->head;
	while (current) {
		header = (sapi_header_struct *)(current->data);
		next = current->next;
		if (header->header_len > len && header->header[len] == ':'
			&& !strncmp(header->header, session_cookie, session_cookie_len)) {
			if (current->prev) {
				current->prev->next = next;
			} else {
				l->head = next;
			}
			if (next) {
				next->prev = current->prev;
			} else {
				l->tail = current->prev;
			}
			sapi_free_header(header);
			efree(current);
			--l->count;
		}
		current = next;
	}
	efree(session_cookie);
}

static void php_session_send_cookie(void) /* {{{ */
{
	smart_str ncookie = {0};
	zend_string *date_fmt = NULL;
	zend_string *e_session_name, *e_id;

	if (SG(headers_sent)) {
		const char *output_start_filename = php_output_get_start_filename();
		int output_start_lineno = php_output_get_start_lineno();

		if (output_start_filename) {
			php_error_docref(NULL, E_WARNING, "Cannot send session cookie - headers already sent by (output started at %s:%d)", output_start_filename, output_start_lineno);
		} else {
			php_error_docref(NULL, E_WARNING, "Cannot send session cookie - headers already sent");
		}
		return;
	}

	/* URL encode session_name and id because they might be user supplied */
	e_session_name = php_url_encode(PS(session_name), strlen(PS(session_name)));
	e_id = php_url_encode(ZSTR_VAL(PS(id)), ZSTR_LEN(PS(id)));

	smart_str_appendl(&ncookie, "Set-Cookie: ", sizeof("Set-Cookie: ")-1);
	smart_str_appendl(&ncookie, ZSTR_VAL(e_session_name), ZSTR_LEN(e_session_name));
	smart_str_appendc(&ncookie, '=');
	smart_str_appendl(&ncookie, ZSTR_VAL(e_id), ZSTR_LEN(e_id));

	zend_string_release(e_session_name);
	zend_string_release(e_id);

	if (PS(cookie_lifetime) > 0) {
		struct timeval tv;
		time_t t;

		gettimeofday(&tv, NULL);
		t = tv.tv_sec + PS(cookie_lifetime);

		if (t > 0) {
			date_fmt = php_format_date("D, d-M-Y H:i:s T", sizeof("D, d-M-Y H:i:s T")-1, t, 0);
			smart_str_appends(&ncookie, COOKIE_EXPIRES);
			smart_str_appendl(&ncookie, ZSTR_VAL(date_fmt), ZSTR_LEN(date_fmt));
			zend_string_release(date_fmt);

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

	php_session_remove_cookie(); /* remove already sent session ID cookie */
	/*	'replace' must be 0 here, else a previous Set-Cookie
		header, probably sent with setcookie() will be replaced! */
	sapi_add_header_ex(estrndup(ZSTR_VAL(ncookie.s), ZSTR_LEN(ncookie.s)), ZSTR_LEN(ncookie.s), 0, 0);
	smart_str_free(&ncookie);
}
/* }}} */

PHPAPI ps_module *_php_find_ps_module(char *name) /* {{{ */
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

PHPAPI const ps_serializer *_php_find_ps_serializer(char *name) /* {{{ */
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

static void ppid2sid(zval *ppid) {
	ZVAL_DEREF(ppid);
	if (Z_TYPE_P(ppid) == IS_STRING) {
		PS(id) = zend_string_init(Z_STRVAL_P(ppid), Z_STRLEN_P(ppid), 0);
		PS(send_cookie) = 0;
	} else {
		PS(id) = NULL;
		PS(send_cookie) = 1;
	}
}

PHPAPI void php_session_reset_id(void) /* {{{ */
{
	int module_number = PS(module_number);
	zval *sid, *data, *ppid;

	if (!PS(id)) {
		php_error_docref(NULL, E_WARNING, "Cannot set session ID - session ID is not initialized");
		return;
	}

	if (PS(use_cookies) && PS(send_cookie)) {
		php_session_send_cookie();
		PS(send_cookie) = 0;
	}

	/* If the SID constant exists, destroy it. */
	/* We must not delete any items in EG(zend_contants) */
	/* zend_hash_str_del(EG(zend_constants), "sid", sizeof("sid") - 1); */
	sid = zend_get_constant_str("SID", sizeof("SID") - 1);

	if (PS(define_sid)) {
		smart_str var = {0};

		smart_str_appends(&var, PS(session_name));
		smart_str_appendc(&var, '=');
		smart_str_appends(&var, ZSTR_VAL(PS(id)));
		smart_str_0(&var);
		if (sid) {
			zend_string_release(Z_STR_P(sid));
			ZVAL_NEW_STR(sid, var.s);
		} else {
			REGISTER_STRINGL_CONSTANT("SID", ZSTR_VAL(var.s), ZSTR_LEN(var.s), 0);
			smart_str_free(&var);
		}
	} else {
		if (sid) {
			zend_string_release(Z_STR_P(sid));
			ZVAL_EMPTY_STRING(sid);
		} else {
			REGISTER_STRINGL_CONSTANT("SID", "", 0, 0);
		}
	}

	/* Apply trans sid if sid cookie is not set */
	if (APPLY_TRANS_SID
		&& (data = zend_hash_str_find(&EG(symbol_table), "_COOKIE", sizeof("_COOKIE") - 1))) {
		ZVAL_DEREF(data);
		if (Z_TYPE_P(data) == IS_ARRAY && (ppid = zend_hash_str_find(Z_ARRVAL_P(data), PS(session_name), strlen(PS(session_name))))) {
			ZVAL_DEREF(ppid);
		} else {
			/* FIXME: Resetting vars are required when
			   session is stop/start/regenerated. However,
			   php_url_scanner_reset_vars() resets all vars
			   including other URL rewrites set by elsewhere. */
			/* php_url_scanner_reset_vars(); */
			php_url_scanner_add_var(PS(session_name), strlen(PS(session_name)), ZSTR_VAL(PS(id)), ZSTR_LEN(PS(id)), 1);
		}
	}
}
/* }}} */

PHPAPI void php_session_start(void) /* {{{ */
{
	zval *ppid;
	zval *data;
	char *p, *value;
	size_t lensess;

	switch (PS(session_status)) {
		case php_session_active:
			php_error(E_NOTICE, "A session had already been started - ignoring session_start()");
			return;
			break;

		case php_session_disabled:
			value = zend_ini_string("session.save_handler", sizeof("session.save_handler") - 1, 0);
			if (!PS(mod) && value) {
				PS(mod) = _php_find_ps_module(value);
				if (!PS(mod)) {
					php_error_docref(NULL, E_WARNING, "Cannot find save handler '%s' - session startup failed", value);
					return;
				}
			}
			value = zend_ini_string("session.serialize_handler", sizeof("session.serialize_handler") - 1, 0);
			if (!PS(serializer) && value) {
				PS(serializer) = _php_find_ps_serializer(value);
				if (!PS(serializer)) {
					php_error_docref(NULL, E_WARNING, "Cannot find serialization handler '%s' - session startup failed", value);
					return;
				}
			}
			PS(session_status) = php_session_none;
			/* fallthrough */

		default:
		case php_session_none:
			/* Setup internal flags */
			PS(define_sid) = !PS(use_only_cookies); /* SID constant is defined when non-cookie ID is used */
			PS(send_cookie) = PS(use_cookies) || PS(use_only_cookies);
	}

	lensess = strlen(PS(session_name));

	/*
	 * Cookies are preferred, because initially cookie and get
	 * variables will be available.
	 * URL/POST session ID may be used when use_only_cookies=Off.
	 * session.use_strice_mode=On prevents session adoption.
	 * Session based file upload progress uses non-cookie ID.
	 */

	if (!PS(id)) {
		if (PS(use_cookies) && (data = zend_hash_str_find(&EG(symbol_table), "_COOKIE", sizeof("_COOKIE") - 1))) {
			ZVAL_DEREF(data);
			if (Z_TYPE_P(data) == IS_ARRAY && (ppid = zend_hash_str_find(Z_ARRVAL_P(data), PS(session_name), lensess))) {
				ppid2sid(ppid);
				PS(send_cookie) = 0;
				PS(define_sid) = 0;
			}
		}
		/* Initilize session ID from non cookie values */
		if (!PS(use_only_cookies)) {
			if (!PS(id) && (data = zend_hash_str_find(&EG(symbol_table), "_GET", sizeof("_GET") - 1))) {
				ZVAL_DEREF(data);
				if (Z_TYPE_P(data) == IS_ARRAY && (ppid = zend_hash_str_find(Z_ARRVAL_P(data), PS(session_name), lensess))) {
					ppid2sid(ppid);
				}
			}
			if (!PS(id) && (data = zend_hash_str_find(&EG(symbol_table), "_POST", sizeof("_POST") - 1))) {
				ZVAL_DEREF(data);
				if (Z_TYPE_P(data) == IS_ARRAY && (ppid = zend_hash_str_find(Z_ARRVAL_P(data), PS(session_name), lensess))) {
					ppid2sid(ppid);
				}
			}
			/* Check the REQUEST_URI symbol for a string of the form
			 * '<session-name>=<session-id>' to allow URLs of the form
			 * http://yoursite/<session-name>=<session-id>/script.php */
			if (!PS(id) && zend_is_auto_global_str("_SERVER", sizeof("_SERVER") - 1) == SUCCESS &&
				(data = zend_hash_str_find(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER]), "REQUEST_URI", sizeof("REQUEST_URI") - 1)) &&
				Z_TYPE_P(data) == IS_STRING &&
				(p = strstr(Z_STRVAL_P(data), PS(session_name))) &&
				p[lensess] == '='
				) {
				char *q;
				p += lensess + 1;
				if ((q = strpbrk(p, "/?\\"))) {
					PS(id) = zend_string_init(p, q - p, 0);
				}
			}
			/* Check whether the current request was referred to by
			 * an external site which invalidates the previously found id. */
			if (PS(id) && PS(extern_referer_chk)[0] != '\0' &&
				!Z_ISUNDEF(PG(http_globals)[TRACK_VARS_SERVER]) &&
				(data = zend_hash_str_find(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER]), "HTTP_REFERER", sizeof("HTTP_REFERER") - 1)) &&
				Z_TYPE_P(data) == IS_STRING &&
				Z_STRLEN_P(data) != 0 &&
				strstr(Z_STRVAL_P(data), PS(extern_referer_chk)) == NULL
			) {
				zend_string_release(PS(id));
				PS(id) = NULL;
			}
		}
	}

	/* Finally check session id for dangerous characters
	 * Security note: session id may be embedded in HTML pages.*/
	if (PS(id) && strpbrk(ZSTR_VAL(PS(id)), "\r\n\t <>'\"\\")) {
		zend_string_release(PS(id));
		PS(id) = NULL;
	}

	php_session_initialize();
	php_session_cache_limiter();
}
/* }}} */

static void php_session_flush(int write) /* {{{ */
{
	if (PS(session_status) == php_session_active) {
		PS(session_status) = php_session_none;
		php_session_save_current_state(write);
	}
}
/* }}} */

static void php_session_abort(void) /* {{{ */
{
	if (PS(session_status) == php_session_active) {
		PS(session_status) = php_session_none;
		if (PS(mod_data) || PS(mod_user_implemented)) {
			PS(mod)->s_close(&PS(mod_data));
		}
	}
}
/* }}} */

static void php_session_reset(void) /* {{{ */
{
	if (PS(session_status) == php_session_active) {
		php_session_initialize();
	}
}
/* }}} */


/* This API is not used by any PHP modules including session currently.
   session_adapt_url() may be used to set Session ID to target url without
   starting "URL-Rewriter" output handler. */
PHPAPI void session_adapt_url(const char *url, size_t urllen, char **new, size_t *newlen) /* {{{ */
{
	if (APPLY_TRANS_SID && (PS(session_status) == php_session_active)) {
		*new = php_url_scanner_adapt_single_url(url, urllen, PS(session_name), ZSTR_VAL(PS(id)), newlen, 1);
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
	zval *lifetime;
	zend_string *path = NULL, *domain = NULL;
	int argc = ZEND_NUM_ARGS();
	zend_bool secure = 0, httponly = 0;
	zend_string *ini_name;

	if (!PS(use_cookies) ||
		zend_parse_parameters(argc, "z|SSbb", &lifetime, &path, &domain, &secure, &httponly) == FAILURE) {
		return;
	}

	convert_to_string_ex(lifetime);

	ini_name = zend_string_init("session.cookie_lifetime", sizeof("session.cookie_lifetime") - 1, 0);
	zend_alter_ini_entry(ini_name,  Z_STR_P(lifetime), PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	zend_string_release(ini_name);

	if (path) {
		ini_name = zend_string_init("session.cookie_path", sizeof("session.cookie_path") - 1, 0);
		zend_alter_ini_entry(ini_name, path, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release(ini_name);
	}
	if (domain) {
		ini_name = zend_string_init("session.cookie_domain", sizeof("session.cookie_domain") - 1, 0);
		zend_alter_ini_entry(ini_name, domain, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release(ini_name);
	}

	if (argc > 3) {
		ini_name = zend_string_init("session.cookie_secure", sizeof("session.cookie_secure") - 1, 0);
		zend_alter_ini_entry_chars(ini_name, secure ? "1" : "0", 1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release(ini_name);
	}
	if (argc > 4) {
		ini_name = zend_string_init("session.cookie_httponly", sizeof("session.cookie_httponly") - 1, 0);
		zend_alter_ini_entry_chars(ini_name, httponly ? "1" : "0", 1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release(ini_name);
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
	add_assoc_string(return_value, "path", PS(cookie_path));
	add_assoc_string(return_value, "domain", PS(cookie_domain));
	add_assoc_bool(return_value, "secure", PS(cookie_secure));
	add_assoc_bool(return_value, "httponly", PS(cookie_httponly));
}
/* }}} */

/* {{{ proto string session_name([string newname])
   Return the current session name. If newname is given, the session name is replaced with newname */
static PHP_FUNCTION(session_name)
{
	zend_string *name = NULL;
	zend_string *ini_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &name) == FAILURE) {
		return;
	}

	RETVAL_STRING(PS(session_name));

	if (name) {
		ini_name = zend_string_init("session.name", sizeof("session.name") - 1, 0);
		zend_alter_ini_entry(ini_name, name, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release(ini_name);
	}
}
/* }}} */

/* {{{ proto string session_module_name([string newname])
   Return the current module name used for accessing session data. If newname is given, the module name is replaced with newname */
static PHP_FUNCTION(session_module_name)
{
	zend_string *name = NULL;
	zend_string *ini_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &name) == FAILURE) {
		return;
	}

	/* Set return_value to current module name */
	if (PS(mod) && PS(mod)->s_name) {
		RETVAL_STRING(PS(mod)->s_name);
	} else {
		RETVAL_EMPTY_STRING();
	}

	if (name) {
		if (!_php_find_ps_module(ZSTR_VAL(name))) {
			php_error_docref(NULL, E_WARNING, "Cannot find named PHP session module (%s)", ZSTR_VAL(name));

			zval_dtor(return_value);
			RETURN_FALSE;
		}
		if (PS(mod_data) || PS(mod_user_implemented)) {
			PS(mod)->s_close(&PS(mod_data));
		}
		PS(mod_data) = NULL;

		ini_name = zend_string_init("session.save_handler", sizeof("session.save_handler") - 1, 0);
		zend_alter_ini_entry(ini_name, name, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release(ini_name);
	}
}
/* }}} */

/* {{{ proto void session_set_save_handler(string open, string close, string read, string write, string destroy, string gc, string create_sid)
   Sets user-level functions */
static PHP_FUNCTION(session_set_save_handler)
{
	zval *args = NULL;
	int i, num_args, argc = ZEND_NUM_ARGS();
	zend_string *name;
	zend_string *ini_name, *ini_val;

	if (PS(session_status) != php_session_none) {
		RETURN_FALSE;
	}

	if (argc > 0 && argc <= 2) {
		zval *obj = NULL;
		zend_string *func_name;
		zend_function *current_mptr;
		zend_bool register_shutdown = 1;

		if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|b", &obj, php_session_iface_entry, &register_shutdown) == FAILURE) {
			RETURN_FALSE;
		}

		/* For compatibility reason, implemeted interface is not checked */
		/* Find implemented methods - SessionHandlerInterface */
		i = 0;
		ZEND_HASH_FOREACH_STR_KEY(&php_session_iface_entry->function_table, func_name) {
			if ((current_mptr = zend_hash_find_ptr(&Z_OBJCE_P(obj)->function_table, func_name))) {
				if (!Z_ISUNDEF(PS(mod_user_names).names[i])) {
					zval_ptr_dtor(&PS(mod_user_names).names[i]);
				}

				array_init_size(&PS(mod_user_names).names[i], 2);
				Z_ADDREF_P(obj);
				add_next_index_zval(&PS(mod_user_names).names[i], obj);
				add_next_index_str(&PS(mod_user_names).names[i], zend_string_copy(func_name));
			} else {
				php_error_docref(NULL, E_ERROR, "Session handler's function table is corrupt");
				RETURN_FALSE;
			}

			++i;
		} ZEND_HASH_FOREACH_END();

		/* Find implemented methods - SessionIdInterface (optional) */
		ZEND_HASH_FOREACH_STR_KEY(&php_session_id_iface_entry->function_table, func_name) {
			if ((current_mptr = zend_hash_find_ptr(&Z_OBJCE_P(obj)->function_table, func_name))) {
				if (!Z_ISUNDEF(PS(mod_user_names).names[i])) {
					zval_ptr_dtor(&PS(mod_user_names).names[i]);
				}
				array_init_size(&PS(mod_user_names).names[i], 2);
				Z_ADDREF_P(obj);
				add_next_index_zval(&PS(mod_user_names).names[i], obj);
				add_next_index_str(&PS(mod_user_names).names[i], zend_string_copy(func_name));
			} else {
				if (!Z_ISUNDEF(PS(mod_user_names).names[i])) {
					zval_ptr_dtor(&PS(mod_user_names).names[i]);
					ZVAL_UNDEF(&PS(mod_user_names).names[i]);
				}
			}

			++i;
		} ZEND_HASH_FOREACH_END();

		/* Find implemented methods - SessionUpdateTimestampInterface (optional) */
		ZEND_HASH_FOREACH_STR_KEY(&php_session_update_timestamp_iface_entry->function_table, func_name) {
			if ((current_mptr = zend_hash_find_ptr(&Z_OBJCE_P(obj)->function_table, func_name))) {
				if (!Z_ISUNDEF(PS(mod_user_names).names[i])) {
					zval_ptr_dtor(&PS(mod_user_names).names[i]);
				}
				array_init_size(&PS(mod_user_names).names[i], 2);
				Z_ADDREF_P(obj);
				add_next_index_zval(&PS(mod_user_names).names[i], obj);
				add_next_index_str(&PS(mod_user_names).names[i], zend_string_copy(func_name));
			} else {
				if (!Z_ISUNDEF(PS(mod_user_names).names[i])) {
					zval_ptr_dtor(&PS(mod_user_names).names[i]);
					ZVAL_UNDEF(&PS(mod_user_names).names[i]);
				}
			}
			++i;
		} ZEND_HASH_FOREACH_END();

		if (register_shutdown) {
			/* create shutdown function */
			php_shutdown_function_entry shutdown_function_entry;
			shutdown_function_entry.arg_count = 1;
			shutdown_function_entry.arguments = (zval *) safe_emalloc(sizeof(zval), 1, 0);

			ZVAL_STRING(&shutdown_function_entry.arguments[0], "session_register_shutdown");

			/* add shutdown function, removing the old one if it exists */
			if (!register_user_shutdown_function("session_shutdown", sizeof("session_shutdown") - 1, &shutdown_function_entry)) {
				zval_ptr_dtor(&shutdown_function_entry.arguments[0]);
				efree(shutdown_function_entry.arguments);
				php_error_docref(NULL, E_WARNING, "Unable to register session shutdown function");
				RETURN_FALSE;
			}
		} else {
			/* remove shutdown function */
			remove_user_shutdown_function("session_shutdown", sizeof("session_shutdown") - 1);
		}

		if (PS(mod) && PS(session_status) != php_session_active && PS(mod) != &ps_mod_user) {
			ini_name = zend_string_init("session.save_handler", sizeof("session.save_handler") - 1, 0);
			ini_val = zend_string_init("user", sizeof("user") - 1, 0);
			zend_alter_ini_entry(ini_name, ini_val, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
			zend_string_release(ini_val);
			zend_string_release(ini_name);
		}

		RETURN_TRUE;
	}

	/* Set procedural save handler functions */
	if (argc < 6 || PS_NUM_APIS < argc) {
		WRONG_PARAM_COUNT;
	}

	if (zend_parse_parameters(argc, "+", &args, &num_args) == FAILURE) {
		return;
	}

	/* remove shutdown function */
	remove_user_shutdown_function("session_shutdown", sizeof("session_shutdown") - 1);

	/* At this point argc can only be between 6 and PS_NUM_APIS */
	for (i = 0; i < argc; i++) {
		if (!zend_is_callable(&args[i], 0, &name)) {
			php_error_docref(NULL, E_WARNING, "Argument %d is not a valid callback", i+1);
			zend_string_release(name);
			RETURN_FALSE;
		}
		zend_string_release(name);
	}

	if (PS(mod) && PS(mod) != &ps_mod_user) {
		ini_name = zend_string_init("session.save_handler", sizeof("session.save_handler") - 1, 0);
		ini_val = zend_string_init("user", sizeof("user") - 1, 0);
		zend_alter_ini_entry(ini_name, ini_val, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release(ini_val);
		zend_string_release(ini_name);
	}

	for (i = 0; i < argc; i++) {
		if (!Z_ISUNDEF(PS(mod_user_names).names[i])) {
			zval_ptr_dtor(&PS(mod_user_names).names[i]);
		}
		ZVAL_COPY(&PS(mod_user_names).names[i], &args[i]);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string session_save_path([string newname])
   Return the current save path passed to module_name. If newname is given, the save path is replaced with newname */
static PHP_FUNCTION(session_save_path)
{
	zend_string *name = NULL;
	zend_string *ini_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &name) == FAILURE) {
		return;
	}

	RETVAL_STRING(PS(save_path));

	if (name) {
		if (memchr(ZSTR_VAL(name), '\0', ZSTR_LEN(name)) != NULL) {
			php_error_docref(NULL, E_WARNING, "The save_path cannot contain NULL characters");
			zval_dtor(return_value);
			RETURN_FALSE;
		}
		ini_name = zend_string_init("session.save_path", sizeof("session.save_path") - 1, 0);
		zend_alter_ini_entry(ini_name, name, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release(ini_name);
	}
}
/* }}} */

/* {{{ proto string session_id([string newid])
   Return the current session id. If newid is given, the session id is replaced with newid */
static PHP_FUNCTION(session_id)
{
	zend_string *name = NULL;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "|S", &name) == FAILURE) {
		return;
	}

	if (PS(id)) {
		/* keep compatibility for "\0" characters ???
		 * see: ext/session/tests/session_id_error3.phpt */
		size_t len = strlen(ZSTR_VAL(PS(id)));
		if (UNEXPECTED(len != ZSTR_LEN(PS(id)))) {
			RETVAL_NEW_STR(zend_string_init(ZSTR_VAL(PS(id)), len, 0));
		} else {
			RETVAL_STR_COPY(PS(id));
		}
	} else {
		RETVAL_EMPTY_STRING();
	}

	if (name) {
		if (PS(id)) {
			zend_string_release(PS(id));
		}
		PS(id) = zend_string_copy(name);
	}
}
/* }}} */

/* {{{ proto bool session_regenerate_id([bool delete_old_session])
   Update the current session id with a newly generated one. If delete_old_session is set to true, remove the old session. */
static PHP_FUNCTION(session_regenerate_id)
{
	zend_bool del_ses = 0;
	zend_string *data;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &del_ses) == FAILURE) {
		return;
	}

	if (SG(headers_sent) && PS(use_cookies)) {
		php_error_docref(NULL, E_WARNING, "Cannot regenerate session id - headers already sent");
		RETURN_FALSE;
	}

	if (PS(session_status) != php_session_active) {
		php_error_docref(NULL, E_WARNING, "Cannot regenerate session id - session is not active");
		RETURN_FALSE;
	}

	/* Process old session data */
	if (del_ses) {
		if (PS(mod)->s_destroy(&PS(mod_data), PS(id)) == FAILURE) {
			PS(mod)->s_close(&PS(mod_data));
			PS(session_status) = php_session_none;
			php_error_docref(NULL, E_WARNING, "Session object destruction failed.  ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
			RETURN_FALSE;
		}
	} else {
		int ret;
		data = php_session_encode();
		if (data) {
			ret = PS(mod)->s_write(&PS(mod_data), PS(id), data, PS(gc_maxlifetime));
			zend_string_release(data);
		} else {
			ret = PS(mod)->s_write(&PS(mod_data), PS(id), ZSTR_EMPTY_ALLOC(), PS(gc_maxlifetime));
		}
		if (ret == FAILURE) {
			PS(mod)->s_close(&PS(mod_data));
			PS(session_status) = php_session_none;
			php_error_docref(NULL, E_WARNING, "Session write failed. ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
			RETURN_FALSE;
		}
	}
	PS(mod)->s_close(&PS(mod_data));

	/* New session data */
	if (PS(session_vars)) {
		zend_string_release(PS(session_vars));
		PS(session_vars) = NULL;
	}
	zend_string_release(PS(id));
	PS(id) = PS(mod)->s_create_sid(&PS(mod_data));
	if (!PS(id)) {
		PS(session_status) = php_session_none;
		php_error_docref(NULL, E_RECOVERABLE_ERROR, "Failed to create new session ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		RETURN_FALSE;
	}
	if (PS(mod)->s_open(&PS(mod_data), PS(save_path), PS(session_name)) == FAILURE) {
		PS(session_status) = php_session_none;
		php_error_docref(NULL, E_RECOVERABLE_ERROR, "Failed to create(open) session ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		RETURN_FALSE;
	}
	if (PS(use_strict_mode) && PS(mod)->s_validate_sid &&
		PS(mod)->s_validate_sid(&PS(mod_data), PS(id)) == FAILURE) {
		zend_string_release(PS(id));
		PS(id) = PS(mod)->s_create_sid(&PS(mod_data));
		if (!PS(id)) {
			PS(session_status) = php_session_none;
			php_error_docref(NULL, E_RECOVERABLE_ERROR, "Failed to create session ID by collision: %s (path: %s)", PS(mod)->s_name, PS(save_path));
			RETURN_FALSE;
		}
	}
	/* Read is required to make new session data at this point. */
	if (PS(mod)->s_read(&PS(mod_data), PS(id), &data, PS(gc_maxlifetime)) == FAILURE) {
		PS(session_status) = php_session_none;
		php_error_docref(NULL, E_RECOVERABLE_ERROR, "Failed to create(read) session ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		RETURN_FALSE;
	}
	if (data) {
		zend_string_release(data);
	}

	if (PS(use_cookies)) {
		PS(send_cookie) = 1;
	}
	php_session_reset_id();

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void session_create_id([string prefix])
   Generate new session ID. Intended for user save handlers. */
#if 0
/* This is not used yet */
static PHP_FUNCTION(session_create_id)
{
	zend_string *prefix = NULL, *new_id;
	smart_str id = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &prefix) == FAILURE) {
		return;
	}

	if (prefix && ZSTR_LEN(prefix)) {
		if (php_session_valid_key(ZSTR_VAL(prefix)) == FAILURE) {
			/* E_ERROR raised for security reason. */
			php_error_docref(NULL, E_WARNING, "Prefix cannot contain special characters. Only aphanumeric, ',', '-' are allowed");
			RETURN_FALSE;
		} else {
			smart_str_append(&id, prefix);
		}
	}

	if (PS(session_status) == php_session_active) {
		new_id = PS(mod)->s_create_sid(&PS(mod_data));
	} else {
		new_id = php_session_create_id(NULL);
	}

	if (new_id) {
		smart_str_append(&id, new_id);
		zend_string_release(new_id);
	} else {
		smart_str_free(&id);
		php_error_docref(NULL, E_WARNING, "Failed to create new ID");
		RETURN_FALSE;
	}
	smart_str_0(&id);
	RETVAL_NEW_STR(id.s);
	smart_str_free(&id);
}
#endif
/* }}} */

/* {{{ proto string session_cache_limiter([string new_cache_limiter])
   Return the current cache limiter. If new_cache_limited is given, the current cache_limiter is replaced with new_cache_limiter */
static PHP_FUNCTION(session_cache_limiter)
{
	zend_string *limiter = NULL;
	zend_string *ini_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &limiter) == FAILURE) {
		return;
	}

	RETVAL_STRING(PS(cache_limiter));

	if (limiter) {
		ini_name = zend_string_init("session.cache_limiter", sizeof("session.cache_limiter") - 1, 0);
		zend_alter_ini_entry(ini_name, limiter, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release(ini_name);
	}
}
/* }}} */

/* {{{ proto int session_cache_expire([int new_cache_expire])
   Return the current cache expire. If new_cache_expire is given, the current cache_expire is replaced with new_cache_expire */
static PHP_FUNCTION(session_cache_expire)
{
	zval *expires = NULL;
	zend_string *ini_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|z", &expires) == FAILURE) {
		return;
	}

	RETVAL_LONG(PS(cache_expire));

	if (expires) {
		convert_to_string_ex(expires);
		ini_name = zend_string_init("session.cache_expire", sizeof("session.cache_expire") - 1, 0);
		zend_alter_ini_entry(ini_name, Z_STR_P(expires), ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
		zend_string_release(ini_name);
	}
}
/* }}} */

/* {{{ proto string session_encode(void)
   Serializes the current setup and returns the serialized representation */
static PHP_FUNCTION(session_encode)
{
	zend_string *enc;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	enc = php_session_encode();
	if (enc == NULL) {
		RETURN_FALSE;
	}

	RETURN_STR(enc);
}
/* }}} */

/* {{{ proto bool session_decode(string data)
   Deserializes data and reinitializes the variables */
static PHP_FUNCTION(session_decode)
{
	zend_string *str = NULL;

	if (PS(session_status) != php_session_active) {
		php_error_docref(NULL, E_WARNING, "Session is not active. You cannot decode session data");
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		return;
	}

	if (php_session_decode(str) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

static int php_session_start_set_ini(zend_string *varname, zend_string *new_value) {
	int ret;
	smart_str buf ={0};
	smart_str_appends(&buf, "session");
	smart_str_appendc(&buf, '.');
	smart_str_append(&buf, varname);
	smart_str_0(&buf);
	ret = zend_alter_ini_entry_ex(buf.s, new_value, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0);
	smart_str_free(&buf);
	return ret;
}

/* {{{ proto bool session_start([array options])
+   Begin session */
static PHP_FUNCTION(session_start)
{
	zval *options = NULL;
	zval *value;
	zend_ulong num_idx;
	zend_string *str_idx;
	zend_long read_and_close = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|a", &options) == FAILURE) {
		RETURN_FALSE;
	}

	if (PS(id) && !(ZSTR_LEN(PS(id)))) {
		php_error_docref(NULL, E_WARNING, "Cannot start session with empty session ID");
		RETURN_FALSE;
	}

	/* set options */
	if (options) {
		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(options), num_idx, str_idx, value) {
			if (str_idx) {
				switch(Z_TYPE_P(value)) {
					case IS_STRING:
					case IS_TRUE:
					case IS_FALSE:
					case IS_LONG:
						if (zend_string_equals_literal(str_idx, "read_and_close")) {
							read_and_close = zval_get_long(value);
						} else {
							zend_string *val = zval_get_string(value);
							if (php_session_start_set_ini(str_idx, val) == FAILURE) {
								php_error_docref(NULL, E_WARNING, "Setting option '%s' failed", ZSTR_VAL(str_idx));
							}
							zend_string_release(val);
						}
						break;
					default:
						php_error_docref(NULL, E_WARNING, "Option(%s) value must be string, boolean or long", ZSTR_VAL(str_idx));
						break;
				}
			}
			(void) num_idx;
		} ZEND_HASH_FOREACH_END();
	}

	php_session_start();

	if (PS(session_status) != php_session_active) {
		RETURN_FALSE;
	}

	if (read_and_close) {
		php_session_flush(0);
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

	RETURN_BOOL(php_session_destroy() == SUCCESS);
}
/* }}} */

/* {{{ proto void session_unset(void)
   Unset all registered variables */
static PHP_FUNCTION(session_unset)
{
	if (PS(session_status) != php_session_active) {
		RETURN_FALSE;
	}

	IF_SESSION_VARS() {
		zval *sess_var = Z_REFVAL(PS(http_session_vars));
		SEPARATE_ARRAY(sess_var);

		/* Clean $_SESSION. */
		zend_hash_clean(Z_ARRVAL_P(sess_var));
	}
}
/* }}} */

/* {{{ proto void session_write_close(void)
   Write session data and end session */
static PHP_FUNCTION(session_write_close)
{
	php_session_flush(1);
}
/* }}} */

/* {{{ proto void session_abort(void)
   Abort session and end session. Session data will not be written */
static PHP_FUNCTION(session_abort)
{
	php_session_abort();
}
/* }}} */

/* {{{ proto void session_reset(void)
   Reset session data from saved session data */
static PHP_FUNCTION(session_reset)
{
	php_session_reset();
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

/* {{{ proto void session_register_shutdown(void)
   Registers session_write_close() as a shutdown function */
static PHP_FUNCTION(session_register_shutdown)
{
	php_shutdown_function_entry shutdown_function_entry;

	/* This function is registered itself as a shutdown function by
	 * session_set_save_handler($obj). The reason we now register another
	 * shutdown function is in case the user registered their own shutdown
	 * function after calling session_set_save_handler(), which expects
	 * the session still to be available.
	 */

	shutdown_function_entry.arg_count = 1;
	shutdown_function_entry.arguments = (zval *) safe_emalloc(sizeof(zval), 1, 0);

	ZVAL_STRING(&shutdown_function_entry.arguments[0], "session_write_close");

	if (!append_user_shutdown_function(shutdown_function_entry)) {
		zval_ptr_dtor(&shutdown_function_entry.arguments[0]);
		efree(shutdown_function_entry.arguments);

		/* Unable to register shutdown function, presumably because of lack
		 * of memory, so flush the session now. It would be done in rshutdown
		 * anyway but the handler will have had it's dtor called by then.
		 * If the user does have a later shutdown function which needs the
		 * session then tough luck.
		 */
		php_session_flush(1);
		php_error_docref(NULL, E_WARNING, "Unable to register session flush function");
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
	ZEND_ARG_INFO(0, validate_sid)
	ZEND_ARG_INFO(0, update_timestamp)
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

ZEND_BEGIN_ARG_INFO(arginfo_session_class_validateId, 0)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_session_class_updateTimestamp, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_start, 0, 0, 0)
	ZEND_ARG_INFO(0, options) /* array */
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ session_functions[]
 */
static const zend_function_entry session_functions[] = {
	PHP_FE(session_name,              arginfo_session_name)
	PHP_FE(session_module_name,       arginfo_session_module_name)
	PHP_FE(session_save_path,         arginfo_session_save_path)
	PHP_FE(session_id,                arginfo_session_id)
	PHP_FE(session_regenerate_id,     arginfo_session_regenerate_id)
	PHP_FE(session_decode,            arginfo_session_decode)
	PHP_FE(session_encode,            arginfo_session_void)
	PHP_FE(session_start,             arginfo_session_start)
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

/* {{{ SessionUpdateTimestampHandler functions[]
 */
static const zend_function_entry php_session_update_timestamp_iface_functions[] = {
	PHP_ABSTRACT_ME(SessionUpdateTimestampHandlerInterface, validateId, arginfo_session_class_validateId)
	PHP_ABSTRACT_ME(SessionUpdateTimestampHandlerInterface, updateTimestamp, arginfo_session_class_updateTimestamp)
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

static int php_rinit_session(zend_bool auto_start) /* {{{ */
{
	php_rinit_session_globals();

	if (PS(mod) == NULL) {
		char *value;

		value = zend_ini_string("session.save_handler", sizeof("session.save_handler") - 1, 0);
		if (value) {
			PS(mod) = _php_find_ps_module(value);
		}
	}

	if (PS(serializer) == NULL) {
		char *value;

		value = zend_ini_string("session.serialize_handler", sizeof("session.serialize_handler") - 1, 0);
		if (value) {
			PS(serializer) = _php_find_ps_serializer(value);
		}
	}

	if (PS(mod) == NULL || PS(serializer) == NULL) {
		/* current status is unusable */
		PS(session_status) = php_session_disabled;
		return SUCCESS;
	}

	if (auto_start) {
		php_session_start();
	}

	return SUCCESS;
} /* }}} */

static PHP_RINIT_FUNCTION(session) /* {{{ */
{
	return php_rinit_session(PS(auto_start));
}
/* }}} */

static PHP_RSHUTDOWN_FUNCTION(session) /* {{{ */
{
	int i;

	zend_try {
		php_session_flush(1);
	} zend_end_try();
	php_rshutdown_session_globals();

	/* this should NOT be done in php_rshutdown_session_globals() */
	for (i = 0; i < PS_NUM_APIS; i++) {
		if (!Z_ISUNDEF(PS(mod_user_names).names[i])) {
			zval_ptr_dtor(&PS(mod_user_names).names[i]);
			ZVAL_UNDEF(&PS(mod_user_names).names[i]);
		}
	}

	return SUCCESS;
}
/* }}} */

static PHP_GINIT_FUNCTION(ps) /* {{{ */
{
	int i;

#if defined(COMPILE_DL_SESSION) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

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
	ps_globals->session_vars = NULL;
	for (i = 0; i < PS_NUM_APIS; i++) {
		ZVAL_UNDEF(&ps_globals->mod_user_names.names[i]);
	}
	ZVAL_UNDEF(&ps_globals->http_session_vars);
}
/* }}} */

static PHP_MINIT_FUNCTION(session) /* {{{ */
{
	zend_class_entry ce;

	zend_register_auto_global(zend_string_init("_SESSION", sizeof("_SESSION") - 1, 1), 0, NULL);

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
	php_session_iface_entry = zend_register_internal_class(&ce);
	php_session_iface_entry->ce_flags |= ZEND_ACC_INTERFACE;

	INIT_CLASS_ENTRY(ce, PS_SID_IFACE_NAME, php_session_id_iface_functions);
	php_session_id_iface_entry = zend_register_internal_class(&ce);
	php_session_id_iface_entry->ce_flags |= ZEND_ACC_INTERFACE;

	INIT_CLASS_ENTRY(ce, PS_UPDATE_TIMESTAMP_IFACE_NAME, php_session_update_timestamp_iface_functions);
	php_session_update_timestamp_iface_entry = zend_register_internal_class(&ce);
	php_session_update_timestamp_iface_entry->ce_flags |= ZEND_ACC_INTERFACE;

	/* Register base class */
	INIT_CLASS_ENTRY(ce, PS_CLASS_NAME, php_session_class_functions);
	php_session_class_entry = zend_register_internal_class(&ce);
	zend_class_implements(php_session_class_entry, 1, php_session_iface_entry);
	zend_class_implements(php_session_class_entry, 1, php_session_id_iface_entry);

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

	if (save_handlers.s) {
		smart_str_0(&save_handlers);
		php_info_print_table_row(2, "Registered save handlers", ZSTR_VAL(save_handlers.s));
		smart_str_free(&save_handlers);
	} else {
		php_info_print_table_row(2, "Registered save handlers", "none");
	}

	if (ser_handlers.s) {
		smart_str_0(&ser_handlers);
		php_info_print_table_row(2, "Registered serializer handlers", ZSTR_VAL(ser_handlers.s));
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

static zend_bool early_find_sid_in(zval *dest, int where, php_session_rfc1867_progress *progress) /* {{{ */
{
	zval *ppid;

	if (Z_ISUNDEF(PG(http_globals)[where])) {
		return 0;
	}

	if ((ppid = zend_hash_str_find(Z_ARRVAL(PG(http_globals)[where]), PS(session_name), progress->sname_len))
			&& Z_TYPE_P(ppid) == IS_STRING) {
		zval_dtor(dest);
		ZVAL_DEREF(ppid);
		ZVAL_COPY(dest, ppid);
		return 1;
	}

	return 0;
} /* }}} */

static void php_session_rfc1867_early_find_sid(php_session_rfc1867_progress *progress) /* {{{ */
{

	if (PS(use_cookies)) {
		sapi_module.treat_data(PARSE_COOKIE, NULL, NULL);
		if (early_find_sid_in(&progress->sid, TRACK_VARS_COOKIE, progress)) {
			progress->apply_trans_sid = 0;
			return;
		}
	}
	if (PS(use_only_cookies)) {
		return;
	}
	sapi_module.treat_data(PARSE_GET, NULL, NULL);
	early_find_sid_in(&progress->sid, TRACK_VARS_GET, progress);
} /* }}} */

static zend_bool php_check_cancel_upload(php_session_rfc1867_progress *progress) /* {{{ */
{
	zval *progress_ary, *cancel_upload;

	if ((progress_ary = zend_symtable_find(Z_ARRVAL_P(Z_REFVAL(PS(http_session_vars))), progress->key.s)) == NULL) {
		return 0;
	}
	if (Z_TYPE_P(progress_ary) != IS_ARRAY) {
		return 0;
	}
	if ((cancel_upload = zend_hash_str_find(Z_ARRVAL_P(progress_ary), "cancel_upload", sizeof("cancel_upload") - 1)) == NULL) {
		return 0;
	}
	return Z_TYPE_P(cancel_upload) == IS_TRUE;
} /* }}} */

static void php_session_rfc1867_update(php_session_rfc1867_progress *progress, int force_update) /* {{{ */
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

	php_session_initialize();
	PS(session_status) = php_session_active;
	IF_SESSION_VARS() {
		zval *sess_var = Z_REFVAL(PS(http_session_vars));
		SEPARATE_ARRAY(sess_var);

		progress->cancel_upload |= php_check_cancel_upload(progress);
		Z_TRY_ADDREF(progress->data);
		zend_hash_update(Z_ARRVAL_P(sess_var), progress->key.s, &progress->data);
	}
	php_session_flush(1);
} /* }}} */

static void php_session_rfc1867_cleanup(php_session_rfc1867_progress *progress) /* {{{ */
{
	php_session_initialize();
	PS(session_status) = php_session_active;
	IF_SESSION_VARS() {
		zval *sess_var = Z_REFVAL(PS(http_session_vars));
		SEPARATE_ARRAY(sess_var);
		zend_hash_del(Z_ARRVAL_P(sess_var), progress->key.s);
	}
	php_session_flush(1);
} /* }}} */

static int php_session_rfc1867_callback(unsigned int event, void *event_data, void **extra) /* {{{ */
{
	php_session_rfc1867_progress *progress;
	int retval = SUCCESS;

	if (php_session_rfc1867_orig_callback) {
		retval = php_session_rfc1867_orig_callback(event, event_data, extra);
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

			if (Z_TYPE(progress->sid) && progress->key.s) {
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
					ZVAL_STRINGL(&progress->sid, (*data->value), value_len);
				} else if (name_len == strlen(PS(rfc1867_name)) && memcmp(data->name, PS(rfc1867_name), name_len + 1) == 0) {
					smart_str_free(&progress->key);
					smart_str_appends(&progress->key, PS(rfc1867_prefix));
					smart_str_appendl(&progress->key, *data->value, value_len);
					smart_str_0(&progress->key);

					progress->apply_trans_sid = APPLY_TRANS_SID;
					php_session_rfc1867_early_find_sid(progress);
				}
			}
		}
		break;
		case MULTIPART_EVENT_FILE_START: {
			multipart_event_file_start *data = (multipart_event_file_start *) event_data;

			/* Do nothing when $_POST["PHP_SESSION_UPLOAD_PROGRESS"] is not set
			 * or when we have no session id */
			if (!Z_TYPE(progress->sid) || !progress->key.s) {
				break;
			}

			/* First FILE_START event, initializing data */
			if (Z_ISUNDEF(progress->data)) {

				if (PS(rfc1867_freq) >= 0) {
					progress->update_step = PS(rfc1867_freq);
				} else if (PS(rfc1867_freq) < 0) { /* % of total size */
					progress->update_step = progress->content_length * -PS(rfc1867_freq) / 100;
				}
				progress->next_update = 0;
				progress->next_update_time = 0.0;

				array_init(&progress->data);
				array_init(&progress->files);

				add_assoc_long_ex(&progress->data, "start_time", sizeof("start_time") - 1, (zend_long)sapi_get_request_time());
				add_assoc_long_ex(&progress->data, "content_length",  sizeof("content_length") - 1, progress->content_length);
				add_assoc_long_ex(&progress->data, "bytes_processed", sizeof("bytes_processed") - 1, data->post_bytes_processed);
				add_assoc_bool_ex(&progress->data, "done", sizeof("done") - 1, 0);
				add_assoc_zval_ex(&progress->data, "files", sizeof("files") - 1, &progress->files);

				progress->post_bytes_processed = zend_hash_str_find(Z_ARRVAL(progress->data), "bytes_processed", sizeof("bytes_processed") - 1);

				php_rinit_session(0);
				PS(id) = zend_string_init(Z_STRVAL(progress->sid), Z_STRLEN(progress->sid), 0);
				if (progress->apply_trans_sid) {
					/* Enable trans sid by modifying flags */
					PS(use_trans_sid) = 1;
					PS(use_only_cookies) = 0;
				}
				PS(send_cookie) = 0;
			}

			array_init(&progress->current_file);

			/* Each uploaded file has its own array. Trying to make it close to $_FILES entries. */
			add_assoc_string_ex(&progress->current_file, "field_name", sizeof("field_name") - 1, data->name);
			add_assoc_string_ex(&progress->current_file, "name", sizeof("name") - 1, *data->filename);
			add_assoc_null_ex(&progress->current_file, "tmp_name", sizeof("tmp_name") - 1);
			add_assoc_long_ex(&progress->current_file, "error", sizeof("error") - 1, 0);

			add_assoc_bool_ex(&progress->current_file, "done", sizeof("done") - 1, 0);
			add_assoc_long_ex(&progress->current_file, "start_time", sizeof("start_time") - 1, (zend_long)time(NULL));
			add_assoc_long_ex(&progress->current_file, "bytes_processed", sizeof("bytes_processed") - 1, 0);

			add_next_index_zval(&progress->files, &progress->current_file);

			progress->current_file_bytes_processed = zend_hash_str_find(Z_ARRVAL(progress->current_file), "bytes_processed", sizeof("bytes_processed") - 1);

			Z_LVAL_P(progress->current_file_bytes_processed) =  data->post_bytes_processed;
			php_session_rfc1867_update(progress, 0);
		}
		break;
		case MULTIPART_EVENT_FILE_DATA: {
			multipart_event_file_data *data = (multipart_event_file_data *) event_data;

			if (!Z_TYPE(progress->sid) || !progress->key.s) {
				break;
			}

			Z_LVAL_P(progress->current_file_bytes_processed) = data->offset + data->length;
			Z_LVAL_P(progress->post_bytes_processed) = data->post_bytes_processed;

			php_session_rfc1867_update(progress, 0);
		}
		break;
		case MULTIPART_EVENT_FILE_END: {
			multipart_event_file_end *data = (multipart_event_file_end *) event_data;

			if (!Z_TYPE(progress->sid) || !progress->key.s) {
				break;
			}

			if (data->temp_filename) {
				add_assoc_string_ex(&progress->current_file, "tmp_name",  sizeof("tmp_name") - 1, data->temp_filename);
			}

			add_assoc_long_ex(&progress->current_file, "error", sizeof("error") - 1, data->cancel_upload);
			add_assoc_bool_ex(&progress->current_file, "done", sizeof("done") - 1,  1);

			Z_LVAL_P(progress->post_bytes_processed) = data->post_bytes_processed;

			php_session_rfc1867_update(progress, 0);
		}
		break;
		case MULTIPART_EVENT_END: {
			multipart_event_end *data = (multipart_event_end *) event_data;

			if (Z_TYPE(progress->sid) && progress->key.s) {
				if (PS(rfc1867_cleanup)) {
					php_session_rfc1867_cleanup(progress);
				} else {
					add_assoc_bool_ex(&progress->data, "done", sizeof("done") - 1, 1);
					Z_LVAL_P(progress->post_bytes_processed) = data->post_bytes_processed;
					php_session_rfc1867_update(progress, 1);
				}
				php_rshutdown_session_globals();
			}

			if (!Z_ISUNDEF(progress->data)) {
				zval_ptr_dtor(&progress->data);
			}
			zval_ptr_dtor(&progress->sid);
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
	PHP_SESSION_VERSION,
	PHP_MODULE_GLOBALS(ps),
	PHP_GINIT(ps),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_SESSION
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
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
