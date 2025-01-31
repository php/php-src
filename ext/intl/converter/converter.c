/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sara Golemon <pollita@php.net>                              |
   +----------------------------------------------------------------------+
 */

#include "converter.h"
#include "zend_exceptions.h"

#include <unicode/utypes.h>
#include <unicode/utf8.h>
#include <unicode/utf16.h>
#include <unicode/ucnv.h>
#include <unicode/ustring.h>

#include "../intl_error.h"
#include "../intl_common.h"
#include "converter_arginfo.h"

typedef struct _php_converter_object {
	UConverter *src, *dest;
	zend_fcall_info_cache to_cache, from_cache;
	intl_error error;
	zend_object obj;
} php_converter_object;


static inline php_converter_object *php_converter_fetch_object(zend_object *obj) {
	return (php_converter_object *)((char*)(obj) - XtOffsetOf(php_converter_object, obj));
}
#define Z_INTL_CONVERTER_P(zv) php_converter_fetch_object(Z_OBJ_P(zv))

static zend_class_entry     *php_converter_ce;
static zend_object_handlers  php_converter_object_handlers;

#define CONV_GET(pzv)  (Z_INTL_CONVERTER_P((pzv)))
#define THROW_UFAILURE(obj, fname, error) php_converter_throw_failure(obj, error, \
                                          fname "() returned error " ZEND_LONG_FMT ": %s", (zend_long)error, u_errorName(error))

/* {{{ php_converter_throw_failure */
static inline void php_converter_throw_failure(php_converter_object *objval, UErrorCode error, const char *format, ...) {
	intl_error *err = objval ? &(objval->error) : NULL;
	char message[1024];
	va_list vargs;

	va_start(vargs, format);
	vsnprintf(message, sizeof(message), format, vargs);
	va_end(vargs);

	intl_errors_set(err, error, message, 1);
}
/* }}} */

/* {{{ php_converter_default_callback */
static void php_converter_default_callback(zval *return_value, zval *zobj, zend_long reason, zval *error) {
	/* Basic functionality so children can call parent::toUCallback() */
	switch (reason) {
		case UCNV_UNASSIGNED:
		case UCNV_ILLEGAL:
		case UCNV_IRREGULAR:
		{
			php_converter_object *objval = (php_converter_object*)CONV_GET(zobj);
			char chars[127];
			int8_t chars_len = sizeof(chars);
			UErrorCode uerror = U_ZERO_ERROR;
			if(!objval->src) {
				php_converter_throw_failure(objval, U_INVALID_STATE_ERROR, "Source Converter has not been initialized yet");
				chars[0] = 0x1A;
				chars[1] = 0;
				chars_len = 1;
				ZEND_TRY_ASSIGN_REF_LONG(error, U_INVALID_STATE_ERROR);
				RETVAL_STRINGL(chars, chars_len);
				return;
			}

			/* Yes, this is fairly wasteful at first glance,
			 * but considering that the alternative is to store
			 * what's sent into setSubstChars() and the fact
			 * that this is an extremely unlikely codepath
			 * I'd rather take the CPU hit here, than waste time
			 * storing a value I'm unlikely to use.
			 */
			ucnv_getSubstChars(objval->src, chars, &chars_len, &uerror);
			if (U_FAILURE(uerror)) {
				THROW_UFAILURE(objval, "ucnv_getSubstChars", uerror);
				chars[0] = 0x1A;
				chars[1] = 0;
				chars_len = 1;
			}
			ZEND_TRY_ASSIGN_REF_LONG(error, uerror);
			RETVAL_STRINGL(chars, chars_len);
		}
	}
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, toUCallback) {
	zend_long reason;
	zend_string *source, *codeUnits;
	zval *error;

	ZEND_PARSE_PARAMETERS_START(4, 4)
		Z_PARAM_LONG(reason)
		Z_PARAM_STR(source)
		Z_PARAM_STR(codeUnits)
		Z_PARAM_ZVAL(error)
	ZEND_PARSE_PARAMETERS_END();

	php_converter_default_callback(return_value, ZEND_THIS, reason, error);
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, fromUCallback) {
	zend_long reason;
	zval *source, *error;
	zend_long codePoint;

	ZEND_PARSE_PARAMETERS_START(4, 4)
		Z_PARAM_LONG(reason)
		Z_PARAM_ARRAY(source)
		Z_PARAM_LONG(codePoint)
		Z_PARAM_ZVAL(error)
	ZEND_PARSE_PARAMETERS_END();

	php_converter_default_callback(return_value, ZEND_THIS, reason, error);
}
/* }}} */

/* {{{ php_converter_check_limits */
static inline bool php_converter_check_limits(php_converter_object *objval, zend_long available, zend_long needed) {
	if (available < needed) {
		php_converter_throw_failure(objval, U_BUFFER_OVERFLOW_ERROR, "Buffer overrun " ZEND_LONG_FMT " bytes needed, " ZEND_LONG_FMT " available", needed, available);
		return 0;
	}
	return 1;
}
/* }}} */

#define TARGET_CHECK(cnvargs, needed) php_converter_check_limits(objval, cnvargs->targetLimit - cnvargs->target, needed)

/* {{{ php_converter_append_toUnicode_target */
static void php_converter_append_toUnicode_target(zval *val, UConverterToUnicodeArgs *args, php_converter_object *objval) {
	switch (Z_TYPE_P(val)) {
		case IS_NULL:
			/* Code unit is being skipped */
			return;
		case IS_LONG:
		{
			zend_long lval = Z_LVAL_P(val);
			if ((lval < 0) || (lval > 0x10FFFF)) {
				php_converter_throw_failure(objval, U_ILLEGAL_ARGUMENT_ERROR, "Invalid codepoint U+%04lx", lval);
				return;
			}
			if (lval > 0xFFFF) {
				/* Supplemental planes U+010000 - U+10FFFF */
				if (TARGET_CHECK(args, 2)) {
					/* TODO: Find the ICU call which does this properly */
					*(args->target++) = (UChar)(((lval - 0x10000) >> 10)   | 0xD800);
					*(args->target++) = (UChar)(((lval - 0x10000) & 0x3FF) | 0xDC00);
				}
				return;
			}
			/* Non-suggogate BMP codepoint */
			if (TARGET_CHECK(args, 1)) {
				*(args->target++) = (UChar)lval;
			}
			return;
		}
		case IS_STRING:
		{
			const char *strval = Z_STRVAL_P(val);
			int i = 0, strlen = Z_STRLEN_P(val);

			while((i != strlen) && TARGET_CHECK(args, 1)) {
				UChar c;
				U8_NEXT(strval, i, strlen, c);
				*(args->target++) = c;
			}
			return;
		}
		case IS_ARRAY:
		{
			HashTable *ht = Z_ARRVAL_P(val);
			zval *tmpzval;

			ZEND_HASH_FOREACH_VAL(ht, tmpzval) {
				php_converter_append_toUnicode_target(tmpzval, args, objval);
			} ZEND_HASH_FOREACH_END();
			return;
		}
		default:
			php_converter_throw_failure(objval, U_ILLEGAL_ARGUMENT_ERROR,
			                            "toUCallback() specified illegal type for substitution character");
	}
}
/* }}} */

/* {{{ php_converter_to_u_callback */
static void php_converter_to_u_callback(const void *context,
                                        UConverterToUnicodeArgs *args,
                                        const char *codeUnits, int32_t length,
                                        UConverterCallbackReason reason,
                                        UErrorCode *pErrorCode) {
	php_converter_object *objval = (php_converter_object*)context;
	zval retval;
	zval zargs[4];

	ZVAL_LONG(&zargs[0], reason);
	if (args->source) {
		ZVAL_STRINGL(&zargs[1], args->source, args->sourceLimit - args->source);
	} else {
		ZVAL_EMPTY_STRING(&zargs[1]);
	}
	if (codeUnits) {
		ZVAL_STRINGL(&zargs[2], codeUnits, length);
	} else {
		ZVAL_EMPTY_STRING(&zargs[2]);
	}
	ZVAL_LONG(&zargs[3], *pErrorCode);
	ZVAL_MAKE_REF(&zargs[3]);

	zend_call_known_fcc(&objval->to_cache, &retval, 4, zargs, NULL);
	/* When no exception is thrown */
	if (EXPECTED(!Z_ISUNDEF(retval))) {
		php_converter_append_toUnicode_target(&retval, args, objval);
		zval_ptr_dtor(&retval);
	}

	if (Z_TYPE(zargs[3]) == IS_LONG) {
		*pErrorCode = Z_LVAL(zargs[3]);
	} else if (Z_ISREF(zargs[3]) && Z_TYPE_P(Z_REFVAL(zargs[3])) == IS_LONG) {
		*pErrorCode = Z_LVAL_P(Z_REFVAL(zargs[3]));
	}

	zval_ptr_dtor(&zargs[0]);
	zval_ptr_dtor(&zargs[1]);
	zval_ptr_dtor(&zargs[2]);
	zval_ptr_dtor(&zargs[3]);
}
/* }}} */

/* {{{ php_converter_append_fromUnicode_target */
static void php_converter_append_fromUnicode_target(zval *val, UConverterFromUnicodeArgs *args, php_converter_object *objval) {
	switch (Z_TYPE_P(val)) {
		case IS_NULL:
			/* Ignore */
			return;
		case IS_LONG:
			if (TARGET_CHECK(args, 1)) {
				*(args->target++) = Z_LVAL_P(val);
			}
			return;
		case IS_STRING:
		{
			size_t vallen = Z_STRLEN_P(val);
			if (TARGET_CHECK(args, vallen)) {
				args->target = zend_mempcpy(args->target, Z_STRVAL_P(val), vallen);
			}
			return;
		}
		case IS_ARRAY:
		{
			HashTable *ht = Z_ARRVAL_P(val);
			zval *tmpzval;
			ZEND_HASH_FOREACH_VAL(ht, tmpzval) {
				php_converter_append_fromUnicode_target(tmpzval, args, objval);
			} ZEND_HASH_FOREACH_END();
			return;
		}
		default:
			php_converter_throw_failure(objval, U_ILLEGAL_ARGUMENT_ERROR, "fromUCallback() specified illegal type for substitution character");
	}
}
/* }}} */

/* {{{ php_converter_from_u_callback */
static void php_converter_from_u_callback(const void *context,
                                          UConverterFromUnicodeArgs *args,
                                          const UChar *codeUnits, int32_t length, UChar32 codePoint,
                                          UConverterCallbackReason reason,
                                          UErrorCode *pErrorCode) {
	php_converter_object *objval = (php_converter_object*)context;
	zval retval;
	zval zargs[4];

	ZVAL_LONG(&zargs[0], reason);
	array_init(&zargs[1]);
	int i = 0;
	while (i < length) {
		UChar32 c;
		U16_NEXT(codeUnits, i, length, c);
		add_next_index_long(&zargs[1], c);
	}
	ZVAL_LONG(&zargs[2], codePoint);
	ZVAL_LONG(&zargs[3], *pErrorCode);
	ZVAL_MAKE_REF(&zargs[3]);

	zend_call_known_fcc(&objval->from_cache, &retval, 4, zargs, NULL);
	/* When no exception is thrown */
	if (EXPECTED(!Z_ISUNDEF(retval))) {
		php_converter_append_fromUnicode_target(&retval, args, objval);
		zval_ptr_dtor(&retval);
	}

	if (Z_TYPE(zargs[3]) == IS_LONG) {
		*pErrorCode = Z_LVAL(zargs[3]);
	} else if (Z_ISREF(zargs[3]) && Z_TYPE_P(Z_REFVAL(zargs[3])) == IS_LONG) {
		*pErrorCode = Z_LVAL_P(Z_REFVAL(zargs[3]));
	}

	zval_ptr_dtor(&zargs[0]);
	zval_ptr_dtor(&zargs[1]);
	zval_ptr_dtor(&zargs[2]);
	zval_ptr_dtor(&zargs[3]);
}
/* }}} */

/* {{{ php_converter_set_callbacks */
static inline bool php_converter_set_callbacks(php_converter_object *objval, UConverter *cnv) {
	bool ret = true;
	UErrorCode error = U_ZERO_ERROR;

	if (objval->obj.ce == php_converter_ce) {
		/* Short-circuit having to go through method calls and data marshalling
		 * when we're using default behavior
		 */
		return 1;
	}

	ucnv_setToUCallBack(cnv, (UConverterToUCallback)php_converter_to_u_callback, (const void*)objval,
	                    NULL, NULL, &error);
	if (U_FAILURE(error)) {
		THROW_UFAILURE(objval, "ucnv_setToUCallBack", error);
		ret = 0;
	}

	error = U_ZERO_ERROR;
	ucnv_setFromUCallBack(cnv, (UConverterFromUCallback)php_converter_from_u_callback, (const void*)objval,
	                      NULL, NULL, &error);
	if (U_FAILURE(error)) {
		THROW_UFAILURE(objval, "ucnv_setFromUCallBack", error);
		ret = 0;
	}
	return ret;
}
/* }}} */

/* {{{ php_converter_set_encoding */
static bool php_converter_set_encoding(php_converter_object *objval,
                                       UConverter **pcnv,
                                       const char *enc, size_t enc_len) {
	UErrorCode error = U_ZERO_ERROR;
	UConverter *cnv = ucnv_open(enc, &error);

	if (error == U_AMBIGUOUS_ALIAS_WARNING) {
		UErrorCode getname_error = U_ZERO_ERROR;
		const char *actual_encoding = ucnv_getName(cnv, &getname_error);
		if (U_FAILURE(getname_error)) {
			/* Should never happen */
			actual_encoding = "(unknown)";
		}
		php_error_docref(NULL, E_WARNING, "Ambiguous encoding specified, using %s", actual_encoding);
	} else if (U_FAILURE(error)) {
		if (objval) {
			THROW_UFAILURE(objval, "ucnv_open", error);
		} else {
			char *msg;
			spprintf(&msg, 0, "Error setting encoding: %d - %s", (int)error, u_errorName(error));
			intl_error_set(NULL, error, msg, 1);
			efree(msg);
		}
		return false;
	}

	if (objval && !php_converter_set_callbacks(objval, cnv)) {
		return false;
	}

	if (*pcnv) {
		ucnv_close(*pcnv);
	}
	*pcnv = cnv;
	return true;
}
/* }}} */

/* {{{ php_converter_do_set_encoding */
static void php_converter_do_set_encoding(UConverter **pcnv, INTERNAL_FUNCTION_PARAMETERS) {
	php_converter_object *objval = CONV_GET(ZEND_THIS);
	char *enc;
	size_t enc_len;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(enc, enc_len)
	ZEND_PARSE_PARAMETERS_END();

	intl_errors_reset(&objval->error);

	RETURN_BOOL(php_converter_set_encoding(objval, pcnv, enc, enc_len));
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, setSourceEncoding) {
	php_converter_object *objval = CONV_GET(ZEND_THIS);
	php_converter_do_set_encoding(&(objval->src), INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, setDestinationEncoding) {
	php_converter_object *objval = CONV_GET(ZEND_THIS);
	php_converter_do_set_encoding(&(objval->dest), INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ php_converter_do_get_encoding */
static void php_converter_do_get_encoding(php_converter_object *objval, UConverter *cnv, INTERNAL_FUNCTION_PARAMETERS) {
	const char *name;

	ZEND_PARSE_PARAMETERS_NONE();

	intl_errors_reset(&objval->error);

	if (!cnv) {
		RETURN_NULL();
	}

	name = ucnv_getName(cnv, &objval->error.code);
	if (U_FAILURE(objval->error.code)) {
		THROW_UFAILURE(objval, "ucnv_getName()", objval->error.code);
		RETURN_FALSE;
	}

	RETURN_STRING(name);
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, getSourceEncoding) {
	php_converter_object *objval = CONV_GET(ZEND_THIS);
	php_converter_do_get_encoding(objval, objval->src, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, getDestinationEncoding) {
	php_converter_object *objval = CONV_GET(ZEND_THIS);
	php_converter_do_get_encoding(objval, objval->dest, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ php_converter_do_get_type */
static void php_converter_do_get_type(php_converter_object *objval, UConverter *cnv, INTERNAL_FUNCTION_PARAMETERS) {
	UConverterType t;

	ZEND_PARSE_PARAMETERS_NONE();
	intl_errors_reset(&objval->error);

	if (!cnv) {
		RETURN_NULL();
	}

	t = ucnv_getType(cnv);
	if (U_FAILURE(objval->error.code)) {
		THROW_UFAILURE(objval, "ucnv_getType", objval->error.code);
		RETURN_FALSE;
	}

	RETURN_LONG(t);
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, getSourceType) {
	php_converter_object *objval = CONV_GET(ZEND_THIS);
	php_converter_do_get_type(objval, objval->src, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, getDestinationType) {
	php_converter_object *objval = CONV_GET(ZEND_THIS);
	php_converter_do_get_type(objval, objval->dest, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ php_converter_resolve_callback */
static void php_converter_resolve_callback(
	zend_fcall_info_cache *fcc,
	zend_object *obj,
	const char *callback_name,
	size_t callback_name_len
) {
	zend_function *fn = zend_hash_str_find_ptr_lc(&obj->ce->function_table, callback_name, callback_name_len);
	ZEND_ASSERT(fn != NULL);

	fcc->function_handler = fn;
	fcc->object = obj;
	fcc->called_scope = obj->ce;
	fcc->calling_scope = NULL;
	fcc->closure = NULL;
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, __construct) {
	php_converter_object *objval = CONV_GET(ZEND_THIS);
	char *src = "utf-8";
	size_t src_len = sizeof("utf-8") - 1;
	char *dest = src;
	size_t dest_len = src_len;

	intl_error_reset(NULL);

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(dest, dest_len)
		Z_PARAM_STRING_OR_NULL(src, src_len)
	ZEND_PARSE_PARAMETERS_END();

	php_converter_set_encoding(objval, &(objval->src),  src,  src_len );
	php_converter_set_encoding(objval, &(objval->dest), dest, dest_len);
	php_converter_resolve_callback(&objval->to_cache, Z_OBJ_P(ZEND_THIS), ZEND_STRL("toUCallback"));
	php_converter_resolve_callback(&objval->from_cache, Z_OBJ_P(ZEND_THIS), ZEND_STRL("fromUCallback"));
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, setSubstChars) {
	php_converter_object *objval = CONV_GET(ZEND_THIS);
	char *chars;
	size_t chars_len;
	int ret = 1;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(chars, chars_len)
	ZEND_PARSE_PARAMETERS_END();

	intl_errors_reset(&objval->error);

	if (objval->src) {
		UErrorCode error = U_ZERO_ERROR;
		ucnv_setSubstChars(objval->src, chars, chars_len, &error);
		if (U_FAILURE(error)) {
			THROW_UFAILURE(objval, "ucnv_setSubstChars", error);
			ret = 0;
		}
	} else {
		php_converter_throw_failure(objval, U_INVALID_STATE_ERROR, "Source Converter has not been initialized yet");
		ret = 0;
	}

	if (objval->dest) {
		UErrorCode error = U_ZERO_ERROR;
		ucnv_setSubstChars(objval->dest, chars, chars_len, &error);
		if (U_FAILURE(error)) {
			THROW_UFAILURE(objval, "ucnv_setSubstChars", error);
			ret = 0;
		}
	} else {
		php_converter_throw_failure(objval, U_INVALID_STATE_ERROR, "Destination Converter has not been initialized yet");
		ret = 0;
	}

	RETURN_BOOL(ret);
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, getSubstChars) {
	php_converter_object *objval = CONV_GET(ZEND_THIS);
	char chars[127];
	int8_t chars_len = sizeof(chars);
	UErrorCode error = U_ZERO_ERROR;

	ZEND_PARSE_PARAMETERS_NONE();
	intl_errors_reset(&objval->error);

	if (!objval->src) {
		RETURN_NULL();
	}

	/* src and dest get the same subst chars set,
	 * so it doesn't really matter which one we read from
	 */
	ucnv_getSubstChars(objval->src, chars, &chars_len, &error);
	if (U_FAILURE(error)) {
		THROW_UFAILURE(objval, "ucnv_getSubstChars", error);
		RETURN_FALSE;
	}

	RETURN_STRINGL(chars, chars_len);
}
/* }}} */

/* {{{ php_converter_do_convert */
static zend_string* php_converter_do_convert(UConverter *dest_cnv,
                                             UConverter *src_cnv,  const char *src, int32_t src_len,
                                             php_converter_object *objval
                                            ) {
	UErrorCode	error = U_ZERO_ERROR;
	int32_t		temp_len, ret_len;
	zend_string	*ret;
	UChar		*temp;

	if (!src_cnv || !dest_cnv) {
		php_converter_throw_failure(objval, U_INVALID_STATE_ERROR,
		                            "Internal converters not initialized");
		return NULL;
	}

	/* Get necessary buffer size first */
	temp_len = 1 + ucnv_toUChars(src_cnv, NULL, 0, src, src_len, &error);
	if (U_FAILURE(error) && error != U_BUFFER_OVERFLOW_ERROR) {
		THROW_UFAILURE(objval, "ucnv_toUChars", error);
		return NULL;
	}
	temp = safe_emalloc(sizeof(UChar), temp_len, sizeof(UChar));

	/* Convert to intermediate UChar* array */
	error = U_ZERO_ERROR;
	temp_len = ucnv_toUChars(src_cnv, temp, temp_len, src, src_len, &error);
	if (U_FAILURE(error)) {
		THROW_UFAILURE(objval, "ucnv_toUChars", error);
		efree(temp);
		return NULL;
	}
	temp[temp_len] = 0;

	/* Get necessary output buffer size */
	ret_len = ucnv_fromUChars(dest_cnv, NULL, 0, temp, temp_len, &error);
	if (U_FAILURE(error) && error != U_BUFFER_OVERFLOW_ERROR) {
		THROW_UFAILURE(objval, "ucnv_fromUChars", error);
		efree(temp);
		return NULL;
	}

	ret = zend_string_alloc(ret_len, 0);

	/* Convert to final encoding */
	error = U_ZERO_ERROR;
	ZSTR_LEN(ret) = ucnv_fromUChars(dest_cnv, ZSTR_VAL(ret), ret_len+1, temp, temp_len, &error);
	efree(temp);
	if (U_FAILURE(error)) {
		THROW_UFAILURE(objval, "ucnv_fromUChars", error);
		zend_string_efree(ret);
		return NULL;
	}

	return ret;
}
/* }}} */

/* {{{ */
#define UCNV_REASON_CASE(v) case (UCNV_ ## v) : RETURN_STRINGL( "REASON_" #v , sizeof( "REASON_" #v ) - 1);
PHP_METHOD(UConverter, reasonText) {
	zend_long reason;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(reason)
	ZEND_PARSE_PARAMETERS_END();
	intl_error_reset(NULL);

	switch (reason) {
		UCNV_REASON_CASE(UNASSIGNED)
		UCNV_REASON_CASE(ILLEGAL)
		UCNV_REASON_CASE(IRREGULAR)
		UCNV_REASON_CASE(RESET)
		UCNV_REASON_CASE(CLOSE)
		UCNV_REASON_CASE(CLONE)
		default:
			zend_argument_value_error(1, "must be a UConverter::REASON_* constant");
			RETURN_THROWS();
	}
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, convert) {
	php_converter_object *objval = CONV_GET(ZEND_THIS);
	char *str;
	size_t str_len;
	zend_string *ret;
	bool reverse = false;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(reverse)
	ZEND_PARSE_PARAMETERS_END();
	intl_errors_reset(&objval->error);

	ret = php_converter_do_convert(reverse ? objval->src : objval->dest,
	                               reverse ? objval->dest : objval->src,
	                               str,   str_len,
	                               objval);
	if (ret) {
		RETURN_NEW_STR(ret);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, transcode) {
	char *str, *src, *dest;
	size_t str_len, src_len, dest_len;
	zval *options = NULL;
	UConverter *src_cnv = NULL, *dest_cnv = NULL;

	ZEND_PARSE_PARAMETERS_START(3, 4)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_STRING(dest, dest_len)
		Z_PARAM_STRING(src, src_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_OR_NULL(options)
	ZEND_PARSE_PARAMETERS_END();
	intl_error_reset(NULL);

	if (php_converter_set_encoding(NULL, &src_cnv,  src,  src_len) &&
	    php_converter_set_encoding(NULL, &dest_cnv, dest, dest_len)) {
	    zend_string *ret;
		UErrorCode error = U_ZERO_ERROR;

		if (options && zend_hash_num_elements(Z_ARRVAL_P(options))) {
			zval *tmpzval;

			if (U_SUCCESS(error) &&
				(tmpzval = zend_hash_str_find(Z_ARRVAL_P(options), "from_subst", sizeof("from_subst") - 1)) != NULL &&
				Z_TYPE_P(tmpzval) == IS_STRING) {
				error = U_ZERO_ERROR;
				ucnv_setSubstChars(src_cnv, Z_STRVAL_P(tmpzval), Z_STRLEN_P(tmpzval) & 0x7F, &error);
			}
			if (U_SUCCESS(error) &&
				(tmpzval = zend_hash_str_find(Z_ARRVAL_P(options), "to_subst", sizeof("to_subst") - 1)) != NULL &&
				Z_TYPE_P(tmpzval) == IS_STRING) {
				error = U_ZERO_ERROR;
				ucnv_setSubstChars(dest_cnv, Z_STRVAL_P(tmpzval), Z_STRLEN_P(tmpzval) & 0x7F, &error);
			}
		}

		if (U_SUCCESS(error) &&
			(ret = php_converter_do_convert(dest_cnv, src_cnv, str, str_len, NULL)) != NULL) {
			RETVAL_NEW_STR(ret);
		}

		if (U_FAILURE(error)) {
			THROW_UFAILURE(NULL, "transcode", error);
			RETVAL_FALSE;
		}
	} else {
		RETVAL_FALSE;
	}

	if (src_cnv) {
		ucnv_close(src_cnv);
	}
	if (dest_cnv) {
		ucnv_close(dest_cnv);
	}
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, getErrorCode) {
	php_converter_object *objval = CONV_GET(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(intl_error_get_code(&(objval->error)));
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, getErrorMessage) {
	php_converter_object *objval = CONV_GET(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	zend_string *message = intl_error_get_message(&(objval->error));
	if (message) {
		RETURN_STR(message);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, getAvailable) {
	int32_t i,
			count = ucnv_countAvailable();

	ZEND_PARSE_PARAMETERS_NONE();

	intl_error_reset(NULL);

	array_init(return_value);
	for(i = 0; i < count; i++) {
		const char *name = ucnv_getAvailableName(i);
		add_next_index_string(return_value, name);
	}
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, getAliases) {
	char *name;
	size_t name_len;
	UErrorCode error = U_ZERO_ERROR;
	uint16_t i, count;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(name, name_len)
	ZEND_PARSE_PARAMETERS_END();
	intl_error_reset(NULL);

	count = ucnv_countAliases(name, &error);
	if (U_FAILURE(error)) {
		THROW_UFAILURE(NULL, "ucnv_countAliases", error);
		RETURN_FALSE;
	}

	array_init(return_value);
	for(i = 0; i < count; i++) {
		const char *alias;

		error = U_ZERO_ERROR;
		alias = ucnv_getAlias(name, i, &error);
		if (U_FAILURE(error)) {
			THROW_UFAILURE(NULL, "ucnv_getAlias", error);
			zend_array_destroy(Z_ARR_P(return_value));
			RETURN_NULL();
		}
		add_next_index_string(return_value, alias);
	}
}
/* }}} */

/* {{{ */
PHP_METHOD(UConverter, getStandards) {
	uint16_t i, count;

	ZEND_PARSE_PARAMETERS_NONE();
	intl_error_reset(NULL);

	array_init(return_value);
	count = ucnv_countStandards();
	for(i = 0; i < count; i++) {
		UErrorCode error = U_ZERO_ERROR;
		const char *name = ucnv_getStandard(i, &error);
		if (U_FAILURE(error)) {
			THROW_UFAILURE(NULL, "ucnv_getStandard", error);
			zend_array_destroy(Z_ARR_P(return_value));
			RETURN_NULL();
		}
		add_next_index_string(return_value, name);
	}
}
/* }}} */

/* {{{ Converter create/clone/destroy */
static void php_converter_free_object(zend_object *obj) {
	php_converter_object *objval = php_converter_fetch_object(obj);

	if (objval->src) {
		ucnv_close(objval->src);
	}

	if (objval->dest) {
		ucnv_close(objval->dest);
	}

	intl_error_reset(&objval->error);
	zend_object_std_dtor(obj);
}

static zend_object *php_converter_object_ctor(zend_class_entry *ce, php_converter_object **pobjval) {
	php_converter_object *objval;

	objval = zend_object_alloc(sizeof(php_converter_object), ce);

	zend_object_std_init(&objval->obj, ce);
	object_properties_init(&objval->obj, ce);
	intl_error_init(&(objval->error));

	*pobjval = objval;

	return &objval->obj;
}

static zend_object *php_converter_create_object(zend_class_entry *ce) {
	php_converter_object *objval = NULL;
	zend_object *retval = php_converter_object_ctor(ce, &objval);

	object_properties_init(&(objval->obj), ce);

	return retval;
}

static zend_object *php_converter_clone_object(zend_object *object) {
	php_converter_object *objval, *oldobj = php_converter_fetch_object(object);
	zend_object *retval = php_converter_object_ctor(object->ce, &objval);
	UErrorCode error = U_ZERO_ERROR;

#if U_ICU_VERSION_MAJOR_NUM > 70
	objval->src = ucnv_clone(oldobj->src, &error);
#else
	objval->src = ucnv_safeClone(oldobj->src, NULL, NULL, &error);
#endif
	if (U_SUCCESS(error)) {
		error = U_ZERO_ERROR;
#if U_ICU_VERSION_MAJOR_NUM > 70
		objval->dest = ucnv_clone(oldobj->dest, &error);
#else
		objval->dest = ucnv_safeClone(oldobj->dest, NULL, NULL, &error);
#endif
	}

	if (U_FAILURE(error)) {
		zend_throw_error(NULL, "Failed to clone UConverter");
		return retval;
	}

	/* Update contexts for converter error handlers */
	php_converter_set_callbacks(objval, objval->src );
	php_converter_set_callbacks(objval, objval->dest);

	zend_objects_clone_members(&(objval->obj), &(oldobj->obj));

	/* Newly cloned object deliberately does not inherit error state from original object */

	return retval;
}
/* }}} */

/* {{{ php_converter_minit */
int php_converter_minit(INIT_FUNC_ARGS) {
	php_converter_ce = register_class_UConverter();
	php_converter_ce->create_object = php_converter_create_object;
	php_converter_ce->default_object_handlers = &php_converter_object_handlers;
	memcpy(&php_converter_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_converter_object_handlers.offset = XtOffsetOf(php_converter_object, obj);
	php_converter_object_handlers.clone_obj = php_converter_clone_object;
	php_converter_object_handlers.free_obj = php_converter_free_object;

	return SUCCESS;
}
/* }}} */
