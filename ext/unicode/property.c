/*
   +----------------------------------------------------------------------+
   | PHP Version 6                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andrei Zmievski <andrei@php.net>                            |
   +----------------------------------------------------------------------+
 */

/* $Id$ */ 

#include "php_unicode.h"

typedef UBool (*prop_check_func_t)(UChar32 ch);

typedef struct {
	zval	  			  **args[4];
	void	 			 ***thread_ctx;
	zend_fcall_info 		fci;
	zend_fcall_info_cache	fci_cache;
} char_enum_context_t;

static void check_property_impl(INTERNAL_FUNCTION_PARAMETERS, prop_check_func_t checker)
{
	UChar	   *str;
	int			str_len;
	zend_bool	result = 1;
	int			offset = 0;
	UChar32		ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "u", &str, &str_len) == FAILURE) {
		return;
	}

	if (str_len == 0) {
		RETURN_FALSE;
	}

	while (offset < str_len && result) {
		U16_NEXT(str, offset, str_len, ch);
		result = checker(ch);
	}

	RETURN_BOOL(result);
}

/* {{{ C/POSIX migration functions */

/* {{{ proto bool char_is_lower(string text) U
   Determines if the string is lowercase */
PHP_FUNCTION(char_is_lower)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_islower);
}
/* }}} */

/* {{{ proto bool char_is_upper(string text) U
   Determines if the string is uppercase */
PHP_FUNCTION(char_is_upper)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isupper);
}
/* }}} */

/* {{{ proto bool char_is_digit(string text) U
   Determines if the string consists only of digits */
PHP_FUNCTION(char_is_digit)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isdigit);
}
/* }}} */

/* {{{ proto bool char_is_alpha(string text) U
   Determines if the string consists only of letter characters */
PHP_FUNCTION(char_is_alpha)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isalpha);
}
/* }}} */

/* {{{ proto bool char_is_alnum(string text) U
   Determines if the string consists only of alpanumeric characters */
PHP_FUNCTION(char_is_alnum)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isalnum);
}
/* }}} */

/* {{{ proto bool char_is_xdigit(string text) U
   Determines if the string consists only of hexadecimal digits */
PHP_FUNCTION(char_is_xdigit)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isxdigit);
}
/* }}} */

/* {{{ proto bool char_is_punct(string text) U
   Determines if the string consists only of punctuation characters */
PHP_FUNCTION(char_is_punct)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_ispunct);
}
/* }}} */

/* {{{ proto bool char_is_graph(string text) U
   Determines if the string consists only of "graphic" characters */
PHP_FUNCTION(char_is_graph)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isgraph);
}
/* }}} */

/* {{{ proto bool char_is_blank(string text) U
   Determines if the string consists only of "blank" characters */
PHP_FUNCTION(char_is_blank)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isblank);
}
/* }}} */

/* {{{ proto bool char_is_space(string text) U
   Determines if the string consists only of space characters */
PHP_FUNCTION(char_is_space)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isspace);
}
/* }}} */

/* {{{ proto bool char_is_cntrl(string text) U
   Determines if the string consists only of control characters */
PHP_FUNCTION(char_is_cntrl)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_iscntrl);
}
/* }}} */

/* {{{ proto bool char_is_print(string text) U
   Determines if the string consists only of printable characters */
PHP_FUNCTION(char_is_print)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isprint);
}
/* }}} */

/* }}} */

/* {{{ Additional binary property functions */

/* {{{ proto bool char_is_defined(string text) U
   Determines if the string consists only of defined characters (valid Unicode points) */
PHP_FUNCTION(char_is_defined)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isdefined);
}
/* }}} */

/* {{{ proto bool char_is_id_start(string text) U
   Determines if the specified character is permissible as the first character in an identifier according to Unicode */
PHP_FUNCTION(char_is_id_start)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isIDStart);
}
/* }}} */

/* {{{ proto bool char_is_id_part(string text) U
   etermines if the specified characters are permissible in an identifier, according to Java */
PHP_FUNCTION(char_is_id_part)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isIDPart);
}
/* }}} */

/* {{{ proto bool char_is_id_ignorable(string text) U
   Determines if the specified characters should be regarded as an ignorable character in an identifier, according to Java */
PHP_FUNCTION(char_is_id_ignorable)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isIDIgnorable);
}
/* }}} */

/* {{{ proto bool char_is_iso_control(string text) U
   Determines whether the specified code points are ISO control codes */
PHP_FUNCTION(char_is_iso_control)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isISOControl);
}
/* }}} */

/* {{{ proto bool char_is_mirrored(string text) U
   Determines whether the specified characters have the Bidi_Mirrored property */
PHP_FUNCTION(char_is_mirrored)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isMirrored);
}
/* }}} */

/* {{{ proto bool char_is_base(string text) U
   Determines if the string consists of only of base characters */
PHP_FUNCTION(char_is_base)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isbase);
}
/* }}} */

/* {{{ proto bool char_is_whitespace(string text) U
   Determines if the string consists only of whitespace characters, according to Java/ICU */
PHP_FUNCTION(char_is_whitespace)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isUWhiteSpace);
}
/* }}} */

/* {{{ proto bool char_is_alphabetic(string text) U
   Determines if the string consists only of characters with Alphabetic property */
PHP_FUNCTION(char_is_alphabetic)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isUAlphabetic);
}
/* }}} */

/* {{{ proto bool char_is_uppercase(string text) U
   Determines if the string consists only of characters with Uppercase property */
PHP_FUNCTION(char_is_uppercase)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isUUppercase);
}
/* }}} */

/* {{{ proto bool char_is_lowercase (string text) U
   Determines if the string consists only of characters with Lowercase property */
PHP_FUNCTION(char_is_lowercase)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isULowercase);
}
/* }}} */

/* {{{ proto bool char_is_titlecase(string text) U
   Determines whether the string consists only of titlecase characters */
PHP_FUNCTION(char_is_titlecase)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_istitle);
}
/* }}} */

/* }}} */

/* {{{ Single character properties */

/* {{{ proto float char_get_numeric_value(char text) U
   Get the numeric value for the character, as defined in the Unicode Character Database */
PHP_FUNCTION(char_get_numeric_value)
{
	UChar	   *str;
	int			str_len;
	int			offset = 0;
	UChar32		ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "u", &str, &str_len) == FAILURE) {
		return;
	}

	if (str_len == 0) {
		RETURN_FALSE;
	}
	U16_NEXT(str, offset, str_len, ch);

	RETURN_DOUBLE(u_getNumericValue(ch));
}
/* }}} */

/* {{{ proto int char_get_combining_class(char text) U
   Returns the combining class of the character */
PHP_FUNCTION(char_get_combining_class)
{
	UChar	   *str;
	int			str_len;
	int			offset = 0;
	UChar32		ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "u", &str, &str_len) == FAILURE) {
		return;
	}

	if (str_len == 0) {
		RETURN_FALSE;
	}
	U16_NEXT(str, offset, str_len, ch);

	RETURN_LONG((long)u_getCombiningClass(ch));
}
/* }}} */

/* {{{ proto int char_get_digit_value(char text[, int radix]) U
   Returns the decimal digit value of the character (optionally in the specific radix). */
PHP_FUNCTION(char_get_digit_value)
{
	UChar	   *str;
	int			str_len;
	int			offset = 0;
	long		radix = 0;
	UChar32		ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "u|l", &str, &str_len, &radix) == FAILURE) {
		return;
	}

	if (str_len == 0) {
		RETURN_FALSE;
	}
	U16_NEXT(str, offset, str_len, ch);

	if (ZEND_NUM_ARGS() > 1) {
		if (radix < 2 || radix > 36) {
			php_error(E_WARNING, "Radix has to be in 2-36 range");
			return;
		}
		RETURN_LONG(u_digit(ch, radix));
	} else {
		RETURN_LONG(u_charDigitValue(ch));
	}
}
/* }}} */

/* {{{ proto char char_get_mirrored(char c) U
   Maps the specified character to its "mirror-image" */
PHP_FUNCTION(char_get_mirrored)
{
	UChar	   *str;
	int			str_len;
	int			offset = 0;
	UChar32		ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "u", &str, &str_len) == FAILURE) {
		return;
	}

	if (str_len == 0) {
		RETURN_FALSE;
	}
	U16_NEXT(str, offset, str_len, ch);

	RETURN_UCHAR32(u_charMirror(ch));
}
/* }}} */

/* {{{ proto int char_get_direction(char c) U
   Returns the bidirectional category value for the character, which is used in the Unicode bidirectional algorithm (UAX #9 http://www.unicode.org/reports/tr9/) */
PHP_FUNCTION(char_get_direction)
{
	UChar	   *str;
	int			str_len;
	int			offset = 0;
	UChar32		ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "u", &str, &str_len) == FAILURE) {
		return;
	}

	if (str_len == 0) {
		RETURN_FALSE;
	}
	U16_NEXT(str, offset, str_len, ch);

	RETURN_LONG((long)u_charDirection(ch));
}
/* }}} */

/* {{{ proto string char_get_age(char c) U
   Get the "age" of the code point (the Unicode version when it was first designated or assigned a character) */
PHP_FUNCTION(char_get_age)
{
	UChar	   *str;
	int			str_len;
	int			offset = 0;
	UChar32		ch;
	UVersionInfo version;
	char        buf[18] = { 0, };

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "u", &str, &str_len) == FAILURE) {
		return;
	}

	if (str_len == 0) {
		RETURN_FALSE;
	}
	U16_NEXT(str, offset, str_len, ch);

	u_charAge(ch, version);
	u_versionToString(version, buf);

	RETURN_ASCII_STRING(buf, ZSTR_DUPLICATE);
}
/* }}} */

/* {{{ proto int char_get_type(char c) U
   Returns the general category value for the code point */
PHP_FUNCTION(char_get_type)
{
	UChar	   *str;
	int			str_len;
	int			offset = 0;
	UChar32		ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "u", &str, &str_len) == FAILURE) {
		return;
	}

	if (str_len == 0) {
		RETURN_FALSE;
	}
	U16_NEXT(str, offset, str_len, ch);

	RETURN_LONG(u_charType(ch));
}
/* }}} */

/* {{{ proto bool char_is_valid(char c) U
   Determines if the the code point is valid character, according to Unicode */
PHP_FUNCTION(char_is_valid)
{
	UChar	   *str;
	int			str_len;
	int			offset = 0;
	UChar32		ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "u", &str, &str_len) == FAILURE) {
		return;
	}

	if (str_len == 0) {
		RETURN_FALSE;
	}
	U16_NEXT(str, offset, str_len, ch);

	RETURN_BOOL(U_IS_UNICODE_CHAR(ch));
}
/* }}} */

/* {{{ proto char char_from_digit(int digit[, int radix = 10]) U
   Get the character representation for the specified digit (optionally in the specified radix) */
PHP_FUNCTION(char_from_digit)
{
	long		digit;
	long		radix = 10;
	UChar32		ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &digit, &radix) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() > 1) {
		if (radix < 2 || radix > 36) {
			php_error(E_WARNING, "Radix has to be in 2-36 range");
			return;
		}
	}
	ch = u_forDigit(digit, radix);
	
	if (ch == (UChar32)0) {
		RETURN_FALSE;
	}

	RETURN_UCHAR32(ch);
}
/* }}} */

/* {{{ proto  char char_from_name(string charname[, bool extended = false]) U
   Translate a human readable character name into a codepoint */
PHP_FUNCTION(char_from_name)
{
	void	   *name;
	int			name_len;
	zend_uchar	name_type;
	UChar32		ch;
	UCharNameChoice choice = U_UNICODE_CHAR_NAME;
	zend_bool	extended = FALSE;
	char	   *buf;
	UErrorCode	status = U_ZERO_ERROR;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t|b", &name, &name_len, &name_type, &extended) == FAILURE) {
		return;
	}

	if (extended) {
		choice = U_EXTENDED_CHAR_NAME;
	}

	if (name_type == IS_UNICODE) {
		buf = zend_unicode_to_ascii(name, name_len TSRMLS_CC);
		if (buf == NULL) {
			php_error(E_WARNING, "Character name has to consist only of ASCII characters");
			RETURN_FALSE;
		}
	} else {
		buf = (char *) name;
	}

	ch = u_charFromName(choice, buf, &status);

	if (name_type == IS_UNICODE) {
		efree(buf);
	}

	if (U_SUCCESS(status)) {
		RETURN_UCHAR32(ch);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string char_get_name(char c[, bool extended = false]) U
   Get the human readable name associated with the character */
PHP_FUNCTION(char_get_name)
{
	UChar	   *str;
	int			str_len;
	int			offset = 0;
	UChar32		ch;
	zend_bool	extended = FALSE;
	UCharNameChoice choice = U_UNICODE_CHAR_NAME;
	char	   *buf;
	int			buf_len = 128;
	UErrorCode  status = U_ZERO_ERROR;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "u|b", &str, &str_len, &extended) == FAILURE) {
		return;
	}

	if (str_len == 0) {
		RETURN_FALSE;
	}

	if (extended) {
		choice = U_EXTENDED_CHAR_NAME;
	}

	U16_NEXT(str, offset, str_len, ch);

	buf = emalloc(buf_len);
	buf_len = u_charName(ch, choice, buf, buf_len, &status);
	if (buf_len == 0) {
		efree(buf);
		RETURN_FALSE;
	} else if (status == U_BUFFER_OVERFLOW_ERROR) {
		status = U_ZERO_ERROR;
		buf = erealloc(buf, buf_len+1);
		buf_len = u_charName(ch, choice, buf, buf_len+1, &status);
	}

	RETURN_ASCII_STRINGL(buf, buf_len, ZSTR_AUTOFREE);
}
/* }}} */

/* }}} */

/* {{{ Other property functions */

/* {{{ proto bool char_has_binary_property(string text, int property) U
   Determines if all the characters in the string have the specified binary property */
PHP_FUNCTION(char_has_binary_property)
{
	UChar 		*str = NULL;
	int    		 str_len;
	long   		 prop;
	UProperty	 uprop; 
	int			 offset = 0;
	zend_bool	 result = 1;
	UChar32		 ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ul", &str, &str_len, &prop) == FAILURE) {
		return;
	}

	if (str_len == 0) {
		RETURN_FALSE;
	}

	uprop = (UProperty)prop;

	while (offset < str_len && result) {
		U16_NEXT(str, offset, str_len, ch);
		result = u_hasBinaryProperty(ch, uprop);
	}

	RETURN_BOOL(result);
}
/* }}} */

/* {{{ proto int char_get_property_value(char c, int property) U
   Get the value of a property associated with the character */
PHP_FUNCTION(char_get_property_value)
{
	UChar	   *str;
	int			str_len;
	int			offset = 0;
	UChar32		ch;
	long		prop;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ul", &str, &str_len, &prop) == FAILURE) {
		return;
	}

	if (str_len == 0) {
		RETURN_FALSE;
	}

	U16_NEXT(str, offset, str_len, ch);

	if (prop >= UCHAR_BINARY_START && prop < UCHAR_BINARY_LIMIT) {
		RETURN_BOOL((zend_bool)u_getIntPropertyValue(ch, (UProperty)prop));
	} else {
		RETURN_LONG(u_getIntPropertyValue(ch, (UProperty)prop));
	}
}
/* }}} */

/* {{{ proto int char_get_property_min_value(int property) U
   Get the minimum possible value for the specified property */
PHP_FUNCTION(char_get_property_min_value)
{
	long prop;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &prop) == FAILURE) {
		return;
	}
	
	RETURN_LONG(u_getIntPropertyMinValue((UProperty)prop));
}
/* }}} */

/* {{{ proto int char_get_property_max_value(int property) U
   Get the maximum possible value associated with the specified property */
PHP_FUNCTION(char_get_property_max_value)
{
	long prop;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &prop) == FAILURE) {
		return;
	}
	
	RETURN_LONG(u_getIntPropertyMaxValue((UProperty)prop));
}
/* }}} */

/* {{{ proto string char_get_property_name(int property) U
   Get the Unicode name for the given property */
PHP_FUNCTION(char_get_property_name)
{
	long 		 prop;
	long		 name_choice = U_LONG_PROPERTY_NAME;
	const char	*name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &prop, &name_choice) == FAILURE) {
		return;
	}

	if (name_choice < 0) {
		name_choice = U_LONG_PROPERTY_NAME;
	}

	name = u_getPropertyName((UProperty) prop, (UPropertyNameChoice) name_choice);
	if (name) {
		RETURN_ASCII_STRING((char *)name, ZSTR_DUPLICATE);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int char_get_property_from_name(string property_name) U
   Get the property ID for the given property name */
PHP_FUNCTION(char_get_property_from_name)
{
	void	   *name;
	int			name_len;
	zend_uchar	name_type;
	char	   *buf;
	UProperty	prop;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &name, &name_len, &name_type) == FAILURE) {
		return;
	}

	if (name_type == IS_UNICODE) {
		buf = zend_unicode_to_ascii(name, name_len TSRMLS_CC);
		if (buf == NULL) {
			php_error(E_WARNING, "Property name has to consist only of ASCII characters");
			RETURN_FALSE;
		}
	} else {
		buf = (char *) name;
	}

	prop = u_getPropertyEnum(buf);
	if (name_type == IS_UNICODE) {
		efree(buf);
	}

	RETURN_LONG(prop);
}
/* }}} */

/* {{{ proto string char_get_property_value_name(int property, int value[, int name_choice]) U
   Get the Unicode name for the givenproperty value */
PHP_FUNCTION(char_get_property_value_name)
{
	long 		 prop;
	long		 value;
	long		 name_choice = U_LONG_PROPERTY_NAME;
	const char	*name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll|l", &prop, &value, &name_choice) == FAILURE) {
		return;
	}

	if (name_choice < 0) {
		name_choice = U_LONG_PROPERTY_NAME;
	}

	name = u_getPropertyValueName((UProperty) prop, (int32_t) value, (UPropertyNameChoice) name_choice);
	if (name) {
		RETURN_ASCII_STRING((char *)name, ZSTR_DUPLICATE);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int char_get_property_value_from_name(int property, string value_name) U
   Get the value ID for the given property value name */
PHP_FUNCTION(char_get_property_value_from_name)
{
	long		prop;
	void	   *name;
	int			name_len;
	zend_uchar	name_type;
	char	   *buf;
	long		value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lt", &prop, &name, &name_len, &name_type) == FAILURE) {
		return;
	}

	if (name_type == IS_UNICODE) {
		buf = zend_unicode_to_ascii(name, name_len TSRMLS_CC);
		if (buf == NULL) {
			php_error(E_WARNING, "Property value name has to consist only of ASCII characters");
			RETURN_FALSE;
		}
	} else {
		buf = (char *) name;
	}

	value = u_getPropertyValueEnum((UProperty)prop, buf);
	if (name_type == IS_UNICODE) {
		efree(buf);
	}

	RETURN_LONG(value);
}
/* }}} */

/* }}} */

/* {{{ Enumerator functions */

static UBool php_enum_char_names(const void *context,
								 UChar32 code,
								 UCharNameChoice nameChoice,
								 const char *name,
								 int32_t length)
{
    char_enum_context_t	   *ctx = (char_enum_context_t *)context;
    zval				   *retval_ptr = NULL;
    int						status;
    UBool					result = FALSE;
    TSRMLS_FETCH_FROM_CTX(ctx->thread_ctx);

    convert_to_long_ex(ctx->args[0]);
    convert_to_unicode_ex(ctx->args[1]);
    convert_to_boolean_ex(ctx->args[2]);

    ZVAL_LONG(*ctx->args[0], code);
	if (Z_USTRVAL_PP(ctx->args[1])) {
		efree(Z_USTRVAL_PP(ctx->args[1]));
	}
    ZVAL_ASCII_STRINGL(*ctx->args[1], (char *)name, length, ZSTR_DUPLICATE);
    ZVAL_BOOL(*ctx->args[2], nameChoice == U_EXTENDED_CHAR_NAME);

    ctx->fci.retval_ptr_ptr = &retval_ptr;

    status = zend_call_function(&ctx->fci, &ctx->fci_cache TSRMLS_CC);

    if (status == SUCCESS && retval_ptr && !EG(exception)) {
        convert_to_boolean(retval_ptr);
        result = (UBool)Z_BVAL_P(retval_ptr);
    } else {
        if (!EG(exception)) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Enumeration callback encountered an error");
        }
        result = FALSE;
    }
    if (retval_ptr) {
        zval_ptr_dtor(&retval_ptr);
    }
    return result;
}

static UBool php_enum_char_type_range(const void *context,
									  UChar32 start,
									  UChar32 limit,
									  UCharCategory type)
{
    char_enum_context_t	   *ctx = (char_enum_context_t *)context;
    zval				   *retval_ptr = NULL;
    int						status;
    UBool					result = FALSE;
    TSRMLS_FETCH_FROM_CTX(ctx->thread_ctx);

    convert_to_long_ex(ctx->args[0]);
    convert_to_long_ex(ctx->args[1]);
    convert_to_long_ex(ctx->args[2]);

    ZVAL_LONG(*ctx->args[0], start);
    ZVAL_LONG(*ctx->args[1], limit);
    ZVAL_LONG(*ctx->args[2], type);

    ctx->fci.retval_ptr_ptr = &retval_ptr;

    status = zend_call_function(&ctx->fci, &ctx->fci_cache TSRMLS_CC);

    if (status == SUCCESS && retval_ptr && !EG(exception)) {
        convert_to_boolean(retval_ptr);
        result = (UBool)Z_BVAL_P(retval_ptr);
    } else {
        if (!EG(exception)) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Enumeration callback encountered an error");
        }
        result = FALSE;
    }
    if (retval_ptr) {
        zval_ptr_dtor(&retval_ptr);
    }
    return result;
}

/* {{{ proto bool char_enum_names(callback Callback, int start, int limit[, bool extended = false]) U
   Enumerate all assigned Unicode characters between the start and limit code points (start inclusive, limit exclusive) and call a function for each, passing the code point value and the character name. */
PHP_FUNCTION(char_enum_names)
{
	zval			   *callback;
	long				start, limit;
	zend_bool			extended = FALSE;
	zval			   *zcode, *zname, *zextended;
	char_enum_context_t ectx;
	UCharNameChoice		choice = U_UNICODE_CHAR_NAME;
	UErrorCode 			status = U_ZERO_ERROR;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "zll|b", &callback, &start, &limit, &extended)) {
        return;               
    }   
    
	if (!zend_is_callable(callback, 0, NULL TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid enumeration callback");
		return;
	}

	if (extended) {
		choice = U_EXTENDED_CHAR_NAME;
	}  

    /* Do all the heavy lifing once, instead of in the callback */
    MAKE_STD_ZVAL(zcode);
    MAKE_STD_ZVAL(zextended);
    MAKE_STD_ZVAL(zname);

    ZVAL_LONG(zcode, 0);
    ZVAL_BOOL(zextended, 0);
    Z_TYPE_P(zname) = IS_UNICODE;
	Z_USTRVAL_P(zname) = NULL;

    memset(&ectx, 0, sizeof(char_enum_context_t));
    ectx.fci.size = sizeof(ectx.fci);
    ectx.fci.function_table = EG(function_table);
    ectx.fci.function_name = callback;
    ectx.fci.no_separation = 1;
    ectx.fci_cache = empty_fcall_info_cache;
    ectx.args[0] = &zcode;
    ectx.args[1] = &zname;
    ectx.args[2] = &zextended;
    ectx.fci.param_count = 3;
    ectx.fci.params = ectx.args;
    TSRMLS_SET_CTX(ectx.thread_ctx);

	u_enumCharNames(start, limit, (UEnumCharNamesFn *)php_enum_char_names,
					(void *)&ectx, choice, &status);

    zval_ptr_dtor(&zcode);
    zval_ptr_dtor(&zextended);
    zval_ptr_dtor(&zname);

	if (U_SUCCESS(status)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool char_enum_types(callback Callback) U
   Enumerate all code points with their general categories invoking a callback for each category */
PHP_FUNCTION(char_enum_types)
{
    zval			   *callback;
    zval			   *zstart, *zlimit, *ztype;
    char_enum_context_t ectx;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "z", &callback)) {
        return;               
    }   
    
	if (!zend_is_callable(callback, 0, NULL TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid enumeration callback");
		return;
	}

    /* Do all the heavy lifing once, instead of in the callback */
    MAKE_STD_ZVAL(zstart);
    MAKE_STD_ZVAL(zlimit);
    MAKE_STD_ZVAL(ztype);

    ZVAL_LONG(zstart, 0);
	ZVAL_LONG(zlimit, 0);
	ZVAL_LONG(ztype,  0);

    memset(&ectx, 0, sizeof(char_enum_context_t));
    ectx.fci.size = sizeof(ectx.fci);
    ectx.fci.function_table = EG(function_table);
    ectx.fci.function_name = callback;
    ectx.fci.no_separation = 1;
    ectx.fci_cache = empty_fcall_info_cache;
    ectx.args[0] = &zstart;
    ectx.args[1] = &zlimit;
    ectx.args[2] = &ztype;
    ectx.fci.param_count = 3;
    ectx.fci.params = ectx.args;
    TSRMLS_SET_CTX(ectx.thread_ctx);

	u_enumCharTypes((UCharEnumTypeRange *)php_enum_char_type_range, (void *)&ectx);

    zval_ptr_dtor(&zstart);
    zval_ptr_dtor(&zlimit);
    zval_ptr_dtor(&ztype);

	RETURN_TRUE;
}
/* }}} */


/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
