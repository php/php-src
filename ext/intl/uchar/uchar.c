#include "uchar.h"
#include "intl_data.h"
#include "intl_convert.h"

#include <unicode/uchar.h>
#include <unicode/utf8.h>

#include "uchar_arginfo.h"

#define IC_METHOD(mname) PHP_METHOD(IntlChar, mname)

static inline int convert_cp(UChar32* pcp, zend_string *string_codepoint, zend_long int_codepoint) {
	if (string_codepoint != NULL) {
		int32_t i = 0;
		size_t string_codepoint_length = ZSTR_LEN(string_codepoint);

		if (ZEND_SIZE_T_INT_OVFL(string_codepoint_length)) {
			intl_error_set_code(NULL, U_ILLEGAL_ARGUMENT_ERROR);
			intl_error_set_custom_msg(NULL, "Input string is too long.", 0);
			return FAILURE;
		}

		U8_NEXT(ZSTR_VAL(string_codepoint), i, string_codepoint_length, int_codepoint);
		if ((size_t)i != string_codepoint_length) {
			intl_error_set_code(NULL, U_ILLEGAL_ARGUMENT_ERROR);
			intl_error_set_custom_msg(NULL, "Passing a UTF-8 character for codepoint requires a string which is exactly one UTF-8 codepoint long.", 0);
			return FAILURE;
		}
	}

	if ((int_codepoint < UCHAR_MIN_VALUE) || (int_codepoint > UCHAR_MAX_VALUE)) {
		intl_error_set_code(NULL, U_ILLEGAL_ARGUMENT_ERROR);
		intl_error_set_custom_msg(NULL, "Codepoint out of range", 0);
		return FAILURE;
	}
	*pcp = (UChar32)int_codepoint;
	return SUCCESS;
}

static zend_never_inline int parse_code_point_param(INTERNAL_FUNCTION_PARAMETERS, UChar32 *cp) {
	zend_string *string_codepoint;
	zend_long int_codepoint = 0;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR_OR_LONG(string_codepoint, int_codepoint)
	ZEND_PARSE_PARAMETERS_END_EX(return FAILURE);
	return convert_cp(cp, string_codepoint, int_codepoint);
}

/* {{{ Converts a numeric codepoint to UTF-8
 * Acts as an identify function when given a valid UTF-8 encoded codepoint
 */
IC_METHOD(chr) {
	UChar32 cp;
	char buffer[5];
	int buffer_len = 0;

	if (parse_code_point_param(INTERNAL_FUNCTION_PARAM_PASSTHRU, &cp) == FAILURE) {
		RETURN_NULL();
	}

	/* We can use unsafe because we know the codepoint is in valid range
	 * and that 4 bytes is enough for any unicode point
	 */
	U8_APPEND_UNSAFE(buffer, buffer_len, cp);
	buffer[buffer_len] = 0;
	RETURN_STRINGL(buffer, buffer_len);
}
/* }}} */

/* {{{ Converts a UTf-8 encoded codepoint to its integer U32 value
 * Acts as an identity function when passed a valid integer codepoint
 */
IC_METHOD(ord) {
	UChar32 cp;

	if (parse_code_point_param(INTERNAL_FUNCTION_PARAM_PASSTHRU, &cp) == FAILURE) {
		RETURN_NULL();
	}

	RETURN_LONG(cp);
}
/* }}} */

/* {{{ */
IC_METHOD(hasBinaryProperty) {
	UChar32 cp;
	zend_long prop;
	zend_string *string_codepoint;
	zend_long int_codepoint = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR_OR_LONG(string_codepoint, int_codepoint)
		Z_PARAM_LONG(prop)
	ZEND_PARSE_PARAMETERS_END();

	if (convert_cp(&cp, string_codepoint, int_codepoint) == FAILURE) {
		RETURN_NULL();
	}

	RETURN_BOOL(u_hasBinaryProperty(cp, (UProperty)prop));
}
/* }}} */

/* {{{ */
IC_METHOD(getIntPropertyValue) {
	UChar32 cp;
	zend_long prop;
	zend_string *string_codepoint;
	zend_long int_codepoint = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR_OR_LONG(string_codepoint, int_codepoint)
		Z_PARAM_LONG(prop)
	ZEND_PARSE_PARAMETERS_END();

	if (convert_cp(&cp, string_codepoint, int_codepoint) == FAILURE) {
		RETURN_NULL();
	}

	RETURN_LONG(u_getIntPropertyValue(cp, (UProperty)prop));
}
/* }}} */

/* {{{ */
IC_METHOD(getIntPropertyMinValue) {
	zend_long prop;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &prop) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(u_getIntPropertyMinValue((UProperty)prop));
}
/* }}} */

/* {{{ */
IC_METHOD(getIntPropertyMaxValue) {
	zend_long prop;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &prop) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(u_getIntPropertyMaxValue((UProperty)prop));
}
/* }}} */

/* {{{ */
IC_METHOD(getNumericValue) {
	UChar32 cp;

	if (parse_code_point_param(INTERNAL_FUNCTION_PARAM_PASSTHRU, &cp) == FAILURE) {
		RETURN_NULL();
	}

	RETURN_DOUBLE(u_getNumericValue(cp));
}
/* }}} */

/* {{{ */
typedef struct _enumCharType_data {
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
} enumCharType_data;
static UBool enumCharType_callback(enumCharType_data *context,
		UChar32 start, UChar32 limit, UCharCategory type) {
	zval retval;
	zval args[3];

	ZVAL_NULL(&retval);
	/* Note that $start is INclusive, while $limit is EXclusive
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
		zval_ptr_dtor(&retval);
		return 0;
	}
	zval_ptr_dtor(&retval);
	return 1;
}
IC_METHOD(enumCharTypes) {
	enumCharType_data context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "f", &context.fci, &context.fci_cache) == FAILURE) {
		RETURN_THROWS();
	}
	u_enumCharTypes((UCharEnumTypeRange*)enumCharType_callback, &context);
}
/* }}} */

/* {{{ */
IC_METHOD(getBlockCode) {
	UChar32 cp;

	if (parse_code_point_param(INTERNAL_FUNCTION_PARAM_PASSTHRU, &cp) == FAILURE) {
		RETURN_NULL();
	}

	RETURN_LONG(ublock_getCode(cp));
}
/* }}} */

/* {{{ */
IC_METHOD(charName) {
	UChar32 cp;
	zend_string *string_codepoint;
	zend_long int_codepoint = 0;
	UErrorCode error = U_ZERO_ERROR;
	zend_long nameChoice = U_UNICODE_CHAR_NAME;
	zend_string *buffer = NULL;
	int32_t buffer_len;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR_OR_LONG(string_codepoint, int_codepoint)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(nameChoice)
	ZEND_PARSE_PARAMETERS_END();

	if (convert_cp(&cp, string_codepoint, int_codepoint) == FAILURE) {
		RETURN_NULL();
	}

	buffer_len = u_charName(cp, (UCharNameChoice)nameChoice, NULL, 0, &error);
	buffer = zend_string_alloc(buffer_len, 0);
	error = U_ZERO_ERROR;
	buffer_len = u_charName(cp, (UCharNameChoice)nameChoice, ZSTR_VAL(buffer), ZSTR_LEN(buffer) + 1, &error);
	if (U_FAILURE(error)) {
		zend_string_efree(buffer);
		INTL_CHECK_STATUS_OR_NULL(error, "Failure getting character name");
	}
	RETURN_NEW_STR(buffer);
}
/* }}} */

/* {{{ */
IC_METHOD(charFromName) {
	char *name;
	size_t name_len;
	zend_long nameChoice = U_UNICODE_CHAR_NAME;
	UChar32 ret;
	UErrorCode error = U_ZERO_ERROR;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &name, &name_len, &nameChoice) == FAILURE) {
		RETURN_THROWS();
	}

	ret = u_charFromName((UCharNameChoice)nameChoice, name, &error);
	INTL_CHECK_STATUS_OR_NULL(error, NULL);
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ void void IntlChar::enumCharNames(int|string $start, int|string $limit, callable $callback, int $nameChoice = IntlChar::UNICODE_CHAR_NAME) */
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
		zval_ptr_dtor(&retval);
		zval_ptr_dtor_str(&args[2]);
		return 0;
	}
	zval_ptr_dtor(&retval);
	zval_ptr_dtor_str(&args[2]);
	return 1;
}
IC_METHOD(enumCharNames) {
	UChar32 start, limit;
	zend_string *string_start, *string_limit;
	zend_long int_start = 0, int_limit = 0;
	enumCharNames_data context;
	zend_long nameChoice = U_UNICODE_CHAR_NAME;
	UErrorCode error = U_ZERO_ERROR;


	ZEND_PARSE_PARAMETERS_START(3, 4)
		Z_PARAM_STR_OR_LONG(string_start, int_start)
		Z_PARAM_STR_OR_LONG(string_limit, int_limit)
		Z_PARAM_FUNC(context.fci, context.fci_cache)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(nameChoice)
	ZEND_PARSE_PARAMETERS_END();

	if (convert_cp(&start, string_start, int_start) == FAILURE || convert_cp(&limit, string_limit, int_limit) == FAILURE) {
		RETURN_FALSE;
	}

	u_enumCharNames(start, limit, (UEnumCharNamesFn*)enumCharNames_callback, &context, nameChoice, &error);
	INTL_CHECK_STATUS(error, NULL);
	RETURN_TRUE;
}
/* }}} */

/* {{{ */
IC_METHOD(getPropertyName) {
	zend_long property;
	zend_long nameChoice = U_LONG_PROPERTY_NAME;
	const char *ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &property, &nameChoice) == FAILURE) {
		RETURN_THROWS();
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

/* {{{ */
IC_METHOD(getPropertyEnum) {
	char *alias;
	size_t alias_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &alias, &alias_len) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(u_getPropertyEnum(alias));
}
/* }}} */

/* {{{ */
IC_METHOD(getPropertyValueName) {
	zend_long property, value, nameChoice = U_LONG_PROPERTY_NAME;
	const char *ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll|l", &property, &value, &nameChoice) == FAILURE) {
		RETURN_THROWS();
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

/* {{{ */
IC_METHOD(getPropertyValueEnum) {
	zend_long property;
	char *name;
	size_t name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ls", &property, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(u_getPropertyValueEnum((UProperty)property, name));
}
/* }}} */

/* {{{ */
IC_METHOD(foldCase) {
	UChar32 cp, ret;
	zend_long options = U_FOLD_CASE_DEFAULT;
	zend_string *string_codepoint;
	zend_long int_codepoint = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR_OR_LONG(string_codepoint, int_codepoint)
		Z_PARAM_LONG(options)
	ZEND_PARSE_PARAMETERS_END();

	if (convert_cp(&cp, string_codepoint, int_codepoint) == FAILURE) {
		RETURN_NULL();
	}

	ret = u_foldCase(cp, options);
	if (string_codepoint != NULL) {
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

/* {{{ */
IC_METHOD(digit) {
	UChar32 cp;
	zend_long radix = 10;
	int ret;
	zend_string *string_codepoint;
	zend_long int_codepoint = 0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR_OR_LONG(string_codepoint, int_codepoint)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(radix)
	ZEND_PARSE_PARAMETERS_END();

	if (convert_cp(&cp, string_codepoint, int_codepoint) == FAILURE) {
		RETURN_NULL();
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

/* {{{ */
IC_METHOD(forDigit) {
	zend_long digit, radix = 10;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &digit, &radix) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(u_forDigit(digit, radix));
}
/* }}} */

/* {{{ */
IC_METHOD(charAge) {
	UChar32 cp;
	UVersionInfo version;
	int i;

	if (parse_code_point_param(INTERNAL_FUNCTION_PARAM_PASSTHRU, &cp) == FAILURE) {
		RETURN_NULL();
	}

	u_charAge(cp, version);
	array_init(return_value);
	for(i = 0; i < U_MAX_VERSION_LENGTH; ++i) {
		add_next_index_long(return_value, version[i]);
	}
}
/* }}} */

/* {{{ */
IC_METHOD(getUnicodeVersion) {
	UVersionInfo version;
	int i;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	u_getUnicodeVersion(version);
	array_init(return_value);
	for(i = 0; i < U_MAX_VERSION_LENGTH; ++i) {
		add_next_index_long(return_value, version[i]);
	}
}
/* }}} */

/* {{{ */
IC_METHOD(getFC_NFKC_Closure) {
	UChar32 cp;
	UChar *closure;
	zend_string *u8str;
	int32_t closure_len;
	UErrorCode error = U_ZERO_ERROR;

	if (parse_code_point_param(INTERNAL_FUNCTION_PARAM_PASSTHRU, &cp) == FAILURE) {
		RETURN_NULL();
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
	INTL_CHECK_STATUS(error, "Failed converting output to UTF8");
	efree(closure);
	RETVAL_NEW_STR(u8str);
}
/* }}} */

/* {{{ */
#define IC_BOOL_METHOD_CHAR(name) \
IC_METHOD(name) { \
	UChar32 cp; \
	if (parse_code_point_param(INTERNAL_FUNCTION_PARAM_PASSTHRU, &cp) == FAILURE) { \
		RETURN_NULL(); \
	} \
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

/* {{{ */
#define IC_INT_METHOD_CHAR(name) \
IC_METHOD(name) { \
	UChar32 cp; \
	if (parse_code_point_param(INTERNAL_FUNCTION_PARAM_PASSTHRU, &cp) == FAILURE) { \
		RETURN_NULL(); \
	} \
	RETURN_LONG(u_##name(cp)); \
}
IC_INT_METHOD_CHAR(charDirection)
IC_INT_METHOD_CHAR(charType)
IC_INT_METHOD_CHAR(getCombiningClass)
IC_INT_METHOD_CHAR(charDigitValue)
#undef IC_INT_METHOD_CHAR
/* }}} */

/* {{{ Returns a utf-8 character if codepoint was passed as a utf-8 sequence
 * Returns an int otherwise
 */
#define IC_CHAR_METHOD_CHAR(name) \
IC_METHOD(name) { \
	UChar32 cp, ret; \
	zend_string *string_codepoint; \
		zend_long int_codepoint = -1; \
		ZEND_PARSE_PARAMETERS_START(1, 1) \
			Z_PARAM_STR_OR_LONG(string_codepoint, int_codepoint) \
		ZEND_PARSE_PARAMETERS_END(); \
		if (convert_cp(&cp, string_codepoint, int_codepoint) == FAILURE) { \
			RETURN_NULL(); \
		} \
	ret = u_##name(cp); \
	if (string_codepoint != NULL) { \
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

int php_uchar_minit(INIT_FUNC_ARGS) {
	register_class_IntlChar();

	return SUCCESS;
}
