/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Andrei Zmievski <andrei@php.net>                            |
   +----------------------------------------------------------------------+
 */

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
#include "session_arginfo.h"
#include "ext/standard/php_var.h"
#include "ext/date/php_date.h"
#include "ext/standard/url_scanner_ex.h"
#include "ext/standard/info.h"
#include "zend_smart_str.h"
#include "ext/standard/url.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/head.h"
#include "ext/random/php_random.h"

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

/* SessionUpdateTimestampInterface */
zend_class_entry *php_session_update_timestamp_iface_entry;

#define PS_MAX_SID_LENGTH 256

/* ***********
   * Helpers *
   *********** */

#define IF_SESSION_VARS() \
	if (Z_ISREF_P(&PS(http_session_vars)) && Z_TYPE_P(Z_REFVAL(PS(http_session_vars))) == IS_ARRAY)

#define SESSION_CHECK_ACTIVE_STATE	\
	if (PS(session_status) == php_session_active) {	\
		php_error_docref(NULL, E_WARNING, "Session ini settings cannot be changed when a session is active");	\
		return FAILURE;	\
	}

#define SESSION_CHECK_OUTPUT_STATE										\
	if (SG(headers_sent) && stage != ZEND_INI_STAGE_DEACTIVATE) {												\
		php_error_docref(NULL, E_WARNING, "Session ini settings cannot be changed after headers have already been sent");	\
		return FAILURE;													\
	}

#define SESSION_FORBIDDEN_CHARS "=,;.[ \t\r\n\013\014"

#define APPLY_TRANS_SID (PS(use_trans_sid) && !PS(use_only_cookies))

static int php_session_send_cookie(void);
static int php_session_abort(void);

/* Initialized in MINIT, readonly otherwise. */
static int my_module_number = 0;

/* Dispatched by RINIT and by php_session_destroy */
static inline void php_rinit_session_globals(void) /* {{{ */
{
	/* Do NOT init PS(mod_user_names) here! */
	/* TODO: These could be moved to MINIT and removed. These should be initialized by php_rshutdown_session_globals() always when execution is finished. */
	PS(id) = NULL;
	PS(session_status) = php_session_none;
	PS(in_save_handler) = 0;
	PS(set_handler) = 0;
	PS(mod_data) = NULL;
	PS(mod_user_is_open) = 0;
	PS(define_sid) = 1;
	PS(session_vars) = NULL;
	PS(module_number) = my_module_number;
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
		zend_string_release_ex(PS(id), 0);
		PS(id) = NULL;
	}

	if (PS(session_vars)) {
		zend_string_release_ex(PS(session_vars), 0);
		PS(session_vars) = NULL;
	}

	if (PS(mod_user_class_name)) {
		zend_string_release(PS(mod_user_class_name));
		PS(mod_user_class_name) = NULL;
	}

	/* User save handlers may end up directly here by misuse, bugs in user script, etc. */
	/* Set session status to prevent error while restoring save handler INI value. */
	PS(session_status) = php_session_none;
}
/* }}} */

PHPAPI zend_result php_session_destroy(void) /* {{{ */
{
	zend_result retval = SUCCESS;

	if (PS(session_status) != php_session_active) {
		php_error_docref(NULL, E_WARNING, "Trying to destroy uninitialized session");
		return FAILURE;
	}

	if (PS(id) && PS(mod)->s_destroy(&PS(mod_data), PS(id)) == FAILURE) {
		retval = FAILURE;
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Session object destruction failed");
		}
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
	zend_string_release_ex(var_name, 0);
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

static ZEND_COLD void php_session_cancel_decode(void)
{
	php_session_destroy();
	php_session_track_init();
	php_error_docref(NULL, E_WARNING, "Failed to decode session object. Session has been destroyed");
}

static zend_result php_session_decode(zend_string *data) /* {{{ */
{
	if (!PS(serializer)) {
		php_error_docref(NULL, E_WARNING, "Unknown session.serialize_handler. Failed to decode session object");
		return FAILURE;
	}
	zend_result result = SUCCESS;
	zend_try {
		if (PS(serializer)->decode(ZSTR_VAL(data), ZSTR_LEN(data)) == FAILURE) {
			php_session_cancel_decode();
			result = FAILURE;
		}
	} zend_catch {
		php_session_cancel_decode();
		zend_bailout();
	} zend_end_try();
	return result;
}
/* }}} */

/*
 * Note that we cannot use the BASE64 alphabet here, because
 * it contains "/" and "+": both are unacceptable for simple inclusion
 * into URLs.
 */

static const char hexconvtab[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,-";

static void bin_to_readable(unsigned char *in, size_t inlen, char *out, size_t outlen, char nbits) /* {{{ */
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

	while (outlen--) {
		if (have < nbits) {
			if (p < q) {
				w |= *p++ << have;
				have += 8;
			} else {
				/* Should never happen. Input must be large enough. */
				ZEND_UNREACHABLE();
				break;
			}
		}

		/* consume nbits */
		*out++ = hexconvtab[w & mask];
		w >>= nbits;
		have -= nbits;
	}

	*out = '\0';
}
/* }}} */

#define PS_EXTRA_RAND_BYTES 60

PHPAPI zend_string *php_session_create_id(PS_CREATE_SID_ARGS) /* {{{ */
{
	unsigned char rbuf[PS_MAX_SID_LENGTH + PS_EXTRA_RAND_BYTES];
	zend_string *outid;

	/* It would be enough to read ceil(sid_length * sid_bits_per_character / 8) bytes here.
	 * We read sid_length bytes instead for simplicity. */
	/* Read additional PS_EXTRA_RAND_BYTES just in case CSPRNG is not safe enough */
	if (php_random_bytes_throw(rbuf, PS(sid_length) + PS_EXTRA_RAND_BYTES) == FAILURE) {
		return NULL;
	}

	outid = zend_string_alloc(PS(sid_length), 0);
	bin_to_readable(
		rbuf, PS(sid_length),
		ZSTR_VAL(outid), ZSTR_LEN(outid),
		(char)PS(sid_bits_per_character));

	return outid;
}
/* }}} */

/* Default session id char validation function allowed by ps_modules.
 * If you change the logic here, please also update the error message in
 * ps_modules appropriately */
PHPAPI zend_result php_session_valid_key(const char *key) /* {{{ */
{
	size_t len;
	const char *p;
	char c;
	zend_result ret = SUCCESS;

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
	if (len == 0 || len > PS_MAX_SID_LENGTH) {
		ret = FAILURE;
	}

	return ret;
}
/* }}} */


static zend_long php_session_gc(bool immediate) /* {{{ */
{
	int nrand;
	zend_long num = -1;

	/* GC must be done before reading session data. */
	if ((PS(mod_data) || PS(mod_user_implemented))) {
		if (immediate) {
			PS(mod)->s_gc(&PS(mod_data), PS(gc_maxlifetime), &num);
			return num;
		}
		nrand = (zend_long) ((float) PS(gc_divisor) * php_combined_lcg());
		if (PS(gc_probability) > 0 && nrand < PS(gc_probability)) {
			PS(mod)->s_gc(&PS(mod_data), PS(gc_maxlifetime), &num);
		}
	}
	return num;
} /* }}} */

static zend_result php_session_initialize(void) /* {{{ */
{
	zend_string *val = NULL;

	PS(session_status) = php_session_active;

	if (!PS(mod)) {
		PS(session_status) = php_session_disabled;
		php_error_docref(NULL, E_WARNING, "No storage module chosen - failed to initialize session");
		return FAILURE;
	}

	/* Open session handler first */
	if (PS(mod)->s_open(&PS(mod_data), PS(save_path), PS(session_name)) == FAILURE
		/* || PS(mod_data) == NULL */ /* FIXME: open must set valid PS(mod_data) with success */
	) {
		php_session_abort();
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Failed to initialize storage module: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		}
		return FAILURE;
	}

	/* If there is no ID, use session module to create one */
	if (!PS(id) || !ZSTR_VAL(PS(id))[0]) {
		if (PS(id)) {
			zend_string_release_ex(PS(id), 0);
		}
		PS(id) = PS(mod)->s_create_sid(&PS(mod_data));
		if (!PS(id)) {
			php_session_abort();
			if (!EG(exception)) {
				zend_throw_error(NULL, "Failed to create session ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
			}
			return FAILURE;
		}
		if (PS(use_cookies)) {
			PS(send_cookie) = 1;
		}
	} else if (PS(use_strict_mode) && PS(mod)->s_validate_sid &&
		PS(mod)->s_validate_sid(&PS(mod_data), PS(id)) == FAILURE
	) {
		if (PS(id)) {
			zend_string_release_ex(PS(id), 0);
		}
		PS(id) = PS(mod)->s_create_sid(&PS(mod_data));
		if (!PS(id)) {
			PS(id) = php_session_create_id(NULL);
		}
		if (PS(use_cookies)) {
			PS(send_cookie) = 1;
		}
	}

	if (php_session_reset_id() == FAILURE) {
		php_session_abort();
		return FAILURE;
	}

	/* Read data */
	php_session_track_init();
	if (PS(mod)->s_read(&PS(mod_data), PS(id), &val, PS(gc_maxlifetime)) == FAILURE) {
		php_session_abort();
		/* FYI: Some broken save handlers return FAILURE for non-existent session ID, this is incorrect */
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Failed to read session data: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		}
		return FAILURE;
	}

	/* GC must be done after read */
	php_session_gc(0);

	if (PS(session_vars)) {
		zend_string_release_ex(PS(session_vars), 0);
		PS(session_vars) = NULL;
	}
	if (val) {
		if (PS(lazy_write)) {
			PS(session_vars) = zend_string_copy(val);
		}
		php_session_decode(val);
		zend_string_release_ex(val, 0);
	}
	return SUCCESS;
}
/* }}} */

static void php_session_save_current_state(int write) /* {{{ */
{
	zend_result ret = FAILURE;

	if (write) {
		IF_SESSION_VARS() {
			zend_string *handler_class_name = PS(mod_user_class_name);
			const char *handler_function_name;

			if (PS(mod_data) || PS(mod_user_implemented)) {
				zend_string *val;

				val = php_session_encode();
				if (val) {
					if (PS(lazy_write) && PS(session_vars)
						&& PS(mod)->s_update_timestamp
						&& PS(mod)->s_update_timestamp != php_session_update_timestamp
						&& zend_string_equals(val, PS(session_vars))
					) {
						ret = PS(mod)->s_update_timestamp(&PS(mod_data), PS(id), val, PS(gc_maxlifetime));
						handler_function_name = handler_class_name != NULL ? "updateTimestamp" : "update_timestamp";
					} else {
						ret = PS(mod)->s_write(&PS(mod_data), PS(id), val, PS(gc_maxlifetime));
						handler_function_name = "write";
					}
					zend_string_release_ex(val, 0);
				} else {
					ret = PS(mod)->s_write(&PS(mod_data), PS(id), ZSTR_EMPTY_ALLOC(), PS(gc_maxlifetime));
					handler_function_name = "write";
				}
			}

			if ((ret == FAILURE) && !EG(exception)) {
				if (!PS(mod_user_implemented)) {
					php_error_docref(NULL, E_WARNING, "Failed to write session data (%s). Please "
									 "verify that the current setting of session.save_path "
									 "is correct (%s)",
									 PS(mod)->s_name,
									 PS(save_path));
				} else if (handler_class_name != NULL) {
					php_error_docref(NULL, E_WARNING, "Failed to write session data using user "
									 "defined save handler. (session.save_path: %s, handler: %s::%s)", PS(save_path),
									 ZSTR_VAL(handler_class_name), handler_function_name);
				} else {
					php_error_docref(NULL, E_WARNING, "Failed to write session data using user "
									 "defined save handler. (session.save_path: %s, handler: %s)", PS(save_path),
									 handler_function_name);
				}
			}
		}
	}

	if (PS(mod_data) || PS(mod_user_implemented)) {
		PS(mod)->s_close(&PS(mod_data));
	}
}
/* }}} */

static void php_session_normalize_vars(void) /* {{{ */
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
	const ps_module *tmp;
	int err_type = E_ERROR;

	SESSION_CHECK_ACTIVE_STATE;
	SESSION_CHECK_OUTPUT_STATE;

	tmp = _php_find_ps_module(ZSTR_VAL(new_value));

	if (stage == ZEND_INI_STAGE_RUNTIME) {
		err_type = E_WARNING;
	}

	if (PG(modules_activated) && !tmp) {
		/* Do not output error when restoring ini options. */
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			php_error_docref(NULL, err_type, "Session save handler \"%s\" cannot be found", ZSTR_VAL(new_value));
		}

		return FAILURE;
	}

	/* "user" save handler should not be set by user */
	if (!PS(set_handler) &&  tmp == ps_user_ptr) {
		php_error_docref(NULL, err_type, "Session save handler \"user\" cannot be set by ini_set()");
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
	SESSION_CHECK_OUTPUT_STATE;

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
			php_error_docref(NULL, err_type, "Serialization handler \"%s\" cannot be found", ZSTR_VAL(new_value));
		}
		return FAILURE;
	}
	PS(serializer) = tmp;

	return SUCCESS;
}
/* }}} */

static PHP_INI_MH(OnUpdateSaveDir) /* {{{ */
{
	SESSION_CHECK_ACTIVE_STATE;
	SESSION_CHECK_OUTPUT_STATE;

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

	return OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}
/* }}} */


static PHP_INI_MH(OnUpdateName) /* {{{ */
{
	SESSION_CHECK_ACTIVE_STATE;
	SESSION_CHECK_OUTPUT_STATE;

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
			php_error_docref(NULL, err_type, "session.name \"%s\" cannot be numeric or empty", ZSTR_VAL(new_value));
		}
		return FAILURE;
	}

	return OnUpdateStringUnempty(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}
/* }}} */


static PHP_INI_MH(OnUpdateCookieLifetime) /* {{{ */
{
	SESSION_CHECK_ACTIVE_STATE;
	SESSION_CHECK_OUTPUT_STATE;
	if (atol(ZSTR_VAL(new_value)) < 0) {
		php_error_docref(NULL, E_WARNING, "CookieLifetime cannot be negative");
		return FAILURE;
	}
	return OnUpdateLongGEZero(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}
/* }}} */


static PHP_INI_MH(OnUpdateSessionLong) /* {{{ */
{
	SESSION_CHECK_ACTIVE_STATE;
	SESSION_CHECK_OUTPUT_STATE;
	return OnUpdateLong(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}
/* }}} */


static PHP_INI_MH(OnUpdateSessionString) /* {{{ */
{
	SESSION_CHECK_ACTIVE_STATE;
	SESSION_CHECK_OUTPUT_STATE;
	return OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}
/* }}} */


static PHP_INI_MH(OnUpdateSessionBool) /* {{{ */
{
	SESSION_CHECK_ACTIVE_STATE;
	SESSION_CHECK_OUTPUT_STATE;
	return OnUpdateBool(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}
/* }}} */


static PHP_INI_MH(OnUpdateSidLength) /* {{{ */
{
	zend_long val;
	char *endptr = NULL;

	SESSION_CHECK_ACTIVE_STATE;
	SESSION_CHECK_OUTPUT_STATE;
	val = ZEND_STRTOL(ZSTR_VAL(new_value), &endptr, 10);
	if (endptr && (*endptr == '\0')
		&& val >= 22 && val <= PS_MAX_SID_LENGTH) {
		/* Numeric value */
		PS(sid_length) = val;
		return SUCCESS;
	}

	php_error_docref(NULL, E_WARNING, "session.configuration \"session.sid_length\" must be between 22 and 256");
	return FAILURE;
}
/* }}} */

static PHP_INI_MH(OnUpdateSidBits) /* {{{ */
{
	zend_long val;
	char *endptr = NULL;

	SESSION_CHECK_ACTIVE_STATE;
	SESSION_CHECK_OUTPUT_STATE;
	val = ZEND_STRTOL(ZSTR_VAL(new_value), &endptr, 10);
	if (endptr && (*endptr == '\0')
		&& val >= 4 && val <=6) {
		/* Numeric value */
		PS(sid_bits_per_character) = val;
		return SUCCESS;
	}

	php_error_docref(NULL, E_WARNING, "session.configuration \"session.sid_bits_per_character\" must be between 4 and 6");
	return FAILURE;
}
/* }}} */

static PHP_INI_MH(OnUpdateRfc1867Freq) /* {{{ */
{
	int tmp = ZEND_ATOL(ZSTR_VAL(new_value));
	if(tmp < 0) {
		php_error_docref(NULL, E_WARNING, "session.upload_progress.freq must be greater than or equal to 0");
		return FAILURE;
	}
	if(ZSTR_LEN(new_value) > 0 && ZSTR_VAL(new_value)[ZSTR_LEN(new_value)-1] == '%') {
		if(tmp > 100) {
			php_error_docref(NULL, E_WARNING, "session.upload_progress.freq must be less than or equal to 100%%");
			return FAILURE;
		}
		PS(rfc1867_freq) = -tmp;
	} else {
		PS(rfc1867_freq) = tmp;
	}
	return SUCCESS;
} /* }}} */

/* {{{ PHP_INI */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("session.save_path",          "",          PHP_INI_ALL, OnUpdateSaveDir,       save_path,          php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.name",               "PHPSESSID", PHP_INI_ALL, OnUpdateName,          session_name,       php_ps_globals,    ps_globals)
	PHP_INI_ENTRY("session.save_handler",           "files",     PHP_INI_ALL, OnUpdateSaveHandler)
	STD_PHP_INI_BOOLEAN("session.auto_start",       "0",         PHP_INI_PERDIR, OnUpdateBool,       auto_start,         php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.gc_probability",     "1",         PHP_INI_ALL, OnUpdateSessionLong,          gc_probability,     php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.gc_divisor",         "100",       PHP_INI_ALL, OnUpdateSessionLong,          gc_divisor,         php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.gc_maxlifetime",     "1440",      PHP_INI_ALL, OnUpdateSessionLong,          gc_maxlifetime,     php_ps_globals,    ps_globals)
	PHP_INI_ENTRY("session.serialize_handler",      "php",       PHP_INI_ALL, OnUpdateSerializer)
	STD_PHP_INI_ENTRY("session.cookie_lifetime",    "0",         PHP_INI_ALL, OnUpdateCookieLifetime,cookie_lifetime,    php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.cookie_path",        "/",         PHP_INI_ALL, OnUpdateSessionString, cookie_path,        php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.cookie_domain",      "",          PHP_INI_ALL, OnUpdateSessionString, cookie_domain,      php_ps_globals,    ps_globals)
	STD_PHP_INI_BOOLEAN("session.cookie_secure",    "0",         PHP_INI_ALL, OnUpdateSessionBool,   cookie_secure,      php_ps_globals,    ps_globals)
	STD_PHP_INI_BOOLEAN("session.cookie_httponly",  "0",         PHP_INI_ALL, OnUpdateSessionBool,   cookie_httponly,    php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.cookie_samesite",    "",          PHP_INI_ALL, OnUpdateSessionString, cookie_samesite,    php_ps_globals,    ps_globals)
	STD_PHP_INI_BOOLEAN("session.use_cookies",      "1",         PHP_INI_ALL, OnUpdateSessionBool,   use_cookies,        php_ps_globals,    ps_globals)
	STD_PHP_INI_BOOLEAN("session.use_only_cookies", "1",         PHP_INI_ALL, OnUpdateSessionBool,   use_only_cookies,   php_ps_globals,    ps_globals)
	STD_PHP_INI_BOOLEAN("session.use_strict_mode",  "0",         PHP_INI_ALL, OnUpdateSessionBool,   use_strict_mode,    php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.referer_check",      "",          PHP_INI_ALL, OnUpdateSessionString, extern_referer_chk, php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.cache_limiter",      "nocache",   PHP_INI_ALL, OnUpdateSessionString, cache_limiter,      php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.cache_expire",       "180",       PHP_INI_ALL, OnUpdateSessionLong,   cache_expire,       php_ps_globals,    ps_globals)
	STD_PHP_INI_BOOLEAN("session.use_trans_sid",    "0",         PHP_INI_ALL, OnUpdateSessionBool,   use_trans_sid,      php_ps_globals,    ps_globals)
	PHP_INI_ENTRY("session.sid_length",             "32",        PHP_INI_ALL, OnUpdateSidLength)
	PHP_INI_ENTRY("session.sid_bits_per_character", "4",         PHP_INI_ALL, OnUpdateSidBits)
	STD_PHP_INI_BOOLEAN("session.lazy_write",       "1",         PHP_INI_ALL, OnUpdateSessionBool,    lazy_write,         php_ps_globals,    ps_globals)

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
	bool result;
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
	zend_string_release_ex(var_name, 0);
	return result || !vallen ? SUCCESS : FAILURE;
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
	zend_string *name;
	php_unserialize_data_t var_hash;
	zval *current, rv;

	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	for (p = val; p < endptr; ) {
		// Can this be changed to size_t?
		int namelen = ((unsigned char)(*p)) & (~PS_BIN_UNDEF);

		if (namelen < 0 || namelen > PS_BIN_MAX || (p + namelen) >= endptr) {
			PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
			return FAILURE;
		}

		name = zend_string_init(p + 1, namelen, 0);
		p += namelen + 1;
		current = var_tmp_var(&var_hash);

		if (php_var_unserialize(current, (const unsigned char **) &p, (const unsigned char *) endptr, &var_hash)) {
			ZVAL_PTR(&rv, current);
			php_set_session_var(name, &rv, &var_hash);
		} else {
			zend_string_release_ex(name, 0);
			php_session_normalize_vars();
			PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
			return FAILURE;
		}
		zend_string_release_ex(name, 0);
	}

	php_session_normalize_vars();
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);

	return SUCCESS;
}
/* }}} */

#define PS_DELIMITER '|'

PS_SERIALIZER_ENCODE_FUNC(php) /* {{{ */
{
	smart_str buf = {0};
	php_serialize_data_t var_hash;
	PS_ENCODE_VARS;

	PHP_VAR_SERIALIZE_INIT(var_hash);

	PS_ENCODE_LOOP(
		smart_str_appendl(&buf, ZSTR_VAL(key), ZSTR_LEN(key));
		if (memchr(ZSTR_VAL(key), PS_DELIMITER, ZSTR_LEN(key))) {
			PHP_VAR_SERIALIZE_DESTROY(var_hash);
			smart_str_free(&buf);
			return NULL;
		}
		smart_str_appendc(&buf, PS_DELIMITER);
		php_var_serialize(&buf, struc, &var_hash);
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
	zend_result retval = SUCCESS;
	php_unserialize_data_t var_hash;
	zval *current, rv;

	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	p = val;

	while (p < endptr) {
		q = p;
		while (*q != PS_DELIMITER) {
			if (++q >= endptr) {
				retval = FAILURE;
				goto break_outer_loop;
			}
		}

		namelen = q - p;
		name = zend_string_init(p, namelen, 0);
		q++;

		current = var_tmp_var(&var_hash);
		if (php_var_unserialize(current, (const unsigned char **)&q, (const unsigned char *)endptr, &var_hash)) {
			ZVAL_PTR(&rv, current);
			php_set_session_var(name, &rv, &var_hash);
		} else {
			zend_string_release_ex(name, 0);
			retval = FAILURE;
			goto break_outer_loop;
		}
		zend_string_release_ex(name, 0);
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

PHPAPI zend_result php_session_register_serializer(const char *name, zend_string *(*encode)(PS_SERIALIZER_ENCODE_ARGS), zend_result (*decode)(PS_SERIALIZER_DECODE_ARGS)) /* {{{ */
{
	zend_result ret = FAILURE;

	for (int i = 0; i < MAX_SERIALIZERS; i++) {
		if (ps_serializers[i].name == NULL) {
			ps_serializers[i].name = name;
			ps_serializers[i].encode = encode;
			ps_serializers[i].decode = decode;
			ps_serializers[i + 1].name = NULL;
			ret = SUCCESS;
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

static const ps_module *ps_modules[MAX_MODULES + 1] = {
	ps_files_ptr,
	ps_user_ptr
};

PHPAPI zend_result php_session_register_module(const ps_module *ptr) /* {{{ */
{
	int ret = FAILURE;

	for (int i = 0; i < MAX_MODULES; i++) {
		if (!ps_modules[i]) {
			ps_modules[i] = ptr;
			ret = SUCCESS;
			break;
		}
	}
	return ret;
}
/* }}} */

/* Dummy PS module function */
/* We consider any ID valid (thus also implying that a session with such an ID exists),
	thus we always return SUCCESS */
PHPAPI zend_result php_session_validate_sid(PS_VALIDATE_SID_ARGS) {
	return SUCCESS;
}

/* Dummy PS module function */
PHPAPI zend_result php_session_update_timestamp(PS_UPDATE_TIMESTAMP_ARGS) {
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

static const char *month_names[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const char *week_days[] = {
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
	zend_stat_t sb = {0};
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

static const php_session_cache_limiter_t php_session_cache_limiters[] = {
	CACHE_LIMITER_ENTRY(public)
	CACHE_LIMITER_ENTRY(private)
	CACHE_LIMITER_ENTRY(private_no_expire)
	CACHE_LIMITER_ENTRY(nocache)
	{0}
};

static int php_session_cache_limiter(void) /* {{{ */
{
	const php_session_cache_limiter_t *lim;

	if (PS(cache_limiter)[0] == '\0') return 0;
	if (PS(session_status) != php_session_active) return -1;

	if (SG(headers_sent)) {
		const char *output_start_filename = php_output_get_start_filename();
		int output_start_lineno = php_output_get_start_lineno();

		php_session_abort();
		if (output_start_filename) {
			php_error_docref(NULL, E_WARNING, "Session cache limiter cannot be sent after headers have already been sent (output started at %s:%d)", output_start_filename, output_start_lineno);
		} else {
			php_error_docref(NULL, E_WARNING, "Session cache limiter cannot be sent after headers have already been sent");
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
	size_t session_cookie_len;
	size_t len = sizeof("Set-Cookie")-1;

	ZEND_ASSERT(strpbrk(PS(session_name), SESSION_FORBIDDEN_CHARS) == NULL);
	spprintf(&session_cookie, 0, "Set-Cookie: %s=", PS(session_name));

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

static zend_result php_session_send_cookie(void) /* {{{ */
{
	smart_str ncookie = {0};
	zend_string *date_fmt = NULL;
	zend_string *e_id;

	if (SG(headers_sent)) {
		const char *output_start_filename = php_output_get_start_filename();
		int output_start_lineno = php_output_get_start_lineno();

		if (output_start_filename) {
			php_error_docref(NULL, E_WARNING, "Session cookie cannot be sent after headers have already been sent (output started at %s:%d)", output_start_filename, output_start_lineno);
		} else {
			php_error_docref(NULL, E_WARNING, "Session cookie cannot be sent after headers have already been sent");
		}
		return FAILURE;
	}

	/* Prevent broken Set-Cookie header, because the session_name might be user supplied */
	if (strpbrk(PS(session_name), SESSION_FORBIDDEN_CHARS) != NULL) {   /* man isspace for \013 and \014 */
		php_error_docref(NULL, E_WARNING, "session.name cannot contain any of the following '=,;.[ \\t\\r\\n\\013\\014'");
		return FAILURE;
	}

	/* URL encode id because it might be user supplied */
	e_id = php_url_encode(ZSTR_VAL(PS(id)), ZSTR_LEN(PS(id)));

	smart_str_appendl(&ncookie, "Set-Cookie: ", sizeof("Set-Cookie: ")-1);
	smart_str_appendl(&ncookie, PS(session_name), strlen(PS(session_name)));
	smart_str_appendc(&ncookie, '=');
	smart_str_appendl(&ncookie, ZSTR_VAL(e_id), ZSTR_LEN(e_id));

	zend_string_release_ex(e_id, 0);

	if (PS(cookie_lifetime) > 0) {
		struct timeval tv;
		time_t t;

		gettimeofday(&tv, NULL);
		t = tv.tv_sec + PS(cookie_lifetime);

		if (t > 0) {
			date_fmt = php_format_date("D, d M Y H:i:s \\G\\M\\T", sizeof("D, d M Y H:i:s \\G\\M\\T")-1, t, 0);
			smart_str_appends(&ncookie, COOKIE_EXPIRES);
			smart_str_appendl(&ncookie, ZSTR_VAL(date_fmt), ZSTR_LEN(date_fmt));
			zend_string_release_ex(date_fmt, 0);

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

	if (PS(cookie_samesite)[0]) {
		smart_str_appends(&ncookie, COOKIE_SAMESITE);
		smart_str_appends(&ncookie, PS(cookie_samesite));
	}

	smart_str_0(&ncookie);

	php_session_remove_cookie(); /* remove already sent session ID cookie */
	/*	'replace' must be 0 here, else a previous Set-Cookie
		header, probably sent with setcookie() will be replaced! */
	sapi_add_header_ex(estrndup(ZSTR_VAL(ncookie.s), ZSTR_LEN(ncookie.s)), ZSTR_LEN(ncookie.s), 0, 0);
	smart_str_free(&ncookie);

	return SUCCESS;
}
/* }}} */

PHPAPI const ps_module *_php_find_ps_module(const char *name) /* {{{ */
{
	const ps_module *ret = NULL;
	const ps_module **mod;
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

PHPAPI const ps_serializer *_php_find_ps_serializer(const char *name) /* {{{ */
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


PHPAPI zend_result php_session_reset_id(void) /* {{{ */
{
	int module_number = PS(module_number);
	zval *sid, *data, *ppid;
	bool apply_trans_sid;

	if (!PS(id)) {
		php_error_docref(NULL, E_WARNING, "Cannot set session ID - session ID is not initialized");
		return FAILURE;
	}

	if (PS(use_cookies) && PS(send_cookie)) {
		php_session_send_cookie();
		PS(send_cookie) = 0;
	}

	/* If the SID constant exists, destroy it. */
	/* We must not delete any items in EG(zend_constants) */
	/* zend_hash_str_del(EG(zend_constants), "sid", sizeof("sid") - 1); */
	sid = zend_get_constant_str("SID", sizeof("SID") - 1);

	if (PS(define_sid)) {
		smart_str var = {0};

		smart_str_appends(&var, PS(session_name));
		smart_str_appendc(&var, '=');
		smart_str_appends(&var, ZSTR_VAL(PS(id)));
		smart_str_0(&var);
		if (sid) {
			zval_ptr_dtor_str(sid);
			ZVAL_STR(sid, smart_str_extract(&var));
		} else {
			REGISTER_STRINGL_CONSTANT("SID", ZSTR_VAL(var.s), ZSTR_LEN(var.s), 0);
			smart_str_free(&var);
		}
	} else {
		if (sid) {
			zval_ptr_dtor_str(sid);
			ZVAL_EMPTY_STRING(sid);
		} else {
			REGISTER_STRINGL_CONSTANT("SID", "", 0, 0);
		}
	}

	/* Apply trans sid if sid cookie is not set */
	apply_trans_sid = 0;
	if (APPLY_TRANS_SID) {
		apply_trans_sid = 1;
		if (PS(use_cookies) &&
			(data = zend_hash_str_find(&EG(symbol_table), "_COOKIE", sizeof("_COOKIE") - 1))) {
			ZVAL_DEREF(data);
			if (Z_TYPE_P(data) == IS_ARRAY &&
				(ppid = zend_hash_str_find(Z_ARRVAL_P(data), PS(session_name), strlen(PS(session_name))))) {
				ZVAL_DEREF(ppid);
				apply_trans_sid = 0;
			}
		}
	}
	if (apply_trans_sid) {
		zend_string *sname;
		sname = zend_string_init(PS(session_name), strlen(PS(session_name)), 0);
		php_url_scanner_reset_session_var(sname, 1); /* This may fail when session name has changed */
		zend_string_release_ex(sname, 0);
		php_url_scanner_add_session_var(PS(session_name), strlen(PS(session_name)), ZSTR_VAL(PS(id)), ZSTR_LEN(PS(id)), 1);
	}
	return SUCCESS;
}
/* }}} */


PHPAPI zend_result php_session_start(void) /* {{{ */
{
	zval *ppid;
	zval *data;
	char *value;
	size_t lensess;

	switch (PS(session_status)) {
		case php_session_active:
			php_error(E_NOTICE, "Ignoring session_start() because a session has already been started");
			return FAILURE;
			break;

		case php_session_disabled:
			value = zend_ini_string("session.save_handler", sizeof("session.save_handler") - 1, 0);
			if (!PS(mod) && value) {
				PS(mod) = _php_find_ps_module(value);
				if (!PS(mod)) {
					php_error_docref(NULL, E_WARNING, "Cannot find session save handler \"%s\" - session startup failed", value);
					return FAILURE;
				}
			}
			value = zend_ini_string("session.serialize_handler", sizeof("session.serialize_handler") - 1, 0);
			if (!PS(serializer) && value) {
				PS(serializer) = _php_find_ps_serializer(value);
				if (!PS(serializer)) {
					php_error_docref(NULL, E_WARNING, "Cannot find session serialization handler \"%s\" - session startup failed", value);
					return FAILURE;
				}
			}
			PS(session_status) = php_session_none;
			ZEND_FALLTHROUGH;

		case php_session_none:
		default:
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
		/* Initialize session ID from non cookie values */
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
			/* Check whether the current request was referred to by
			 * an external site which invalidates the previously found id. */
			if (PS(id) && PS(extern_referer_chk)[0] != '\0' &&
				!Z_ISUNDEF(PG(http_globals)[TRACK_VARS_SERVER]) &&
				(data = zend_hash_str_find(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER]), "HTTP_REFERER", sizeof("HTTP_REFERER") - 1)) &&
				Z_TYPE_P(data) == IS_STRING &&
				Z_STRLEN_P(data) != 0 &&
				strstr(Z_STRVAL_P(data), PS(extern_referer_chk)) == NULL
			) {
				zend_string_release_ex(PS(id), 0);
				PS(id) = NULL;
			}
		}
	}

	/* Finally check session id for dangerous characters
	 * Security note: session id may be embedded in HTML pages.*/
	if (PS(id) && strpbrk(ZSTR_VAL(PS(id)), "\r\n\t <>'\"\\")) {
		zend_string_release_ex(PS(id), 0);
		PS(id) = NULL;
	}

	if (php_session_initialize() == FAILURE
		|| php_session_cache_limiter() == -2) {
		PS(session_status) = php_session_none;
		if (PS(id)) {
			zend_string_release_ex(PS(id), 0);
			PS(id) = NULL;
		}
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

PHPAPI zend_result php_session_flush(int write) /* {{{ */
{
	if (PS(session_status) == php_session_active) {
		php_session_save_current_state(write);
		PS(session_status) = php_session_none;
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

static zend_result php_session_abort(void) /* {{{ */
{
	if (PS(session_status) == php_session_active) {
		if (PS(mod_data) || PS(mod_user_implemented)) {
			PS(mod)->s_close(&PS(mod_data));
		}
		PS(session_status) = php_session_none;
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

static zend_result php_session_reset(void) /* {{{ */
{
	if (PS(session_status) == php_session_active
		&& php_session_initialize() == SUCCESS) {
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */


/* This API is not used by any PHP modules including session currently.
   session_adapt_url() may be used to set Session ID to target url without
   starting "URL-Rewriter" output handler. */
PHPAPI void session_adapt_url(const char *url, size_t url_len, char **new_url, size_t *new_len) /* {{{ */
{
	if (APPLY_TRANS_SID && (PS(session_status) == php_session_active)) {
		*new_url = php_url_scanner_adapt_single_url(url, url_len, PS(session_name), ZSTR_VAL(PS(id)), new_len, 1);
	}
}
/* }}} */

/* ********************************
   * Userspace exported functions *
   ******************************** */

/* {{{ session_set_cookie_params(array options)
   Set session cookie parameters */
PHP_FUNCTION(session_set_cookie_params)
{
	HashTable *options_ht;
	zend_long lifetime_long;
	zend_string *lifetime = NULL, *path = NULL, *domain = NULL, *samesite = NULL;
	bool secure = 0, secure_null = 1;
	bool httponly = 0, httponly_null = 1;
	zend_string *ini_name;
	zend_result result;
	int found = 0;

	if (!PS(use_cookies)) {
		return;
	}

	ZEND_PARSE_PARAMETERS_START(1, 5)
		Z_PARAM_ARRAY_HT_OR_LONG(options_ht, lifetime_long)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(path)
		Z_PARAM_STR_OR_NULL(domain)
		Z_PARAM_BOOL_OR_NULL(secure, secure_null)
		Z_PARAM_BOOL_OR_NULL(httponly, httponly_null)
	ZEND_PARSE_PARAMETERS_END();

	if (PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_WARNING, "Session cookie parameters cannot be changed when a session is active");
		RETURN_FALSE;
	}

	if (SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Session cookie parameters cannot be changed after headers have already been sent");
		RETURN_FALSE;
	}

	if (options_ht) {
		zend_string *key;
		zval *value;

		if (path) {
			zend_argument_value_error(2, "must be null when argument #1 ($lifetime_or_options) is an array");
			RETURN_THROWS();
		}

		if (domain) {
			zend_argument_value_error(3, "must be null when argument #1 ($lifetime_or_options) is an array");
			RETURN_THROWS();
		}

		if (!secure_null) {
			zend_argument_value_error(4, "must be null when argument #1 ($lifetime_or_options) is an array");
			RETURN_THROWS();
		}

		if (!httponly_null) {
			zend_argument_value_error(5, "must be null when argument #1 ($lifetime_or_options) is an array");
			RETURN_THROWS();
		}
		ZEND_HASH_FOREACH_STR_KEY_VAL(options_ht, key, value) {
			if (key) {
				ZVAL_DEREF(value);
				if (zend_string_equals_literal_ci(key, "lifetime")) {
					lifetime = zval_get_string(value);
					found++;
				} else if (zend_string_equals_literal_ci(key, "path")) {
					path = zval_get_string(value);
					found++;
				} else if (zend_string_equals_literal_ci(key, "domain")) {
					domain = zval_get_string(value);
					found++;
				} else if (zend_string_equals_literal_ci(key, "secure")) {
					secure = zval_is_true(value);
					secure_null = 0;
					found++;
				} else if (zend_string_equals_literal_ci(key, "httponly")) {
					httponly = zval_is_true(value);
					httponly_null = 0;
					found++;
				} else if (zend_string_equals_literal_ci(key, "samesite")) {
					samesite = zval_get_string(value);
					found++;
				} else {
					php_error_docref(NULL, E_WARNING, "Argument #1 ($lifetime_or_options) contains an unrecognized key \"%s\"", ZSTR_VAL(key));
				}
			} else {
				php_error_docref(NULL, E_WARNING, "Argument #1 ($lifetime_or_options) cannot contain numeric keys");
			}
		} ZEND_HASH_FOREACH_END();

		if (found == 0) {
			zend_argument_value_error(1, "must contain at least 1 valid key");
			RETURN_THROWS();
		}
	} else {
		lifetime = zend_long_to_str(lifetime_long);
	}

	/* Exception during string conversion */
	if (EG(exception)) {
		goto cleanup;
	}

	if (lifetime) {
		ini_name = zend_string_init("session.cookie_lifetime", sizeof("session.cookie_lifetime") - 1, 0);
		result = zend_alter_ini_entry(ini_name, lifetime, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release_ex(ini_name, 0);
		if (result == FAILURE) {
			RETVAL_FALSE;
			goto cleanup;
		}
	}
	if (path) {
		ini_name = zend_string_init("session.cookie_path", sizeof("session.cookie_path") - 1, 0);
		result = zend_alter_ini_entry(ini_name, path, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release_ex(ini_name, 0);
		if (result == FAILURE) {
			RETVAL_FALSE;
			goto cleanup;
		}
	}
	if (domain) {
		ini_name = zend_string_init("session.cookie_domain", sizeof("session.cookie_domain") - 1, 0);
		result = zend_alter_ini_entry(ini_name, domain, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release_ex(ini_name, 0);
		if (result == FAILURE) {
			RETVAL_FALSE;
			goto cleanup;
		}
	}
	if (!secure_null) {
		ini_name = zend_string_init("session.cookie_secure", sizeof("session.cookie_secure") - 1, 0);
		result = zend_alter_ini_entry_chars(ini_name, secure ? "1" : "0", 1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release_ex(ini_name, 0);
		if (result == FAILURE) {
			RETVAL_FALSE;
			goto cleanup;
		}
	}
	if (!httponly_null) {
		ini_name = zend_string_init("session.cookie_httponly", sizeof("session.cookie_httponly") - 1, 0);
		result = zend_alter_ini_entry_chars(ini_name, httponly ? "1" : "0", 1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release_ex(ini_name, 0);
		if (result == FAILURE) {
			RETVAL_FALSE;
			goto cleanup;
		}
	}
	if (samesite) {
		ini_name = zend_string_init("session.cookie_samesite", sizeof("session.cookie_samesite") - 1, 0);
		result = zend_alter_ini_entry(ini_name, samesite, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release_ex(ini_name, 0);
		if (result == FAILURE) {
			RETVAL_FALSE;
			goto cleanup;
		}
	}

	RETVAL_TRUE;

cleanup:
	if (lifetime) zend_string_release(lifetime);
	if (found > 0) {
		if (path) zend_string_release(path);
		if (domain) zend_string_release(domain);
		if (samesite) zend_string_release(samesite);
	}
}
/* }}} */

/* {{{ Return the session cookie parameters */
PHP_FUNCTION(session_get_cookie_params)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);

	add_assoc_long(return_value, "lifetime", PS(cookie_lifetime));
	add_assoc_string(return_value, "path", PS(cookie_path));
	add_assoc_string(return_value, "domain", PS(cookie_domain));
	add_assoc_bool(return_value, "secure", PS(cookie_secure));
	add_assoc_bool(return_value, "httponly", PS(cookie_httponly));
	add_assoc_string(return_value, "samesite", PS(cookie_samesite));
}
/* }}} */

/* {{{ Return the current session name. If newname is given, the session name is replaced with newname */
PHP_FUNCTION(session_name)
{
	zend_string *name = NULL;
	zend_string *ini_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S!", &name) == FAILURE) {
		RETURN_THROWS();
	}

	if (name && PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_WARNING, "Session name cannot be changed when a session is active");
		RETURN_FALSE;
	}

	if (name && SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Session name cannot be changed after headers have already been sent");
		RETURN_FALSE;
	}

	RETVAL_STRING(PS(session_name));

	if (name) {
		ini_name = zend_string_init("session.name", sizeof("session.name") - 1, 0);
		zend_alter_ini_entry(ini_name, name, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release_ex(ini_name, 0);
	}
}
/* }}} */

/* {{{ Return the current module name used for accessing session data. If newname is given, the module name is replaced with newname */
PHP_FUNCTION(session_module_name)
{
	zend_string *name = NULL;
	zend_string *ini_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S!", &name) == FAILURE) {
		RETURN_THROWS();
	}

	if (name && PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_WARNING, "Session save handler module cannot be changed when a session is active");
		RETURN_FALSE;
	}

	if (name && SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Session save handler module cannot be changed after headers have already been sent");
		RETURN_FALSE;
	}

	/* Set return_value to current module name */
	if (PS(mod) && PS(mod)->s_name) {
		RETVAL_STRING(PS(mod)->s_name);
	} else {
		RETVAL_EMPTY_STRING();
	}

	if (name) {
		if (zend_string_equals_literal_ci(name, "user")) {
			zend_argument_value_error(1, "cannot be \"user\"");
			RETURN_THROWS();
		}
		if (!_php_find_ps_module(ZSTR_VAL(name))) {
			php_error_docref(NULL, E_WARNING, "Session handler module \"%s\" cannot be found", ZSTR_VAL(name));

			zval_ptr_dtor_str(return_value);
			RETURN_FALSE;
		}
		if (PS(mod_data) || PS(mod_user_implemented)) {
			PS(mod)->s_close(&PS(mod_data));
		}
		PS(mod_data) = NULL;

		ini_name = zend_string_init("session.save_handler", sizeof("session.save_handler") - 1, 0);
		zend_alter_ini_entry(ini_name, name, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release_ex(ini_name, 0);
	}
}
/* }}} */

static zend_result save_handler_check_session(void) {
	if (PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_WARNING, "Session save handler cannot be changed when a session is active");
		return FAILURE;
	}

	if (SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Session save handler cannot be changed after headers have already been sent");
		return FAILURE;
	}

	return SUCCESS;
}

static inline void set_user_save_handler_ini(void) {
	zend_string *ini_name, *ini_val;

	ini_name = zend_string_init("session.save_handler", sizeof("session.save_handler") - 1, 0);
	ini_val = zend_string_init("user", sizeof("user") - 1, 0);
	PS(set_handler) = 1;
	zend_alter_ini_entry(ini_name, ini_val, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	PS(set_handler) = 0;
	zend_string_release_ex(ini_val, 0);
	zend_string_release_ex(ini_name, 0);
}

/* {{{ Sets user-level functions */
PHP_FUNCTION(session_set_save_handler)
{
	zval *args = NULL;
	int i, num_args, argc = ZEND_NUM_ARGS();

	if (argc > 0 && argc <= 2) {
		zval *obj = NULL;
		zend_string *func_name;
		zend_function *current_mptr;
		bool register_shutdown = 1;

		if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|b", &obj, php_session_iface_entry, &register_shutdown) == FAILURE) {
			RETURN_THROWS();
		}

		if (save_handler_check_session() == FAILURE) {
			RETURN_FALSE;
		}

		/* For compatibility reason, implemented interface is not checked */
		/* Find implemented methods - SessionHandlerInterface */
		i = 0;
		ZEND_HASH_MAP_FOREACH_STR_KEY(&php_session_iface_entry->function_table, func_name) {
			if ((current_mptr = zend_hash_find_ptr(&Z_OBJCE_P(obj)->function_table, func_name))) {
				if (!Z_ISUNDEF(PS(mod_user_names).names[i])) {
					zval_ptr_dtor(&PS(mod_user_names).names[i]);
				}

				array_init_size(&PS(mod_user_names).names[i], 2);
				Z_ADDREF_P(obj);
				add_next_index_zval(&PS(mod_user_names).names[i], obj);
				add_next_index_str(&PS(mod_user_names).names[i], zend_string_copy(func_name));
			} else {
				php_error_docref(NULL, E_ERROR, "Session save handler function table is corrupt");
				RETURN_FALSE;
			}

			++i;
		} ZEND_HASH_FOREACH_END();

		/* Find implemented methods - SessionIdInterface (optional) */
		ZEND_HASH_MAP_FOREACH_STR_KEY(&php_session_id_iface_entry->function_table, func_name) {
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
		ZEND_HASH_MAP_FOREACH_STR_KEY(&php_session_update_timestamp_iface_entry->function_table, func_name) {
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


		if (PS(mod_user_class_name)) {
			zend_string_release(PS(mod_user_class_name));
		}
		PS(mod_user_class_name) = zend_string_copy(Z_OBJCE_P(obj)->name);

		if (register_shutdown) {
			/* create shutdown function */
			php_shutdown_function_entry shutdown_function_entry;
			zval callable;
			zend_result result;

			ZVAL_STRING(&callable, "session_register_shutdown");
			result = zend_fcall_info_init(&callable, 0, &shutdown_function_entry.fci,
				&shutdown_function_entry.fci_cache, NULL, NULL);

			ZEND_ASSERT(result == SUCCESS);

			/* add shutdown function, removing the old one if it exists */
			if (!register_user_shutdown_function("session_shutdown", sizeof("session_shutdown") - 1, &shutdown_function_entry)) {
				zval_ptr_dtor(&callable);
				php_error_docref(NULL, E_WARNING, "Unable to register session shutdown function");
				RETURN_FALSE;
			}
		} else {
			/* remove shutdown function */
			remove_user_shutdown_function("session_shutdown", sizeof("session_shutdown") - 1);
		}

		if (PS(session_status) != php_session_active && (!PS(mod) || PS(mod) != &ps_mod_user)) {
			set_user_save_handler_ini();
		}

		RETURN_TRUE;
	}

	/* Set procedural save handler functions */
	if (argc < 6 || PS_NUM_APIS < argc) {
		WRONG_PARAM_COUNT;
	}

	if (zend_parse_parameters(argc, "+", &args, &num_args) == FAILURE) {
		RETURN_THROWS();
	}

	/* At this point argc can only be between 6 and PS_NUM_APIS */
	for (i = 0; i < argc; i++) {
		if (!zend_is_callable(&args[i], IS_CALLABLE_SUPPRESS_DEPRECATIONS, NULL)) {
			zend_string *name = zend_get_callable_name(&args[i]);
			zend_argument_type_error(i + 1, "must be a valid callback, function \"%s\" not found or invalid function name", ZSTR_VAL(name));
			zend_string_release(name);
			RETURN_THROWS();
		}
	}

	if (save_handler_check_session() == FAILURE) {
		RETURN_FALSE;
	}

	if (PS(mod_user_class_name)) {
		zend_string_release(PS(mod_user_class_name));
		PS(mod_user_class_name) = NULL;
	}

	/* remove shutdown function */
	remove_user_shutdown_function("session_shutdown", sizeof("session_shutdown") - 1);

	if (!PS(mod) || PS(mod) != &ps_mod_user) {
		set_user_save_handler_ini();
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

/* {{{ Return the current save path passed to module_name. If newname is given, the save path is replaced with newname */
PHP_FUNCTION(session_save_path)
{
	zend_string *name = NULL;
	zend_string *ini_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|P!", &name) == FAILURE) {
		RETURN_THROWS();
	}

	if (name && PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_WARNING, "Session save path cannot be changed when a session is active");
		RETURN_FALSE;
	}

	if (name && SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Session save path cannot be changed after headers have already been sent");
		RETURN_FALSE;
	}

	RETVAL_STRING(PS(save_path));

	if (name) {
		ini_name = zend_string_init("session.save_path", sizeof("session.save_path") - 1, 0);
		zend_alter_ini_entry(ini_name, name, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release_ex(ini_name, 0);
	}
}
/* }}} */

/* {{{ Return the current session id. If newid is given, the session id is replaced with newid */
PHP_FUNCTION(session_id)
{
	zend_string *name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S!", &name) == FAILURE) {
		RETURN_THROWS();
	}

	if (name && PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_WARNING, "Session ID cannot be changed when a session is active");
		RETURN_FALSE;
	}

	if (name && PS(use_cookies) && SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Session ID cannot be changed after headers have already been sent");
		RETURN_FALSE;
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
			zend_string_release_ex(PS(id), 0);
		}
		PS(id) = zend_string_copy(name);
	}
}
/* }}} */

/* {{{ Update the current session id with a newly generated one. If delete_old_session is set to true, remove the old session. */
PHP_FUNCTION(session_regenerate_id)
{
	bool del_ses = 0;
	zend_string *data;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &del_ses) == FAILURE) {
		RETURN_THROWS();
	}

	if (PS(session_status) != php_session_active) {
		php_error_docref(NULL, E_WARNING, "Session ID cannot be regenerated when there is no active session");
		RETURN_FALSE;
	}

	if (SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Session ID cannot be regenerated after headers have already been sent");
		RETURN_FALSE;
	}

	/* Process old session data */
	if (del_ses) {
		if (PS(mod)->s_destroy(&PS(mod_data), PS(id)) == FAILURE) {
			PS(mod)->s_close(&PS(mod_data));
			PS(session_status) = php_session_none;
			if (!EG(exception)) {
				php_error_docref(NULL, E_WARNING, "Session object destruction failed. ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
			}
			RETURN_FALSE;
		}
	} else {
		zend_result ret;
		data = php_session_encode();
		if (data) {
			ret = PS(mod)->s_write(&PS(mod_data), PS(id), data, PS(gc_maxlifetime));
			zend_string_release_ex(data, 0);
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
		zend_string_release_ex(PS(session_vars), 0);
		PS(session_vars) = NULL;
	}
	zend_string_release_ex(PS(id), 0);
	PS(id) = NULL;

	if (PS(mod)->s_open(&PS(mod_data), PS(save_path), PS(session_name)) == FAILURE) {
		PS(session_status) = php_session_none;
		if (!EG(exception)) {
			zend_throw_error(NULL, "Failed to open session: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		}
		RETURN_THROWS();
	}

	PS(id) = PS(mod)->s_create_sid(&PS(mod_data));
	if (!PS(id)) {
		PS(session_status) = php_session_none;
		if (!EG(exception)) {
			zend_throw_error(NULL, "Failed to create new session ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		}
		RETURN_THROWS();
	}
	if (PS(use_strict_mode)) {
		if ((!PS(mod_user_implemented) && PS(mod)->s_validate_sid) || !Z_ISUNDEF(PS(mod_user_names).name.ps_validate_sid)) {
			int limit = 3;
			/* Try to generate non-existing ID */
			while (limit-- && PS(mod)->s_validate_sid(&PS(mod_data), PS(id)) == SUCCESS) {
				zend_string_release_ex(PS(id), 0);
				PS(id) = PS(mod)->s_create_sid(&PS(mod_data));
				if (!PS(id)) {
					PS(mod)->s_close(&PS(mod_data));
					PS(session_status) = php_session_none;
					if (!EG(exception)) {
						zend_throw_error(NULL, "Failed to create session ID by collision: %s (path: %s)", PS(mod)->s_name, PS(save_path));
					}
					RETURN_THROWS();
				}
			}
		}
		// TODO warn that ID cannot be verified? else { }
	}
	/* Read is required to make new session data at this point. */
	if (PS(mod)->s_read(&PS(mod_data), PS(id), &data, PS(gc_maxlifetime)) == FAILURE) {
		PS(mod)->s_close(&PS(mod_data));
		PS(session_status) = php_session_none;
		if (!EG(exception)) {
			zend_throw_error(NULL, "Failed to create(read) session ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		}
		RETURN_THROWS();
	}
	if (data) {
		zend_string_release_ex(data, 0);
	}

	if (PS(use_cookies)) {
		PS(send_cookie) = 1;
	}
	if (php_session_reset_id() == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Generate new session ID. Intended for user save handlers. */
PHP_FUNCTION(session_create_id)
{
	zend_string *prefix = NULL, *new_id;
	smart_str id = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &prefix) == FAILURE) {
		RETURN_THROWS();
	}

	if (prefix && ZSTR_LEN(prefix)) {
		if (php_session_valid_key(ZSTR_VAL(prefix)) == FAILURE) {
			/* E_ERROR raised for security reason. */
			php_error_docref(NULL, E_WARNING, "Prefix cannot contain special characters. Only the A-Z, a-z, 0-9, \"-\", and \",\" characters are allowed");
			RETURN_FALSE;
		} else {
			smart_str_append(&id, prefix);
		}
	}

	if (!PS(in_save_handler) && PS(session_status) == php_session_active) {
		int limit = 3;
		while (limit--) {
			new_id = PS(mod)->s_create_sid(&PS(mod_data));
			if (!PS(mod)->s_validate_sid || (PS(mod_user_implemented) && Z_ISUNDEF(PS(mod_user_names).name.ps_validate_sid))) {
				break;
			} else {
				/* Detect collision and retry */
				if (PS(mod)->s_validate_sid(&PS(mod_data), new_id) == SUCCESS) {
					zend_string_release_ex(new_id, 0);
					new_id = NULL;
					continue;
				}
				break;
			}
		}
	} else {
		new_id = php_session_create_id(NULL);
	}

	if (new_id) {
		smart_str_append(&id, new_id);
		zend_string_release_ex(new_id, 0);
	} else {
		smart_str_free(&id);
		php_error_docref(NULL, E_WARNING, "Failed to create new ID");
		RETURN_FALSE;
	}
	RETVAL_STR(smart_str_extract(&id));
}
/* }}} */

/* {{{ Return the current cache limiter. If new_cache_limited is given, the current cache_limiter is replaced with new_cache_limiter */
PHP_FUNCTION(session_cache_limiter)
{
	zend_string *limiter = NULL;
	zend_string *ini_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S!", &limiter) == FAILURE) {
		RETURN_THROWS();
	}

	if (limiter && PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_WARNING, "Session cache limiter cannot be changed when a session is active");
		RETURN_FALSE;
	}

	if (limiter && SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Session cache limiter cannot be changed after headers have already been sent");
		RETURN_FALSE;
	}

	RETVAL_STRING(PS(cache_limiter));

	if (limiter) {
		ini_name = zend_string_init("session.cache_limiter", sizeof("session.cache_limiter") - 1, 0);
		zend_alter_ini_entry(ini_name, limiter, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release_ex(ini_name, 0);
	}
}
/* }}} */

/* {{{ Return the current cache expire. If new_cache_expire is given, the current cache_expire is replaced with new_cache_expire */
PHP_FUNCTION(session_cache_expire)
{
	zend_long expires;
	bool expires_is_null = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l!", &expires, &expires_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	if (!expires_is_null && PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_WARNING, "Session cache expiration cannot be changed when a session is active");
		RETURN_LONG(PS(cache_expire));
	}

	if (!expires_is_null && SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Session cache expiration cannot be changed after headers have already been sent");
		RETURN_FALSE;
	}

	RETVAL_LONG(PS(cache_expire));

	if (!expires_is_null) {
		zend_string *ini_name = zend_string_init("session.cache_expire", sizeof("session.cache_expire") - 1, 0);
		zend_string *ini_value = zend_long_to_str(expires);
		zend_alter_ini_entry(ini_name, ini_value, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
		zend_string_release_ex(ini_name, 0);
		zend_string_release_ex(ini_value, 0);
	}
}
/* }}} */

/* {{{ Serializes the current setup and returns the serialized representation */
PHP_FUNCTION(session_encode)
{
	zend_string *enc;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	enc = php_session_encode();
	if (enc == NULL) {
		RETURN_FALSE;
	}

	RETURN_STR(enc);
}
/* }}} */

/* {{{ Deserializes data and reinitializes the variables */
PHP_FUNCTION(session_decode)
{
	zend_string *str = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		RETURN_THROWS();
	}

	if (PS(session_status) != php_session_active) {
		php_error_docref(NULL, E_WARNING, "Session data cannot be decoded when there is no active session");
		RETURN_FALSE;
	}

	if (php_session_decode(str) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

static zend_result php_session_start_set_ini(zend_string *varname, zend_string *new_value) {
	zend_result ret;
	smart_str buf ={0};
	smart_str_appends(&buf, "session");
	smart_str_appendc(&buf, '.');
	smart_str_append(&buf, varname);
	smart_str_0(&buf);
	ret = zend_alter_ini_entry_ex(buf.s, new_value, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0);
	smart_str_free(&buf);
	return ret;
}

/* {{{ Begin session */
PHP_FUNCTION(session_start)
{
	zval *options = NULL;
	zval *value;
	zend_ulong num_idx;
	zend_string *str_idx;
	zend_long read_and_close = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|a", &options) == FAILURE) {
		RETURN_THROWS();
	}

	if (PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_NOTICE, "Ignoring session_start() because a session is already active");
		RETURN_TRUE;
	}

	/*
	 * TODO: To prevent unusable session with trans sid, actual output started status is
	 * required. i.e. There shouldn't be any outputs in output buffer, otherwise session
	 * module is unable to rewrite output.
	 */
	if (PS(use_cookies) && SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Session cannot be started after headers have already been sent");
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
							zend_string *tmp_val;
							zend_string *val = zval_get_tmp_string(value, &tmp_val);
							if (php_session_start_set_ini(str_idx, val) == FAILURE) {
								php_error_docref(NULL, E_WARNING, "Setting option \"%s\" failed", ZSTR_VAL(str_idx));
							}
							zend_tmp_string_release(tmp_val);
						}
						break;
					default:
						zend_type_error("%s(): Option \"%s\" must be of type string|int|bool, %s given",
							get_active_function_name(), ZSTR_VAL(str_idx), zend_zval_type_name(value)
						);
						RETURN_THROWS();
				}
			}
			(void) num_idx;
		} ZEND_HASH_FOREACH_END();
	}

	php_session_start();

	if (PS(session_status) != php_session_active) {
		IF_SESSION_VARS() {
			zval *sess_var = Z_REFVAL(PS(http_session_vars));
			SEPARATE_ARRAY(sess_var);
			/* Clean $_SESSION. */
			zend_hash_clean(Z_ARRVAL_P(sess_var));
		}
		RETURN_FALSE;
	}

	if (read_and_close) {
		php_session_flush(0);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Destroy the current session and all data associated with it */
PHP_FUNCTION(session_destroy)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(php_session_destroy() == SUCCESS);
}
/* }}} */

/* {{{ Unset all registered variables */
PHP_FUNCTION(session_unset)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (PS(session_status) != php_session_active) {
		RETURN_FALSE;
	}

	IF_SESSION_VARS() {
		zval *sess_var = Z_REFVAL(PS(http_session_vars));
		SEPARATE_ARRAY(sess_var);

		/* Clean $_SESSION. */
		zend_hash_clean(Z_ARRVAL_P(sess_var));
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Perform GC and return number of deleted sessions */
PHP_FUNCTION(session_gc)
{
	zend_long num;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (PS(session_status) != php_session_active) {
		php_error_docref(NULL, E_WARNING, "Session cannot be garbage collected when there is no active session");
		RETURN_FALSE;
	}

	num = php_session_gc(1);
	if (num < 0) {
		RETURN_FALSE;
	}

	RETURN_LONG(num);
}
/* }}} */


/* {{{ Write session data and end session */
PHP_FUNCTION(session_write_close)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (PS(session_status) != php_session_active) {
		RETURN_FALSE;
	}
	php_session_flush(1);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Abort session and end session. Session data will not be written */
PHP_FUNCTION(session_abort)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (PS(session_status) != php_session_active) {
		RETURN_FALSE;
	}
	php_session_abort();
	RETURN_TRUE;
}
/* }}} */

/* {{{ Reset session data from saved session data */
PHP_FUNCTION(session_reset)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (PS(session_status) != php_session_active) {
		RETURN_FALSE;
	}
	php_session_reset();
	RETURN_TRUE;
}
/* }}} */

/* {{{ Returns the current session status */
PHP_FUNCTION(session_status)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(PS(session_status));
}
/* }}} */

/* {{{ Registers session_write_close() as a shutdown function */
PHP_FUNCTION(session_register_shutdown)
{
	php_shutdown_function_entry shutdown_function_entry;
	zval callable;
	zend_result result;

	ZEND_PARSE_PARAMETERS_NONE();

	/* This function is registered itself as a shutdown function by
	 * session_set_save_handler($obj). The reason we now register another
	 * shutdown function is in case the user registered their own shutdown
	 * function after calling session_set_save_handler(), which expects
	 * the session still to be available.
	 */
	ZVAL_STRING(&callable, "session_write_close");
	result = zend_fcall_info_init(&callable, 0, &shutdown_function_entry.fci,
		&shutdown_function_entry.fci_cache, NULL, NULL);

	ZEND_ASSERT(result == SUCCESS);

	if (!append_user_shutdown_function(&shutdown_function_entry)) {
		zval_ptr_dtor(&callable);

		/* Unable to register shutdown function, presumably because of lack
		 * of memory, so flush the session now. It would be done in rshutdown
		 * anyway but the handler will have had it's dtor called by then.
		 * If the user does have a later shutdown function which needs the
		 * session then tough luck.
		 */
		php_session_flush(1);
		php_error_docref(NULL, E_WARNING, "Session shutdown function cannot be registered");
	}
}
/* }}} */

/* ********************************
   * Module Setup and Destruction *
   ******************************** */

static zend_result php_rinit_session(bool auto_start) /* {{{ */
{
	php_rinit_session_globals();

	PS(mod) = NULL;
	{
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
	if (PS(session_status) == php_session_active) {
		zend_try {
			php_session_flush(1);
		} zend_end_try();
	}
	php_rshutdown_session_globals();

	/* this should NOT be done in php_rshutdown_session_globals() */
	for (int i = 0; i < PS_NUM_APIS; i++) {
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
	ps_globals->mod_user_class_name = NULL;
	ps_globals->mod_user_is_open = 0;
	ps_globals->session_vars = NULL;
	ps_globals->set_handler = 0;
	for (int i = 0; i < PS_NUM_APIS; i++) {
		ZVAL_UNDEF(&ps_globals->mod_user_names.names[i]);
	}
	ZVAL_UNDEF(&ps_globals->http_session_vars);
}
/* }}} */

static PHP_MINIT_FUNCTION(session) /* {{{ */
{
	zend_register_auto_global(zend_string_init_interned("_SESSION", sizeof("_SESSION") - 1, 1), 0, NULL);

	my_module_number = module_number;
	PS(module_number) = module_number;

	PS(session_status) = php_session_none;
	REGISTER_INI_ENTRIES();

#ifdef HAVE_LIBMM
	PHP_MINIT(ps_mm) (INIT_FUNC_ARGS_PASSTHRU);
#endif
	php_session_rfc1867_orig_callback = php_rfc1867_callback;
	php_rfc1867_callback = php_session_rfc1867_callback;

	/* Register interfaces */
	php_session_iface_entry = register_class_SessionHandlerInterface();

	php_session_id_iface_entry = register_class_SessionIdInterface();

	php_session_update_timestamp_iface_entry = register_class_SessionUpdateTimestampHandlerInterface();

	/* Register base class */
	php_session_class_entry = register_class_SessionHandler(php_session_iface_entry, php_session_id_iface_entry);

	register_session_symbols(module_number);

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
	memset(ZEND_VOIDP(&ps_modules[PREDEFINED_MODULES]), 0, (MAX_MODULES-PREDEFINED_MODULES)*sizeof(ps_module *));

	return SUCCESS;
}
/* }}} */

static PHP_MINFO_FUNCTION(session) /* {{{ */
{
	const ps_module **mod;
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

static bool early_find_sid_in(zval *dest, int where, php_session_rfc1867_progress *progress) /* {{{ */
{
	zval *ppid;

	if (Z_ISUNDEF(PG(http_globals)[where])) {
		return 0;
	}

	if ((ppid = zend_hash_str_find(Z_ARRVAL(PG(http_globals)[where]), PS(session_name), progress->sname_len))
			&& Z_TYPE_P(ppid) == IS_STRING) {
		zval_ptr_dtor(dest);
		ZVAL_COPY_DEREF(dest, ppid);
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

static bool php_check_cancel_upload(php_session_rfc1867_progress *progress) /* {{{ */
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

static zend_result php_session_rfc1867_callback(unsigned int event, void *event_data, void **extra) /* {{{ */
{
	php_session_rfc1867_progress *progress;
	zend_result retval = SUCCESS;

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
					zval_ptr_dtor(&progress->sid);
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
					if (!Z_ISUNDEF(progress->data)) {
						SEPARATE_ARRAY(&progress->data);
						add_assoc_bool_ex(&progress->data, "done", sizeof("done") - 1, 1);
						Z_LVAL_P(progress->post_bytes_processed) = data->post_bytes_processed;
						php_session_rfc1867_update(progress, 1);
					}
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
	ext_functions,
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
