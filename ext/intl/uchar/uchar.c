#include "uchar.h"
#include "intl_data.h"
#include "intl_convert.h"

#include <unicode/uchar.h>

#define IC_METHOD(mname) PHP_METHOD(IntlChar, mname)

static inline int convert_cp(UChar32* pcp, zval *zcp) {
	zend_long cp = -1;
	if (Z_TYPE_P(zcp) == IS_LONG) {
		cp = Z_LVAL_P(zcp);
	} else if (Z_TYPE_P(zcp) == IS_STRING) {
		int i = 0;
		U8_NEXT(Z_STRVAL_P(zcp), i, Z_STRLEN_P(zcp), cp);
		if (i != Z_STRLEN_P(zcp)) {
			intl_error_set_code(NULL, U_ILLEGAL_ARGUMENT_ERROR);
			intl_error_set_custom_msg(NULL, "Passing a UTF-8 character for codepoint requires a string which is exactly one UTF-8 codepoint long.", 0);
			return FAILURE;
		}
	} else {
		intl_error_set_code(NULL, U_ILLEGAL_ARGUMENT_ERROR);
		intl_error_set_custom_msg(NULL, "Invalid parameter for unicode point.  Must be either integer or UTF-8 sequence.", 0);
		return FAILURE;
	}
	if ((cp < UCHAR_MIN_VALUE) || (cp > UCHAR_MAX_VALUE)) {
		intl_error_set_code(NULL, U_ILLEGAL_ARGUMENT_ERROR);
		intl_error_set_custom_msg(NULL, "Codepoint out of range", 0);
		return FAILURE;
	}
	*pcp = (UChar32)cp;
	return SUCCESS;
}

/* {{{ proto string IntlChar::chr(int|string $codepoint)
 * Converts a numeric codepoint to UTF-8
 * Acts as an identify function when given a valid UTF-8 encoded codepoint
 */
ZEND_BEGIN_ARG_INFO_EX(chr_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, codepoint)
ZEND_END_ARG_INFO();
IC_METHOD(chr) {
	UChar32 cp;
	zval *zcp;
	char buffer[5];
	int buffer_len = 0;

	if ((zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zcp) == FAILURE) ||
	    (convert_cp(&cp, zcp) == FAILURE)) {
		return;
	}

	/* We can use unsafe because we know the codepoint is in valid range
	 * and that 4 bytes is enough for any unicode point
	 */
	U8_APPEND_UNSAFE(buffer, buffer_len, cp);
	buffer[buffer_len] = 0;
	RETURN_STRINGL(buffer, buffer_len);
}
/* }}} */

/* {{{ proto int IntlChar::ord(int|string $character)
 * Converts a UTf-8 encoded codepoint to its integer U32 value
 * Acts as an identity function when passed a valid integer codepoint
 */
ZEND_BEGIN_ARG_INFO_EX(ord_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, character)
ZEND_END_ARG_INFO();
IC_METHOD(ord) {
	UChar32 cp;
	zval *zcp;

	if ((zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zcp) == FAILURE) ||
	    (convert_cp(&cp, zcp) == FAILURE)) {
		return;
	}

	RETURN_LONG(cp);
}
/* }}} */

/* {{{ proto bool IntlChar::hasBinaryProperty(int|string $codepoint, int $property) */
ZEND_BEGIN_ARG_INFO_EX(hasBinaryProperty_arginfo, 0, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, codepoint)
	ZEND_ARG_INFO(0, property)
ZEND_END_ARG_INFO();
IC_METHOD(hasBinaryProperty) {
	UChar32 cp;
	zend_long prop;
	zval *zcp;

	if ((zend_parse_parameters(ZEND_NUM_ARGS(), "zl", &zcp, &prop) == FAILURE) ||
	    (convert_cp(&cp, zcp) == FAILURE)) {
		return;
	}

	RETURN_BOOL(u_hasBinaryProperty(cp, (UProperty)prop));
}
/* }}} */

/* {{{ proto int IntlChar::getIntPropertyValue(int|string $codepoint, int $property) */
ZEND_BEGIN_ARG_INFO_EX(getIntPropertyValue_arginfo, 0, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, codepoint)
	ZEND_ARG_INFO(0, property)
ZEND_END_ARG_INFO();
IC_METHOD(getIntPropertyValue) {
	UChar32 cp;
	zend_long prop;
	zval *zcp;

	if ((zend_parse_parameters(ZEND_NUM_ARGS(), "zl", &zcp, &prop) == FAILURE) ||
	    (convert_cp(&cp, zcp) == FAILURE)) {
		return;
	}

	RETURN_LONG(u_getIntPropertyValue(cp, (UProperty)prop));
}
/* }}} */

/* {{{ proto int IntlChar::getIntPropertyMinValue(int $property) */
ZEND_BEGIN_ARG_INFO_EX(getIntPropertyMinValue_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, property)
ZEND_END_ARG_INFO();
IC_METHOD(getIntPropertyMinValue) {
	zend_long prop;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &prop) == FAILURE) {
		return;
	}

	RETURN_LONG(u_getIntPropertyMinValue((UProperty)prop));
}
/* }}} */

/* {{{ proto int IntlChar::getIntPropertyMxValue(int $property) */
ZEND_BEGIN_ARG_INFO_EX(getIntPropertyMaxValue_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, property)
ZEND_END_ARG_INFO();
IC_METHOD(getIntPropertyMaxValue) {
	zend_long prop;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &prop) == FAILURE) {
		return;
	}

	RETURN_LONG(u_getIntPropertyMaxValue((UProperty)prop));
}
/* }}} */

/* {{{ proto float IntlChar::getNumericValue(int|string $codepoint) */
ZEND_BEGIN_ARG_INFO_EX(getNumericValue_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, codepoint)
ZEND_END_ARG_INFO();
IC_METHOD(getNumericValue) {
	UChar32 cp;
	zval *zcp;

	if ((zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zcp) == FAILURE) ||
	    (convert_cp(&cp, zcp) == FAILURE)) {
		return;
	}

	RETURN_DOUBLE(u_getNumericValue(cp));
}
/* }}} */

/* {{{ proto void IntlChar::enumCharTypes(callable $callback) */
ZEND_BEGIN_ARG_INFO_EX(enumCharTypes_arginfo, 0, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO();
typedef struct _enumCharType_data {
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
} enumCharType_data;
static UBool enumCharType_callback(enumCharType_data *context,
                                   UChar32 start, UChar32 limit,
                                   UCharCategory type) {
	zval retval;
	zval args[3];

	ZVAL_NULL(&retval);
	/* Note that $start is INclusive, whiel $limit is EXclusive
	 * Therefore (0, 32, 15) means CPs 0..31 are of type 15
	 */
	ZVAL_LONG(&args[0], start);
	ZVAL_LONG(&args[1], limit);
	ZVAL_LONG(&args[2], type);

	context->fci.retval = &retval;
	context->fci.param_count = 3;
	context->fci.params = args;

	if (zend_call_function(&context->fci, &context->fci_cache) == FAILURE) {
		intl_error_set_code(NULL, U_INTERNAL_PROGRAM_ERROR);
		intl_errors_set_custom_msg(NULL, "enumCharTypes callback failed", 0);
		zval_dtor(&retval);
		return 0;
	}
	zval_dtor(&retval);
	return 1;
}
IC_METHOD(enumCharTypes) {
	enumCharType_data context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "f", &context.fci, &context.fci_cache) == FAILURE) {
		return;
	}
	u_enumCharTypes((UCharEnumTypeRange*)enumCharType_callback, &context);
}
/* }}} */

/* {{{ proto int IntlChar::getBlockCode(int|string $codepoint) */
ZEND_BEGIN_ARG_INFO_EX(getBlockCode_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, codepoint)
ZEND_END_ARG_INFO()
IC_METHOD(getBlockCode) {
	UChar32 cp;
	zval *zcp;

	if ((zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zcp) == FAILURE) ||
	    (convert_cp(&cp, zcp) == FAILURE)) {
		return;
	}

	RETURN_LONG(ublock_getCode(cp));
}
/* }}} */

/* {{{ proto string IntlChar::charName(int|string $codepoint, int $nameChoice = IntlChar::UNICODE_CHAR_NAME) */
ZEND_BEGIN_ARG_INFO_EX(charName_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, codepoint)
	ZEND_ARG_INFO(0, nameChoice)
ZEND_END_ARG_INFO()
IC_METHOD(charName) {
	UChar32 cp;
	zval *zcp;
	UErrorCode error = U_ZERO_ERROR;
	zend_long nameChoice = U_UNICODE_CHAR_NAME;
	zend_string *buffer = NULL;
	int32_t buffer_len;

	if ((zend_parse_parameters(ZEND_NUM_ARGS(), "z|l", &zcp, &nameChoice) == FAILURE) ||
	    (convert_cp(&cp, zcp) == FAILURE)) {
		return;
	}

	buffer_len = u_charName(cp, (UCharNameChoice)nameChoice, NULL, 0, &error);
	buffer = zend_string_alloc(buffer_len, 0);
	error = U_ZERO_ERROR;
	buffer_len = u_charName(cp, (UCharNameChoice)nameChoice, ZSTR_VAL(buffer), ZSTR_LEN(buffer) + 1, &error);
	if (U_FAILURE(error)) {
		zend_string_free(buffer);
		INTL_CHECK_STATUS(error, "Failure getting character name");
	}
	RETURN_NEW_STR(buffer);
}
/* }}} */

/* {{{ proto int IntlChar::charFromName(string $characterName, int $nameChoice = IntlChar::UNICODE_CHAR_NAME) */
ZEND_BEGIN_ARG_INFO_EX(charFromName_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, characterName)
	ZEND_ARG_INFO(0, nameChoice)
ZEND_END_ARG_INFO()
IC_METHOD(charFromName) {
	char *name;
	size_t name_len;
	zend_long nameChoice = U_UNICODE_CHAR_NAME;
	UChar32 ret;
	UErrorCode error = U_ZERO_ERROR;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &name, &name_len, &nameChoice) == FAILURE) {
		return;
	}

	ret = u_charFromName((UCharNameChoice)nameChoice, name, &error);
	INTL_CHECK_STATUS(error, NULL);
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ void void IntlChar::enumCharNames(int|string $start, int|string $limit, callable $callback, int $nameChoice = IntlChar::UNICODE_CHAR_NAME) */
ZEND_BEGIN_ARG_INFO_EX(enumCharNames_arginfo, 0, ZEND_RETURN_VALUE, 3)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, limit)
	ZEND_ARG_INFO(0, callback)
	ZEND_ARG_INFO(0, nameChoice)
ZEND_END_ARG_INFO();
typedef struct _enumCharNames_data {
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
} enumCharNames_data;
static UBool enumCharNames_callback(enumCharNames_data *context,
                                    UChar32 code, UCharNameChoice nameChoice,
                                    const char *name, int32_t length) {
	zval retval;
	zval args[3];

	ZVAL_NULL(&retval);
	ZVAL_LONG(&args[0], code);
	ZVAL_LONG(&args[1], nameChoice);
	ZVAL_STRINGL(&args[2], name, length);

	context->fci.retval = &retval;
	context->fci.param_count = 3;
	context->fci.params = args;

	if (zend_call_function(&context->fci, &context->fci_cache) == FAILURE) {
		intl_error_set_code(NULL, U_INTERNAL_PROGRAM_ERROR);
		intl_error_set_custom_msg(NULL, "enumCharNames callback failed", 0);
		zval_dtor(&retval);
		zval_dtor(&args[2]);
		return 0;
	}
	zval_dtor(&retval);
	zval_dtor(&args[2]);
	return 1;
}
IC_METHOD(enumCharNames) {
	UChar32 start, limit;
	zval *zstart, *zlimit;
	enumCharNames_data context;
	zend_long nameChoice = U_UNICODE_CHAR_NAME;
	UErrorCode error = U_ZERO_ERROR;

	if ((zend_parse_parameters(ZEND_NUM_ARGS(), "zzf|l", &zstart, &zlimit, &context.fci, &context.fci_cache, &nameChoice) == FAILURE) ||
	    (convert_cp(&start, zstart) == FAILURE) ||
	    (convert_cp(&limit, zlimit) == FAILURE)) {
		return;
	}

	u_enumCharNames(start, limit, (UEnumCharNamesFn*)enumCharNames_callback, &context, nameChoice, &error);
	INTL_CHECK_STATUS(error, NULL);
}
/* }}} */

/* {{{ proto string IntlChar::getPropertyName(int $property, int $nameChoice = IntlChar::LONG_PROPERTY_NAME) */
ZEND_BEGIN_ARG_INFO_EX(getPropertyName_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, property)
	ZEND_ARG_INFO(0, nameChoice)
ZEND_END_ARG_INFO();
IC_METHOD(getPropertyName) {
	zend_long property;
	zend_long nameChoice = U_LONG_PROPERTY_NAME;
	const char *ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &property, &nameChoice) == FAILURE) {
		return;
	}

	ret = u_getPropertyName((UProperty)property, (UPropertyNameChoice)nameChoice);
	if (ret) {
		RETURN_STRING(ret);
	} else {
		intl_error_set_code(NULL, U_ILLEGAL_ARGUMENT_ERROR);
		intl_error_set_custom_msg(NULL, "Failed to get property name", 0);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int IntlChar::getPropertyEnum(string $alias) */
ZEND_BEGIN_ARG_INFO_EX(getPropertyEnum_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, alias)
ZEND_END_ARG_INFO();
IC_METHOD(getPropertyEnum) {
	char *alias;
	size_t alias_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &alias, &alias_len) == FAILURE) {
		return;
	}

	RETURN_LONG(u_getPropertyEnum(alias));
}
/* }}} */

/* {{{ proto string IntlChar::getPropertyValueName(int $property, int $value[, int $nameChoice = IntlChar::LONG_PROPERTY_NAME) */
ZEND_BEGIN_ARG_INFO_EX(getPropertyValueName_arginfo, 0, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, property)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, nameChoice)
ZEND_END_ARG_INFO();
IC_METHOD(getPropertyValueName) {
	zend_long property, value, nameChoice = U_LONG_PROPERTY_NAME;
	const char *ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll|l", &property, &value, &nameChoice) == FAILURE) {
		return;
	}

	ret = u_getPropertyValueName((UProperty)property, value, (UPropertyNameChoice)nameChoice);
	if (ret) {
		RETURN_STRING(ret);
	} else {
		intl_error_set_code(NULL, U_ILLEGAL_ARGUMENT_ERROR);
		intl_error_set_custom_msg(NULL, "Failed to get property name", 0);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int IntlChar::getPropertyValueEnum(int $property, string $name) */
ZEND_BEGIN_ARG_INFO_EX(getPropertyValueEnum_arginfo, 0, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, property)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO();
IC_METHOD(getPropertyValueEnum) {
	zend_long property;
	char *name;
	size_t name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ls", &property, &name, &name_len) == FAILURE) {
		return;
	}

	RETURN_LONG(u_getPropertyValueEnum((UProperty)property, name));
}
/* }}} */

/* {{{ proto int|string IntlChar::foldCase(int|string $codepoint, int $options = IntlChar::FOLD_CASE_DEFAULT) */
ZEND_BEGIN_ARG_INFO_EX(foldCase_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, codepoint)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO();
IC_METHOD(foldCase) {
	UChar32 cp, ret;
	zval *zcp;
	zend_long options = U_FOLD_CASE_DEFAULT;

	if ((zend_parse_parameters(ZEND_NUM_ARGS(), "z|l", &zcp, &options) == FAILURE) ||
	    (convert_cp(&cp, zcp) == FAILURE)) {
		return;
	}

	ret = u_foldCase(cp, options);
	if (Z_TYPE_P(zcp) == IS_STRING) {
		char buffer[5];
		int buffer_len = 0;
		U8_APPEND_UNSAFE(buffer, buffer_len, ret);
		buffer[buffer_len] = 0;
		RETURN_STRINGL(buffer, buffer_len);
	} else {
		RETURN_LONG(ret);
	}
}
/* }}} */

/* {{{ proto int IntlChar::digit(int|string $codepoint[, int $radix = 10]) */
ZEND_BEGIN_ARG_INFO_EX(digit_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, codepoint)
	ZEND_ARG_INFO(0, radix)
ZEND_END_ARG_INFO();
IC_METHOD(digit) {
	UChar32 cp;
	zval *zcp;
	zend_long radix = 10;
	int ret;

	if ((zend_parse_parameters(ZEND_NUM_ARGS(), "z|l", &zcp, &radix) == FAILURE) ||
	    (convert_cp(&cp, zcp) == FAILURE)) {
		return;
	}

	ret = u_digit(cp, radix);
	if (ret < 0) {
		intl_error_set_code(NULL, U_ILLEGAL_ARGUMENT_ERROR);
		intl_error_set_custom_msg(NULL, "Invalid digit", 0);
		RETURN_FALSE;
	}
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto int IntlChar::forDigit(int $digit[, int $radix = 10]) */
ZEND_BEGIN_ARG_INFO_EX(forDigit_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, digit)
	ZEND_ARG_INFO(0, radix)
ZEND_END_ARG_INFO();
IC_METHOD(forDigit) {
	zend_long digit, radix = 10;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &digit, &radix) == FAILURE) {
		return;
	}

	RETURN_LONG(u_forDigit(digit, radix));
}
/* }}} */

/* {{{ proto array IntlChar::charAge(int|string $codepoint) */
ZEND_BEGIN_ARG_INFO_EX(charAge_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, codepoint)
ZEND_END_ARG_INFO();
IC_METHOD(charAge) {
	UChar32 cp;
	zval *zcp;
	UVersionInfo version;
	int i;

	if ((zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zcp) == FAILURE) ||
	    (convert_cp(&cp, zcp) == FAILURE)) {
		return;
	}

	u_charAge(cp, version);
	array_init(return_value);
	for(i = 0; i < U_MAX_VERSION_LENGTH; ++i) {
		add_next_index_long(return_value, version[i]);
	}
}
/* }}} */

/* {{{ proto array IntlChar::getUnicodeVersion() */
ZEND_BEGIN_ARG_INFO_EX(getUnicodeVersion_arginfo, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO();
IC_METHOD(getUnicodeVersion) {
	UVersionInfo version;
	int i;


	u_getUnicodeVersion(version);
	array_init(return_value);
	for(i = 0; i < U_MAX_VERSION_LENGTH; ++i) {
		add_next_index_long(return_value, version[i]);
	}
}
/* }}} */

/* {{{ proto string IntlChar::getFC_NFKC_Closure(int|string $codepoint) */
ZEND_BEGIN_ARG_INFO_EX(getFC_NFKC_Closure_arginfo, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, codepoint)
ZEND_END_ARG_INFO();
IC_METHOD(getFC_NFKC_Closure) {
	UChar32 cp;
	zval *zcp;
	UChar *closure;
	zend_string *u8str;
	int32_t closure_len;
	UErrorCode error = U_ZERO_ERROR;

	if ((zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zcp) == FAILURE) ||
	    (convert_cp(&cp, zcp) == FAILURE)) {
		return;
	}

	closure_len = u_getFC_NFKC_Closure(cp, NULL, 0, &error);
	if (closure_len == 0) {
		RETURN_EMPTY_STRING();
	}
	closure = safe_emalloc(sizeof(UChar), closure_len + 1, 0);
	error = U_ZERO_ERROR;
	closure_len = u_getFC_NFKC_Closure(cp, closure, closure_len, &error);
	if (U_FAILURE(error)) {
		efree(closure);
		INTL_CHECK_STATUS(error, "Failed getting closure");
	}

	error = U_ZERO_ERROR;
	u8str = intl_convert_utf16_to_utf8(closure, closure_len, &error);
	efree(closure);
	INTL_CHECK_STATUS(error, "Failed converting output to UTF8");
	RETVAL_NEW_STR(u8str);
}
/* }}} */

/* {{{ proto bool IntlChar::<name>(int|string $codepoint) */
#define IC_BOOL_METHOD_CHAR(name) \
ZEND_BEGIN_ARG_INFO_EX(name##_arginfo, 0, ZEND_RETURN_VALUE, 1) \
	ZEND_ARG_INFO(0, codepoint) \
ZEND_END_ARG_INFO(); \
IC_METHOD(name) { \
	UChar32 cp; zval *zcp; \
	if ((zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zcp) == FAILURE) || \
	    (convert_cp(&cp, zcp) == FAILURE)) { return; } \
	RETURN_BOOL(u_##name(cp)); \
}
IC_BOOL_METHOD_CHAR(isUAlphabetic)
IC_BOOL_METHOD_CHAR(isULowercase)
IC_BOOL_METHOD_CHAR(isUUppercase)
IC_BOOL_METHOD_CHAR(isUWhiteSpace)
IC_BOOL_METHOD_CHAR(islower)
IC_BOOL_METHOD_CHAR(isupper)
IC_BOOL_METHOD_CHAR(istitle)
IC_BOOL_METHOD_CHAR(isdigit)
IC_BOOL_METHOD_CHAR(isalpha)
IC_BOOL_METHOD_CHAR(isalnum)
IC_BOOL_METHOD_CHAR(isxdigit)
IC_BOOL_METHOD_CHAR(ispunct)
IC_BOOL_METHOD_CHAR(isgraph)
IC_BOOL_METHOD_CHAR(isblank)
IC_BOOL_METHOD_CHAR(isdefined)
IC_BOOL_METHOD_CHAR(isspace)
IC_BOOL_METHOD_CHAR(isJavaSpaceChar)
IC_BOOL_METHOD_CHAR(isWhitespace)
IC_BOOL_METHOD_CHAR(iscntrl)
IC_BOOL_METHOD_CHAR(isISOControl)
IC_BOOL_METHOD_CHAR(isprint)
IC_BOOL_METHOD_CHAR(isbase)
IC_BOOL_METHOD_CHAR(isMirrored)
IC_BOOL_METHOD_CHAR(isIDStart)
IC_BOOL_METHOD_CHAR(isIDPart)
IC_BOOL_METHOD_CHAR(isIDIgnorable)
IC_BOOL_METHOD_CHAR(isJavaIDStart)
IC_BOOL_METHOD_CHAR(isJavaIDPart)
#undef IC_BOOL_METHOD_CHAR
/* }}} */

/* {{{ proto int IntlChar::<name>(int|string $codepoint) */
#define IC_INT_METHOD_CHAR(name) \
ZEND_BEGIN_ARG_INFO_EX(name##_arginfo, 0, ZEND_RETURN_VALUE, 1) \
	ZEND_ARG_INFO(0, codepoint) \
ZEND_END_ARG_INFO(); \
IC_METHOD(name) { \
	UChar32 cp; zval *zcp; \
	if ((zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zcp) == FAILURE) || \
	    (convert_cp(&cp, zcp) == FAILURE)) { return; } \
	RETURN_LONG(u_##name(cp)); \
}
IC_INT_METHOD_CHAR(charDirection)
IC_INT_METHOD_CHAR(charType)
IC_INT_METHOD_CHAR(getCombiningClass)
IC_INT_METHOD_CHAR(charDigitValue)
#undef IC_INT_METHOD_CHAR
/* }}} */

/* {{{ proto int|string IntlChar::<name>(int|string $codepoint)
 * Returns a utf-8 character if codepoint was passed as a utf-8 sequence
 * Returns an int otherwise
 */
#define IC_CHAR_METHOD_CHAR(name) \
ZEND_BEGIN_ARG_INFO_EX(name##_arginfo, 0, ZEND_RETURN_VALUE, 1) \
	ZEND_ARG_INFO(0, codepoint) \
ZEND_END_ARG_INFO(); \
IC_METHOD(name) { \
	UChar32 cp, ret; zval *zcp; \
	if ((zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zcp) == FAILURE) || \
	    (convert_cp(&cp, zcp) == FAILURE)) { return; } \
	ret = u_##name(cp); \
	if (Z_TYPE_P(zcp) == IS_STRING) { \
		char buffer[5]; \
		int buffer_len = 0; \
		U8_APPEND_UNSAFE(buffer, buffer_len, ret); \
		buffer[buffer_len] = 0; \
		RETURN_STRINGL(buffer, buffer_len); \
	} else { \
		RETURN_LONG(ret); \
	} \
}
IC_CHAR_METHOD_CHAR(charMirror)
IC_CHAR_METHOD_CHAR(tolower)
IC_CHAR_METHOD_CHAR(toupper)
IC_CHAR_METHOD_CHAR(totitle)
#if U_ICU_VERSION_MAJOR_NUM >= 52
IC_CHAR_METHOD_CHAR(getBidiPairedBracket)
#endif /* ICU >= 52 */
#undef IC_CHAR_METHOD_CHAR
/* }}} */

static zend_function_entry intlchar_methods[] = {
#define IC_ME(mname) PHP_ME(IntlChar, mname, mname##_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	IC_ME(chr)
	IC_ME(ord)
	IC_ME(hasBinaryProperty)
	IC_ME(isUAlphabetic)
	IC_ME(isULowercase)
	IC_ME(isUUppercase)
	IC_ME(isUWhiteSpace)
	IC_ME(getIntPropertyValue)
	IC_ME(getIntPropertyMinValue)
	IC_ME(getIntPropertyMaxValue)
	IC_ME(getNumericValue)
	IC_ME(islower)
	IC_ME(isupper)
	IC_ME(istitle)
	IC_ME(isdigit)
	IC_ME(isalpha)
	IC_ME(isalnum)
	IC_ME(isxdigit)
	IC_ME(ispunct)
	IC_ME(isgraph)
	IC_ME(isblank)
	IC_ME(isdefined)
	IC_ME(isspace)
	IC_ME(isJavaSpaceChar)
	IC_ME(isWhitespace)
	IC_ME(iscntrl)
	IC_ME(isISOControl)
	IC_ME(isprint)
	IC_ME(isbase)
	IC_ME(charDirection)
	IC_ME(isMirrored)
	IC_ME(charMirror)
#if U_ICU_VERSION_MAJOR_NUM >= 52
	IC_ME(getBidiPairedBracket)
#endif /* ICU >= 52 */
	IC_ME(charType)
	IC_ME(enumCharTypes)
	IC_ME(getCombiningClass)
	IC_ME(charDigitValue)
	IC_ME(getBlockCode)
	IC_ME(charName)
	IC_ME(charFromName)
	IC_ME(enumCharNames)
	IC_ME(getPropertyName)
	IC_ME(getPropertyEnum)
	IC_ME(getPropertyValueName)
	IC_ME(getPropertyValueEnum)
	IC_ME(isIDStart)
	IC_ME(isIDPart)
	IC_ME(isIDIgnorable)
	IC_ME(isJavaIDStart)
	IC_ME(isJavaIDPart)
	IC_ME(tolower)
	IC_ME(toupper)
	IC_ME(totitle)
	IC_ME(foldCase)
	IC_ME(digit)
	IC_ME(forDigit)
	IC_ME(charAge)
	IC_ME(getUnicodeVersion)
	IC_ME(getFC_NFKC_Closure)
#undef IC_ME
	PHP_FE_END
};

int php_uchar_minit(INIT_FUNC_ARGS) {
	zend_class_entry tmp, *ce;

	INIT_CLASS_ENTRY(tmp, "IntlChar", intlchar_methods);
	ce = zend_register_internal_class(&tmp);

#define IC_CONSTL(name, val) \
	zend_declare_class_constant_long(ce, name, strlen(name), val);

	zend_declare_class_constant_string(ce, "UNICODE_VERSION", sizeof("UNICODE_VERISON")-1, U_UNICODE_VERSION);
	IC_CONSTL("CODEPOINT_MIN", UCHAR_MIN_VALUE)
	IC_CONSTL("CODEPOINT_MAX", UCHAR_MAX_VALUE)
	IC_CONSTL("FOLD_CASE_DEFAULT", U_FOLD_CASE_DEFAULT)
	IC_CONSTL("FOLD_CASE_EXCLUDE_SPECIAL_I", U_FOLD_CASE_EXCLUDE_SPECIAL_I)

	/* All enums used by the uchar APIs.  There are a LOT of them,
	 * so they're separated out into include files,
	 * leaving this source file for actual implementation.
	 */
#define UPROPERTY(name) IC_CONSTL("PROPERTY_" #name, UCHAR_##name)
#include "uproperty-enum.h"
#undef UPROPERTY

#define UCHARCATEGORY(name) IC_CONSTL("CHAR_CATEGORY_" #name, U_##name)
#include "ucharcategory-enum.h"
#undef UCHARCATEGORY

#define UCHARDIRECTION(name) IC_CONSTL("CHAR_DIRECTION_" #name, U_##name)
#include "uchardirection-enum.h"
#undef UCHARDIRECTION

#define UBLOCKCODE(name) IC_CONSTL("BLOCK_CODE_" #name, UBLOCK_##name)
#include "ublockcode-enum.h"
#undef UBLOCKCODE

	/* Smaller, self-destribing enums */
#define UOTHER(name) IC_CONSTL(#name, U_##name)
#include "uother-enum.h"
#undef UOTHER

#undef IC_CONSTL
#undef IC_CONSTS

	return SUCCESS;
}

