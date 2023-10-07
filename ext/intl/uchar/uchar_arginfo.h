/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 1a3b0e0186557b59b99437c23951c4bf96e50c67 */

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_hasBinaryProperty, 0, 2, _IS_BOOL, 1)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, property, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_charAge, 0, 1, IS_ARRAY, 1)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_charDigitValue, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlChar_charDirection arginfo_class_IntlChar_charDigitValue

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_charFromName, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "IntlChar::UNICODE_CHAR_NAME")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlChar_charMirror, 0, 1, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_NULL)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_charName, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "IntlChar::UNICODE_CHAR_NAME")
ZEND_END_ARG_INFO()

#define arginfo_class_IntlChar_charType arginfo_class_IntlChar_charDigitValue

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_chr, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlChar_digit, 0, 1, MAY_BE_LONG|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, base, IS_LONG, 0, "10")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_enumCharNames, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(0, start, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, end, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "IntlChar::UNICODE_CHAR_NAME")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_enumCharTypes, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlChar_foldCase, 0, 1, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_NULL)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "IntlChar::FOLD_CASE_DEFAULT")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_forDigit, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, digit, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, base, IS_LONG, 0, "10")
ZEND_END_ARG_INFO()

#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlChar_getBidiPairedBracket, 0, 1, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_NULL)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()
#endif

#define arginfo_class_IntlChar_getBlockCode arginfo_class_IntlChar_charDigitValue

#define arginfo_class_IntlChar_getCombiningClass arginfo_class_IntlChar_charDigitValue

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlChar_getFC_NFKC_Closure, 0, 1, MAY_BE_STRING|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_getIntPropertyMaxValue, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, property, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlChar_getIntPropertyMinValue arginfo_class_IntlChar_getIntPropertyMaxValue

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_getIntPropertyValue, 0, 2, IS_LONG, 1)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, property, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_getNumericValue, 0, 1, IS_DOUBLE, 1)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_getPropertyEnum, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, alias, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlChar_getPropertyName, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, property, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "IntlChar::LONG_PROPERTY_NAME")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_getPropertyValueEnum, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, property, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlChar_getPropertyValueName, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, property, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "IntlChar::LONG_PROPERTY_NAME")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_getUnicodeVersion, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_isalnum, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlChar_isalpha arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isbase arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isblank arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_iscntrl arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isdefined arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isdigit arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isgraph arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isIDIgnorable arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isIDPart arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isIDStart arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isISOControl arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isJavaIDPart arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isJavaIDStart arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isJavaSpaceChar arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_islower arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isMirrored arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isprint arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_ispunct arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isspace arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_istitle arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isUAlphabetic arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isULowercase arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isupper arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isUUppercase arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isUWhiteSpace arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isWhitespace arginfo_class_IntlChar_isalnum

#define arginfo_class_IntlChar_isxdigit arginfo_class_IntlChar_isalnum

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlChar_ord, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_MASK(0, character, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlChar_tolower arginfo_class_IntlChar_charMirror

#define arginfo_class_IntlChar_totitle arginfo_class_IntlChar_charMirror

#define arginfo_class_IntlChar_toupper arginfo_class_IntlChar_charMirror


ZEND_METHOD(IntlChar, hasBinaryProperty);
ZEND_METHOD(IntlChar, charAge);
ZEND_METHOD(IntlChar, charDigitValue);
ZEND_METHOD(IntlChar, charDirection);
ZEND_METHOD(IntlChar, charFromName);
ZEND_METHOD(IntlChar, charMirror);
ZEND_METHOD(IntlChar, charName);
ZEND_METHOD(IntlChar, charType);
ZEND_METHOD(IntlChar, chr);
ZEND_METHOD(IntlChar, digit);
ZEND_METHOD(IntlChar, enumCharNames);
ZEND_METHOD(IntlChar, enumCharTypes);
ZEND_METHOD(IntlChar, foldCase);
ZEND_METHOD(IntlChar, forDigit);
#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_METHOD(IntlChar, getBidiPairedBracket);
#endif
ZEND_METHOD(IntlChar, getBlockCode);
ZEND_METHOD(IntlChar, getCombiningClass);
ZEND_METHOD(IntlChar, getFC_NFKC_Closure);
ZEND_METHOD(IntlChar, getIntPropertyMaxValue);
ZEND_METHOD(IntlChar, getIntPropertyMinValue);
ZEND_METHOD(IntlChar, getIntPropertyValue);
ZEND_METHOD(IntlChar, getNumericValue);
ZEND_METHOD(IntlChar, getPropertyEnum);
ZEND_METHOD(IntlChar, getPropertyName);
ZEND_METHOD(IntlChar, getPropertyValueEnum);
ZEND_METHOD(IntlChar, getPropertyValueName);
ZEND_METHOD(IntlChar, getUnicodeVersion);
ZEND_METHOD(IntlChar, isalnum);
ZEND_METHOD(IntlChar, isalpha);
ZEND_METHOD(IntlChar, isbase);
ZEND_METHOD(IntlChar, isblank);
ZEND_METHOD(IntlChar, iscntrl);
ZEND_METHOD(IntlChar, isdefined);
ZEND_METHOD(IntlChar, isdigit);
ZEND_METHOD(IntlChar, isgraph);
ZEND_METHOD(IntlChar, isIDIgnorable);
ZEND_METHOD(IntlChar, isIDPart);
ZEND_METHOD(IntlChar, isIDStart);
ZEND_METHOD(IntlChar, isISOControl);
ZEND_METHOD(IntlChar, isJavaIDPart);
ZEND_METHOD(IntlChar, isJavaIDStart);
ZEND_METHOD(IntlChar, isJavaSpaceChar);
ZEND_METHOD(IntlChar, islower);
ZEND_METHOD(IntlChar, isMirrored);
ZEND_METHOD(IntlChar, isprint);
ZEND_METHOD(IntlChar, ispunct);
ZEND_METHOD(IntlChar, isspace);
ZEND_METHOD(IntlChar, istitle);
ZEND_METHOD(IntlChar, isUAlphabetic);
ZEND_METHOD(IntlChar, isULowercase);
ZEND_METHOD(IntlChar, isupper);
ZEND_METHOD(IntlChar, isUUppercase);
ZEND_METHOD(IntlChar, isUWhiteSpace);
ZEND_METHOD(IntlChar, isWhitespace);
ZEND_METHOD(IntlChar, isxdigit);
ZEND_METHOD(IntlChar, ord);
ZEND_METHOD(IntlChar, tolower);
ZEND_METHOD(IntlChar, totitle);
ZEND_METHOD(IntlChar, toupper);


static const zend_function_entry class_IntlChar_methods[] = {
	ZEND_ME(IntlChar, hasBinaryProperty, arginfo_class_IntlChar_hasBinaryProperty, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, charAge, arginfo_class_IntlChar_charAge, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, charDigitValue, arginfo_class_IntlChar_charDigitValue, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, charDirection, arginfo_class_IntlChar_charDirection, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, charFromName, arginfo_class_IntlChar_charFromName, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, charMirror, arginfo_class_IntlChar_charMirror, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, charName, arginfo_class_IntlChar_charName, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, charType, arginfo_class_IntlChar_charType, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, chr, arginfo_class_IntlChar_chr, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, digit, arginfo_class_IntlChar_digit, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, enumCharNames, arginfo_class_IntlChar_enumCharNames, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, enumCharTypes, arginfo_class_IntlChar_enumCharTypes, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, foldCase, arginfo_class_IntlChar_foldCase, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, forDigit, arginfo_class_IntlChar_forDigit, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
#if U_ICU_VERSION_MAJOR_NUM >= 52
	ZEND_ME(IntlChar, getBidiPairedBracket, arginfo_class_IntlChar_getBidiPairedBracket, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
#endif
	ZEND_ME(IntlChar, getBlockCode, arginfo_class_IntlChar_getBlockCode, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, getCombiningClass, arginfo_class_IntlChar_getCombiningClass, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, getFC_NFKC_Closure, arginfo_class_IntlChar_getFC_NFKC_Closure, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, getIntPropertyMaxValue, arginfo_class_IntlChar_getIntPropertyMaxValue, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, getIntPropertyMinValue, arginfo_class_IntlChar_getIntPropertyMinValue, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, getIntPropertyValue, arginfo_class_IntlChar_getIntPropertyValue, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, getNumericValue, arginfo_class_IntlChar_getNumericValue, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, getPropertyEnum, arginfo_class_IntlChar_getPropertyEnum, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, getPropertyName, arginfo_class_IntlChar_getPropertyName, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, getPropertyValueEnum, arginfo_class_IntlChar_getPropertyValueEnum, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, getPropertyValueName, arginfo_class_IntlChar_getPropertyValueName, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, getUnicodeVersion, arginfo_class_IntlChar_getUnicodeVersion, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isalnum, arginfo_class_IntlChar_isalnum, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isalpha, arginfo_class_IntlChar_isalpha, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isbase, arginfo_class_IntlChar_isbase, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isblank, arginfo_class_IntlChar_isblank, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, iscntrl, arginfo_class_IntlChar_iscntrl, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isdefined, arginfo_class_IntlChar_isdefined, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isdigit, arginfo_class_IntlChar_isdigit, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isgraph, arginfo_class_IntlChar_isgraph, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isIDIgnorable, arginfo_class_IntlChar_isIDIgnorable, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isIDPart, arginfo_class_IntlChar_isIDPart, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isIDStart, arginfo_class_IntlChar_isIDStart, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isISOControl, arginfo_class_IntlChar_isISOControl, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isJavaIDPart, arginfo_class_IntlChar_isJavaIDPart, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isJavaIDStart, arginfo_class_IntlChar_isJavaIDStart, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isJavaSpaceChar, arginfo_class_IntlChar_isJavaSpaceChar, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, islower, arginfo_class_IntlChar_islower, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isMirrored, arginfo_class_IntlChar_isMirrored, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isprint, arginfo_class_IntlChar_isprint, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, ispunct, arginfo_class_IntlChar_ispunct, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isspace, arginfo_class_IntlChar_isspace, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, istitle, arginfo_class_IntlChar_istitle, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isUAlphabetic, arginfo_class_IntlChar_isUAlphabetic, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isULowercase, arginfo_class_IntlChar_isULowercase, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isupper, arginfo_class_IntlChar_isupper, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isUUppercase, arginfo_class_IntlChar_isUUppercase, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isUWhiteSpace, arginfo_class_IntlChar_isUWhiteSpace, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isWhitespace, arginfo_class_IntlChar_isWhitespace, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, isxdigit, arginfo_class_IntlChar_isxdigit, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, ord, arginfo_class_IntlChar_ord, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, tolower, arginfo_class_IntlChar_tolower, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, totitle, arginfo_class_IntlChar_totitle, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlChar, toupper, arginfo_class_IntlChar_toupper, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_IntlChar(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IntlChar", class_IntlChar_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	zval const_UNICODE_VERSION_value;
	zend_string *const_UNICODE_VERSION_value_str = zend_string_init(U_UNICODE_VERSION, strlen(U_UNICODE_VERSION), 1);
	ZVAL_STR(&const_UNICODE_VERSION_value, const_UNICODE_VERSION_value_str);
	zend_string *const_UNICODE_VERSION_name = zend_string_init_interned("UNICODE_VERSION", sizeof("UNICODE_VERSION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_UNICODE_VERSION_name, &const_UNICODE_VERSION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(const_UNICODE_VERSION_name);

	zval const_CODEPOINT_MIN_value;
	ZVAL_LONG(&const_CODEPOINT_MIN_value, UCHAR_MIN_VALUE);
	zend_string *const_CODEPOINT_MIN_name = zend_string_init_interned("CODEPOINT_MIN", sizeof("CODEPOINT_MIN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CODEPOINT_MIN_name, &const_CODEPOINT_MIN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CODEPOINT_MIN_name);

	zval const_CODEPOINT_MAX_value;
	ZVAL_LONG(&const_CODEPOINT_MAX_value, UCHAR_MAX_VALUE);
	zend_string *const_CODEPOINT_MAX_name = zend_string_init_interned("CODEPOINT_MAX", sizeof("CODEPOINT_MAX") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CODEPOINT_MAX_name, &const_CODEPOINT_MAX_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CODEPOINT_MAX_name);

	zval const_NO_NUMERIC_VALUE_value;
	ZVAL_DOUBLE(&const_NO_NUMERIC_VALUE_value, U_NO_NUMERIC_VALUE);
	zend_string *const_NO_NUMERIC_VALUE_name = zend_string_init_interned("NO_NUMERIC_VALUE", sizeof("NO_NUMERIC_VALUE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_NO_NUMERIC_VALUE_name, &const_NO_NUMERIC_VALUE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_DOUBLE));
	zend_string_release(const_NO_NUMERIC_VALUE_name);

	zval const_PROPERTY_ALPHABETIC_value;
	ZVAL_LONG(&const_PROPERTY_ALPHABETIC_value, UCHAR_ALPHABETIC);
	zend_string *const_PROPERTY_ALPHABETIC_name = zend_string_init_interned("PROPERTY_ALPHABETIC", sizeof("PROPERTY_ALPHABETIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_ALPHABETIC_name, &const_PROPERTY_ALPHABETIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_ALPHABETIC_name);

	zval const_PROPERTY_BINARY_START_value;
	ZVAL_LONG(&const_PROPERTY_BINARY_START_value, UCHAR_BINARY_START);
	zend_string *const_PROPERTY_BINARY_START_name = zend_string_init_interned("PROPERTY_BINARY_START", sizeof("PROPERTY_BINARY_START") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_BINARY_START_name, &const_PROPERTY_BINARY_START_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_BINARY_START_name);

	zval const_PROPERTY_ASCII_HEX_DIGIT_value;
	ZVAL_LONG(&const_PROPERTY_ASCII_HEX_DIGIT_value, UCHAR_ASCII_HEX_DIGIT);
	zend_string *const_PROPERTY_ASCII_HEX_DIGIT_name = zend_string_init_interned("PROPERTY_ASCII_HEX_DIGIT", sizeof("PROPERTY_ASCII_HEX_DIGIT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_ASCII_HEX_DIGIT_name, &const_PROPERTY_ASCII_HEX_DIGIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_ASCII_HEX_DIGIT_name);

	zval const_PROPERTY_BIDI_CONTROL_value;
	ZVAL_LONG(&const_PROPERTY_BIDI_CONTROL_value, UCHAR_BIDI_CONTROL);
	zend_string *const_PROPERTY_BIDI_CONTROL_name = zend_string_init_interned("PROPERTY_BIDI_CONTROL", sizeof("PROPERTY_BIDI_CONTROL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_BIDI_CONTROL_name, &const_PROPERTY_BIDI_CONTROL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_BIDI_CONTROL_name);

	zval const_PROPERTY_BIDI_MIRRORED_value;
	ZVAL_LONG(&const_PROPERTY_BIDI_MIRRORED_value, UCHAR_BIDI_MIRRORED);
	zend_string *const_PROPERTY_BIDI_MIRRORED_name = zend_string_init_interned("PROPERTY_BIDI_MIRRORED", sizeof("PROPERTY_BIDI_MIRRORED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_BIDI_MIRRORED_name, &const_PROPERTY_BIDI_MIRRORED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_BIDI_MIRRORED_name);

	zval const_PROPERTY_DASH_value;
	ZVAL_LONG(&const_PROPERTY_DASH_value, UCHAR_DASH);
	zend_string *const_PROPERTY_DASH_name = zend_string_init_interned("PROPERTY_DASH", sizeof("PROPERTY_DASH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_DASH_name, &const_PROPERTY_DASH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_DASH_name);

	zval const_PROPERTY_DEFAULT_IGNORABLE_CODE_POINT_value;
	ZVAL_LONG(&const_PROPERTY_DEFAULT_IGNORABLE_CODE_POINT_value, UCHAR_DEFAULT_IGNORABLE_CODE_POINT);
	zend_string *const_PROPERTY_DEFAULT_IGNORABLE_CODE_POINT_name = zend_string_init_interned("PROPERTY_DEFAULT_IGNORABLE_CODE_POINT", sizeof("PROPERTY_DEFAULT_IGNORABLE_CODE_POINT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_DEFAULT_IGNORABLE_CODE_POINT_name, &const_PROPERTY_DEFAULT_IGNORABLE_CODE_POINT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_DEFAULT_IGNORABLE_CODE_POINT_name);

	zval const_PROPERTY_DEPRECATED_value;
	ZVAL_LONG(&const_PROPERTY_DEPRECATED_value, UCHAR_DEPRECATED);
	zend_string *const_PROPERTY_DEPRECATED_name = zend_string_init_interned("PROPERTY_DEPRECATED", sizeof("PROPERTY_DEPRECATED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_DEPRECATED_name, &const_PROPERTY_DEPRECATED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_DEPRECATED_name);

	zval const_PROPERTY_DIACRITIC_value;
	ZVAL_LONG(&const_PROPERTY_DIACRITIC_value, UCHAR_DIACRITIC);
	zend_string *const_PROPERTY_DIACRITIC_name = zend_string_init_interned("PROPERTY_DIACRITIC", sizeof("PROPERTY_DIACRITIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_DIACRITIC_name, &const_PROPERTY_DIACRITIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_DIACRITIC_name);

	zval const_PROPERTY_EXTENDER_value;
	ZVAL_LONG(&const_PROPERTY_EXTENDER_value, UCHAR_EXTENDER);
	zend_string *const_PROPERTY_EXTENDER_name = zend_string_init_interned("PROPERTY_EXTENDER", sizeof("PROPERTY_EXTENDER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_EXTENDER_name, &const_PROPERTY_EXTENDER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_EXTENDER_name);

	zval const_PROPERTY_FULL_COMPOSITION_EXCLUSION_value;
	ZVAL_LONG(&const_PROPERTY_FULL_COMPOSITION_EXCLUSION_value, UCHAR_FULL_COMPOSITION_EXCLUSION);
	zend_string *const_PROPERTY_FULL_COMPOSITION_EXCLUSION_name = zend_string_init_interned("PROPERTY_FULL_COMPOSITION_EXCLUSION", sizeof("PROPERTY_FULL_COMPOSITION_EXCLUSION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_FULL_COMPOSITION_EXCLUSION_name, &const_PROPERTY_FULL_COMPOSITION_EXCLUSION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_FULL_COMPOSITION_EXCLUSION_name);

	zval const_PROPERTY_GRAPHEME_BASE_value;
	ZVAL_LONG(&const_PROPERTY_GRAPHEME_BASE_value, UCHAR_GRAPHEME_BASE);
	zend_string *const_PROPERTY_GRAPHEME_BASE_name = zend_string_init_interned("PROPERTY_GRAPHEME_BASE", sizeof("PROPERTY_GRAPHEME_BASE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_GRAPHEME_BASE_name, &const_PROPERTY_GRAPHEME_BASE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_GRAPHEME_BASE_name);

	zval const_PROPERTY_GRAPHEME_EXTEND_value;
	ZVAL_LONG(&const_PROPERTY_GRAPHEME_EXTEND_value, UCHAR_GRAPHEME_EXTEND);
	zend_string *const_PROPERTY_GRAPHEME_EXTEND_name = zend_string_init_interned("PROPERTY_GRAPHEME_EXTEND", sizeof("PROPERTY_GRAPHEME_EXTEND") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_GRAPHEME_EXTEND_name, &const_PROPERTY_GRAPHEME_EXTEND_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_GRAPHEME_EXTEND_name);

	zval const_PROPERTY_GRAPHEME_LINK_value;
	ZVAL_LONG(&const_PROPERTY_GRAPHEME_LINK_value, UCHAR_GRAPHEME_LINK);
	zend_string *const_PROPERTY_GRAPHEME_LINK_name = zend_string_init_interned("PROPERTY_GRAPHEME_LINK", sizeof("PROPERTY_GRAPHEME_LINK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_GRAPHEME_LINK_name, &const_PROPERTY_GRAPHEME_LINK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_GRAPHEME_LINK_name);

	zval const_PROPERTY_HEX_DIGIT_value;
	ZVAL_LONG(&const_PROPERTY_HEX_DIGIT_value, UCHAR_HEX_DIGIT);
	zend_string *const_PROPERTY_HEX_DIGIT_name = zend_string_init_interned("PROPERTY_HEX_DIGIT", sizeof("PROPERTY_HEX_DIGIT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_HEX_DIGIT_name, &const_PROPERTY_HEX_DIGIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_HEX_DIGIT_name);

	zval const_PROPERTY_HYPHEN_value;
	ZVAL_LONG(&const_PROPERTY_HYPHEN_value, UCHAR_HYPHEN);
	zend_string *const_PROPERTY_HYPHEN_name = zend_string_init_interned("PROPERTY_HYPHEN", sizeof("PROPERTY_HYPHEN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_HYPHEN_name, &const_PROPERTY_HYPHEN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_HYPHEN_name);

	zval const_PROPERTY_ID_CONTINUE_value;
	ZVAL_LONG(&const_PROPERTY_ID_CONTINUE_value, UCHAR_ID_CONTINUE);
	zend_string *const_PROPERTY_ID_CONTINUE_name = zend_string_init_interned("PROPERTY_ID_CONTINUE", sizeof("PROPERTY_ID_CONTINUE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_ID_CONTINUE_name, &const_PROPERTY_ID_CONTINUE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_ID_CONTINUE_name);

	zval const_PROPERTY_ID_START_value;
	ZVAL_LONG(&const_PROPERTY_ID_START_value, UCHAR_ID_START);
	zend_string *const_PROPERTY_ID_START_name = zend_string_init_interned("PROPERTY_ID_START", sizeof("PROPERTY_ID_START") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_ID_START_name, &const_PROPERTY_ID_START_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_ID_START_name);

	zval const_PROPERTY_IDEOGRAPHIC_value;
	ZVAL_LONG(&const_PROPERTY_IDEOGRAPHIC_value, UCHAR_IDEOGRAPHIC);
	zend_string *const_PROPERTY_IDEOGRAPHIC_name = zend_string_init_interned("PROPERTY_IDEOGRAPHIC", sizeof("PROPERTY_IDEOGRAPHIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_IDEOGRAPHIC_name, &const_PROPERTY_IDEOGRAPHIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_IDEOGRAPHIC_name);

	zval const_PROPERTY_IDS_BINARY_OPERATOR_value;
	ZVAL_LONG(&const_PROPERTY_IDS_BINARY_OPERATOR_value, UCHAR_IDS_BINARY_OPERATOR);
	zend_string *const_PROPERTY_IDS_BINARY_OPERATOR_name = zend_string_init_interned("PROPERTY_IDS_BINARY_OPERATOR", sizeof("PROPERTY_IDS_BINARY_OPERATOR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_IDS_BINARY_OPERATOR_name, &const_PROPERTY_IDS_BINARY_OPERATOR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_IDS_BINARY_OPERATOR_name);

	zval const_PROPERTY_IDS_TRINARY_OPERATOR_value;
	ZVAL_LONG(&const_PROPERTY_IDS_TRINARY_OPERATOR_value, UCHAR_IDS_TRINARY_OPERATOR);
	zend_string *const_PROPERTY_IDS_TRINARY_OPERATOR_name = zend_string_init_interned("PROPERTY_IDS_TRINARY_OPERATOR", sizeof("PROPERTY_IDS_TRINARY_OPERATOR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_IDS_TRINARY_OPERATOR_name, &const_PROPERTY_IDS_TRINARY_OPERATOR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_IDS_TRINARY_OPERATOR_name);

	zval const_PROPERTY_JOIN_CONTROL_value;
	ZVAL_LONG(&const_PROPERTY_JOIN_CONTROL_value, UCHAR_JOIN_CONTROL);
	zend_string *const_PROPERTY_JOIN_CONTROL_name = zend_string_init_interned("PROPERTY_JOIN_CONTROL", sizeof("PROPERTY_JOIN_CONTROL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_JOIN_CONTROL_name, &const_PROPERTY_JOIN_CONTROL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_JOIN_CONTROL_name);

	zval const_PROPERTY_LOGICAL_ORDER_EXCEPTION_value;
	ZVAL_LONG(&const_PROPERTY_LOGICAL_ORDER_EXCEPTION_value, UCHAR_LOGICAL_ORDER_EXCEPTION);
	zend_string *const_PROPERTY_LOGICAL_ORDER_EXCEPTION_name = zend_string_init_interned("PROPERTY_LOGICAL_ORDER_EXCEPTION", sizeof("PROPERTY_LOGICAL_ORDER_EXCEPTION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_LOGICAL_ORDER_EXCEPTION_name, &const_PROPERTY_LOGICAL_ORDER_EXCEPTION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_LOGICAL_ORDER_EXCEPTION_name);

	zval const_PROPERTY_LOWERCASE_value;
	ZVAL_LONG(&const_PROPERTY_LOWERCASE_value, UCHAR_LOWERCASE);
	zend_string *const_PROPERTY_LOWERCASE_name = zend_string_init_interned("PROPERTY_LOWERCASE", sizeof("PROPERTY_LOWERCASE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_LOWERCASE_name, &const_PROPERTY_LOWERCASE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_LOWERCASE_name);

	zval const_PROPERTY_MATH_value;
	ZVAL_LONG(&const_PROPERTY_MATH_value, UCHAR_MATH);
	zend_string *const_PROPERTY_MATH_name = zend_string_init_interned("PROPERTY_MATH", sizeof("PROPERTY_MATH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_MATH_name, &const_PROPERTY_MATH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_MATH_name);

	zval const_PROPERTY_NONCHARACTER_CODE_POINT_value;
	ZVAL_LONG(&const_PROPERTY_NONCHARACTER_CODE_POINT_value, UCHAR_NONCHARACTER_CODE_POINT);
	zend_string *const_PROPERTY_NONCHARACTER_CODE_POINT_name = zend_string_init_interned("PROPERTY_NONCHARACTER_CODE_POINT", sizeof("PROPERTY_NONCHARACTER_CODE_POINT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_NONCHARACTER_CODE_POINT_name, &const_PROPERTY_NONCHARACTER_CODE_POINT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_NONCHARACTER_CODE_POINT_name);

	zval const_PROPERTY_QUOTATION_MARK_value;
	ZVAL_LONG(&const_PROPERTY_QUOTATION_MARK_value, UCHAR_QUOTATION_MARK);
	zend_string *const_PROPERTY_QUOTATION_MARK_name = zend_string_init_interned("PROPERTY_QUOTATION_MARK", sizeof("PROPERTY_QUOTATION_MARK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_QUOTATION_MARK_name, &const_PROPERTY_QUOTATION_MARK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_QUOTATION_MARK_name);

	zval const_PROPERTY_RADICAL_value;
	ZVAL_LONG(&const_PROPERTY_RADICAL_value, UCHAR_RADICAL);
	zend_string *const_PROPERTY_RADICAL_name = zend_string_init_interned("PROPERTY_RADICAL", sizeof("PROPERTY_RADICAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_RADICAL_name, &const_PROPERTY_RADICAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_RADICAL_name);

	zval const_PROPERTY_SOFT_DOTTED_value;
	ZVAL_LONG(&const_PROPERTY_SOFT_DOTTED_value, UCHAR_SOFT_DOTTED);
	zend_string *const_PROPERTY_SOFT_DOTTED_name = zend_string_init_interned("PROPERTY_SOFT_DOTTED", sizeof("PROPERTY_SOFT_DOTTED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_SOFT_DOTTED_name, &const_PROPERTY_SOFT_DOTTED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_SOFT_DOTTED_name);

	zval const_PROPERTY_TERMINAL_PUNCTUATION_value;
	ZVAL_LONG(&const_PROPERTY_TERMINAL_PUNCTUATION_value, UCHAR_TERMINAL_PUNCTUATION);
	zend_string *const_PROPERTY_TERMINAL_PUNCTUATION_name = zend_string_init_interned("PROPERTY_TERMINAL_PUNCTUATION", sizeof("PROPERTY_TERMINAL_PUNCTUATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_TERMINAL_PUNCTUATION_name, &const_PROPERTY_TERMINAL_PUNCTUATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_TERMINAL_PUNCTUATION_name);

	zval const_PROPERTY_UNIFIED_IDEOGRAPH_value;
	ZVAL_LONG(&const_PROPERTY_UNIFIED_IDEOGRAPH_value, UCHAR_UNIFIED_IDEOGRAPH);
	zend_string *const_PROPERTY_UNIFIED_IDEOGRAPH_name = zend_string_init_interned("PROPERTY_UNIFIED_IDEOGRAPH", sizeof("PROPERTY_UNIFIED_IDEOGRAPH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_UNIFIED_IDEOGRAPH_name, &const_PROPERTY_UNIFIED_IDEOGRAPH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_UNIFIED_IDEOGRAPH_name);

	zval const_PROPERTY_UPPERCASE_value;
	ZVAL_LONG(&const_PROPERTY_UPPERCASE_value, UCHAR_UPPERCASE);
	zend_string *const_PROPERTY_UPPERCASE_name = zend_string_init_interned("PROPERTY_UPPERCASE", sizeof("PROPERTY_UPPERCASE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_UPPERCASE_name, &const_PROPERTY_UPPERCASE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_UPPERCASE_name);

	zval const_PROPERTY_WHITE_SPACE_value;
	ZVAL_LONG(&const_PROPERTY_WHITE_SPACE_value, UCHAR_WHITE_SPACE);
	zend_string *const_PROPERTY_WHITE_SPACE_name = zend_string_init_interned("PROPERTY_WHITE_SPACE", sizeof("PROPERTY_WHITE_SPACE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_WHITE_SPACE_name, &const_PROPERTY_WHITE_SPACE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_WHITE_SPACE_name);

	zval const_PROPERTY_XID_CONTINUE_value;
	ZVAL_LONG(&const_PROPERTY_XID_CONTINUE_value, UCHAR_XID_CONTINUE);
	zend_string *const_PROPERTY_XID_CONTINUE_name = zend_string_init_interned("PROPERTY_XID_CONTINUE", sizeof("PROPERTY_XID_CONTINUE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_XID_CONTINUE_name, &const_PROPERTY_XID_CONTINUE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_XID_CONTINUE_name);

	zval const_PROPERTY_XID_START_value;
	ZVAL_LONG(&const_PROPERTY_XID_START_value, UCHAR_XID_START);
	zend_string *const_PROPERTY_XID_START_name = zend_string_init_interned("PROPERTY_XID_START", sizeof("PROPERTY_XID_START") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_XID_START_name, &const_PROPERTY_XID_START_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_XID_START_name);

	zval const_PROPERTY_CASE_SENSITIVE_value;
	ZVAL_LONG(&const_PROPERTY_CASE_SENSITIVE_value, UCHAR_CASE_SENSITIVE);
	zend_string *const_PROPERTY_CASE_SENSITIVE_name = zend_string_init_interned("PROPERTY_CASE_SENSITIVE", sizeof("PROPERTY_CASE_SENSITIVE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_CASE_SENSITIVE_name, &const_PROPERTY_CASE_SENSITIVE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_CASE_SENSITIVE_name);

	zval const_PROPERTY_S_TERM_value;
	ZVAL_LONG(&const_PROPERTY_S_TERM_value, UCHAR_S_TERM);
	zend_string *const_PROPERTY_S_TERM_name = zend_string_init_interned("PROPERTY_S_TERM", sizeof("PROPERTY_S_TERM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_S_TERM_name, &const_PROPERTY_S_TERM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_S_TERM_name);

	zval const_PROPERTY_VARIATION_SELECTOR_value;
	ZVAL_LONG(&const_PROPERTY_VARIATION_SELECTOR_value, UCHAR_VARIATION_SELECTOR);
	zend_string *const_PROPERTY_VARIATION_SELECTOR_name = zend_string_init_interned("PROPERTY_VARIATION_SELECTOR", sizeof("PROPERTY_VARIATION_SELECTOR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_VARIATION_SELECTOR_name, &const_PROPERTY_VARIATION_SELECTOR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_VARIATION_SELECTOR_name);

	zval const_PROPERTY_NFD_INERT_value;
	ZVAL_LONG(&const_PROPERTY_NFD_INERT_value, UCHAR_NFD_INERT);
	zend_string *const_PROPERTY_NFD_INERT_name = zend_string_init_interned("PROPERTY_NFD_INERT", sizeof("PROPERTY_NFD_INERT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_NFD_INERT_name, &const_PROPERTY_NFD_INERT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_NFD_INERT_name);

	zval const_PROPERTY_NFKD_INERT_value;
	ZVAL_LONG(&const_PROPERTY_NFKD_INERT_value, UCHAR_NFKD_INERT);
	zend_string *const_PROPERTY_NFKD_INERT_name = zend_string_init_interned("PROPERTY_NFKD_INERT", sizeof("PROPERTY_NFKD_INERT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_NFKD_INERT_name, &const_PROPERTY_NFKD_INERT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_NFKD_INERT_name);

	zval const_PROPERTY_NFC_INERT_value;
	ZVAL_LONG(&const_PROPERTY_NFC_INERT_value, UCHAR_NFC_INERT);
	zend_string *const_PROPERTY_NFC_INERT_name = zend_string_init_interned("PROPERTY_NFC_INERT", sizeof("PROPERTY_NFC_INERT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_NFC_INERT_name, &const_PROPERTY_NFC_INERT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_NFC_INERT_name);

	zval const_PROPERTY_NFKC_INERT_value;
	ZVAL_LONG(&const_PROPERTY_NFKC_INERT_value, UCHAR_NFKC_INERT);
	zend_string *const_PROPERTY_NFKC_INERT_name = zend_string_init_interned("PROPERTY_NFKC_INERT", sizeof("PROPERTY_NFKC_INERT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_NFKC_INERT_name, &const_PROPERTY_NFKC_INERT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_NFKC_INERT_name);

	zval const_PROPERTY_SEGMENT_STARTER_value;
	ZVAL_LONG(&const_PROPERTY_SEGMENT_STARTER_value, UCHAR_SEGMENT_STARTER);
	zend_string *const_PROPERTY_SEGMENT_STARTER_name = zend_string_init_interned("PROPERTY_SEGMENT_STARTER", sizeof("PROPERTY_SEGMENT_STARTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_SEGMENT_STARTER_name, &const_PROPERTY_SEGMENT_STARTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_SEGMENT_STARTER_name);

	zval const_PROPERTY_PATTERN_SYNTAX_value;
	ZVAL_LONG(&const_PROPERTY_PATTERN_SYNTAX_value, UCHAR_PATTERN_SYNTAX);
	zend_string *const_PROPERTY_PATTERN_SYNTAX_name = zend_string_init_interned("PROPERTY_PATTERN_SYNTAX", sizeof("PROPERTY_PATTERN_SYNTAX") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_PATTERN_SYNTAX_name, &const_PROPERTY_PATTERN_SYNTAX_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_PATTERN_SYNTAX_name);

	zval const_PROPERTY_PATTERN_WHITE_SPACE_value;
	ZVAL_LONG(&const_PROPERTY_PATTERN_WHITE_SPACE_value, UCHAR_PATTERN_WHITE_SPACE);
	zend_string *const_PROPERTY_PATTERN_WHITE_SPACE_name = zend_string_init_interned("PROPERTY_PATTERN_WHITE_SPACE", sizeof("PROPERTY_PATTERN_WHITE_SPACE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_PATTERN_WHITE_SPACE_name, &const_PROPERTY_PATTERN_WHITE_SPACE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_PATTERN_WHITE_SPACE_name);

	zval const_PROPERTY_POSIX_ALNUM_value;
	ZVAL_LONG(&const_PROPERTY_POSIX_ALNUM_value, UCHAR_POSIX_ALNUM);
	zend_string *const_PROPERTY_POSIX_ALNUM_name = zend_string_init_interned("PROPERTY_POSIX_ALNUM", sizeof("PROPERTY_POSIX_ALNUM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_POSIX_ALNUM_name, &const_PROPERTY_POSIX_ALNUM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_POSIX_ALNUM_name);

	zval const_PROPERTY_POSIX_BLANK_value;
	ZVAL_LONG(&const_PROPERTY_POSIX_BLANK_value, UCHAR_POSIX_BLANK);
	zend_string *const_PROPERTY_POSIX_BLANK_name = zend_string_init_interned("PROPERTY_POSIX_BLANK", sizeof("PROPERTY_POSIX_BLANK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_POSIX_BLANK_name, &const_PROPERTY_POSIX_BLANK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_POSIX_BLANK_name);

	zval const_PROPERTY_POSIX_GRAPH_value;
	ZVAL_LONG(&const_PROPERTY_POSIX_GRAPH_value, UCHAR_POSIX_GRAPH);
	zend_string *const_PROPERTY_POSIX_GRAPH_name = zend_string_init_interned("PROPERTY_POSIX_GRAPH", sizeof("PROPERTY_POSIX_GRAPH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_POSIX_GRAPH_name, &const_PROPERTY_POSIX_GRAPH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_POSIX_GRAPH_name);

	zval const_PROPERTY_POSIX_PRINT_value;
	ZVAL_LONG(&const_PROPERTY_POSIX_PRINT_value, UCHAR_POSIX_PRINT);
	zend_string *const_PROPERTY_POSIX_PRINT_name = zend_string_init_interned("PROPERTY_POSIX_PRINT", sizeof("PROPERTY_POSIX_PRINT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_POSIX_PRINT_name, &const_PROPERTY_POSIX_PRINT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_POSIX_PRINT_name);

	zval const_PROPERTY_POSIX_XDIGIT_value;
	ZVAL_LONG(&const_PROPERTY_POSIX_XDIGIT_value, UCHAR_POSIX_XDIGIT);
	zend_string *const_PROPERTY_POSIX_XDIGIT_name = zend_string_init_interned("PROPERTY_POSIX_XDIGIT", sizeof("PROPERTY_POSIX_XDIGIT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_POSIX_XDIGIT_name, &const_PROPERTY_POSIX_XDIGIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_POSIX_XDIGIT_name);

	zval const_PROPERTY_CASED_value;
	ZVAL_LONG(&const_PROPERTY_CASED_value, UCHAR_CASED);
	zend_string *const_PROPERTY_CASED_name = zend_string_init_interned("PROPERTY_CASED", sizeof("PROPERTY_CASED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_CASED_name, &const_PROPERTY_CASED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_CASED_name);

	zval const_PROPERTY_CASE_IGNORABLE_value;
	ZVAL_LONG(&const_PROPERTY_CASE_IGNORABLE_value, UCHAR_CASE_IGNORABLE);
	zend_string *const_PROPERTY_CASE_IGNORABLE_name = zend_string_init_interned("PROPERTY_CASE_IGNORABLE", sizeof("PROPERTY_CASE_IGNORABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_CASE_IGNORABLE_name, &const_PROPERTY_CASE_IGNORABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_CASE_IGNORABLE_name);

	zval const_PROPERTY_CHANGES_WHEN_LOWERCASED_value;
	ZVAL_LONG(&const_PROPERTY_CHANGES_WHEN_LOWERCASED_value, UCHAR_CHANGES_WHEN_LOWERCASED);
	zend_string *const_PROPERTY_CHANGES_WHEN_LOWERCASED_name = zend_string_init_interned("PROPERTY_CHANGES_WHEN_LOWERCASED", sizeof("PROPERTY_CHANGES_WHEN_LOWERCASED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_CHANGES_WHEN_LOWERCASED_name, &const_PROPERTY_CHANGES_WHEN_LOWERCASED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_CHANGES_WHEN_LOWERCASED_name);

	zval const_PROPERTY_CHANGES_WHEN_UPPERCASED_value;
	ZVAL_LONG(&const_PROPERTY_CHANGES_WHEN_UPPERCASED_value, UCHAR_CHANGES_WHEN_UPPERCASED);
	zend_string *const_PROPERTY_CHANGES_WHEN_UPPERCASED_name = zend_string_init_interned("PROPERTY_CHANGES_WHEN_UPPERCASED", sizeof("PROPERTY_CHANGES_WHEN_UPPERCASED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_CHANGES_WHEN_UPPERCASED_name, &const_PROPERTY_CHANGES_WHEN_UPPERCASED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_CHANGES_WHEN_UPPERCASED_name);

	zval const_PROPERTY_CHANGES_WHEN_TITLECASED_value;
	ZVAL_LONG(&const_PROPERTY_CHANGES_WHEN_TITLECASED_value, UCHAR_CHANGES_WHEN_TITLECASED);
	zend_string *const_PROPERTY_CHANGES_WHEN_TITLECASED_name = zend_string_init_interned("PROPERTY_CHANGES_WHEN_TITLECASED", sizeof("PROPERTY_CHANGES_WHEN_TITLECASED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_CHANGES_WHEN_TITLECASED_name, &const_PROPERTY_CHANGES_WHEN_TITLECASED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_CHANGES_WHEN_TITLECASED_name);

	zval const_PROPERTY_CHANGES_WHEN_CASEFOLDED_value;
	ZVAL_LONG(&const_PROPERTY_CHANGES_WHEN_CASEFOLDED_value, UCHAR_CHANGES_WHEN_CASEFOLDED);
	zend_string *const_PROPERTY_CHANGES_WHEN_CASEFOLDED_name = zend_string_init_interned("PROPERTY_CHANGES_WHEN_CASEFOLDED", sizeof("PROPERTY_CHANGES_WHEN_CASEFOLDED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_CHANGES_WHEN_CASEFOLDED_name, &const_PROPERTY_CHANGES_WHEN_CASEFOLDED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_CHANGES_WHEN_CASEFOLDED_name);

	zval const_PROPERTY_CHANGES_WHEN_CASEMAPPED_value;
	ZVAL_LONG(&const_PROPERTY_CHANGES_WHEN_CASEMAPPED_value, UCHAR_CHANGES_WHEN_CASEMAPPED);
	zend_string *const_PROPERTY_CHANGES_WHEN_CASEMAPPED_name = zend_string_init_interned("PROPERTY_CHANGES_WHEN_CASEMAPPED", sizeof("PROPERTY_CHANGES_WHEN_CASEMAPPED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_CHANGES_WHEN_CASEMAPPED_name, &const_PROPERTY_CHANGES_WHEN_CASEMAPPED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_CHANGES_WHEN_CASEMAPPED_name);

	zval const_PROPERTY_CHANGES_WHEN_NFKC_CASEFOLDED_value;
	ZVAL_LONG(&const_PROPERTY_CHANGES_WHEN_NFKC_CASEFOLDED_value, UCHAR_CHANGES_WHEN_NFKC_CASEFOLDED);
	zend_string *const_PROPERTY_CHANGES_WHEN_NFKC_CASEFOLDED_name = zend_string_init_interned("PROPERTY_CHANGES_WHEN_NFKC_CASEFOLDED", sizeof("PROPERTY_CHANGES_WHEN_NFKC_CASEFOLDED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_CHANGES_WHEN_NFKC_CASEFOLDED_name, &const_PROPERTY_CHANGES_WHEN_NFKC_CASEFOLDED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_CHANGES_WHEN_NFKC_CASEFOLDED_name);

	zval const_PROPERTY_BINARY_LIMIT_value;
	ZVAL_LONG(&const_PROPERTY_BINARY_LIMIT_value, UCHAR_BINARY_LIMIT);
	zend_string *const_PROPERTY_BINARY_LIMIT_name = zend_string_init_interned("PROPERTY_BINARY_LIMIT", sizeof("PROPERTY_BINARY_LIMIT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_BINARY_LIMIT_name, &const_PROPERTY_BINARY_LIMIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_BINARY_LIMIT_name);

	zval const_PROPERTY_BIDI_CLASS_value;
	ZVAL_LONG(&const_PROPERTY_BIDI_CLASS_value, UCHAR_BIDI_CLASS);
	zend_string *const_PROPERTY_BIDI_CLASS_name = zend_string_init_interned("PROPERTY_BIDI_CLASS", sizeof("PROPERTY_BIDI_CLASS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_BIDI_CLASS_name, &const_PROPERTY_BIDI_CLASS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_BIDI_CLASS_name);

	zval const_PROPERTY_INT_START_value;
	ZVAL_LONG(&const_PROPERTY_INT_START_value, UCHAR_INT_START);
	zend_string *const_PROPERTY_INT_START_name = zend_string_init_interned("PROPERTY_INT_START", sizeof("PROPERTY_INT_START") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_INT_START_name, &const_PROPERTY_INT_START_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_INT_START_name);

	zval const_PROPERTY_BLOCK_value;
	ZVAL_LONG(&const_PROPERTY_BLOCK_value, UCHAR_BLOCK);
	zend_string *const_PROPERTY_BLOCK_name = zend_string_init_interned("PROPERTY_BLOCK", sizeof("PROPERTY_BLOCK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_BLOCK_name, &const_PROPERTY_BLOCK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_BLOCK_name);

	zval const_PROPERTY_CANONICAL_COMBINING_CLASS_value;
	ZVAL_LONG(&const_PROPERTY_CANONICAL_COMBINING_CLASS_value, UCHAR_CANONICAL_COMBINING_CLASS);
	zend_string *const_PROPERTY_CANONICAL_COMBINING_CLASS_name = zend_string_init_interned("PROPERTY_CANONICAL_COMBINING_CLASS", sizeof("PROPERTY_CANONICAL_COMBINING_CLASS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_CANONICAL_COMBINING_CLASS_name, &const_PROPERTY_CANONICAL_COMBINING_CLASS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_CANONICAL_COMBINING_CLASS_name);

	zval const_PROPERTY_DECOMPOSITION_TYPE_value;
	ZVAL_LONG(&const_PROPERTY_DECOMPOSITION_TYPE_value, UCHAR_DECOMPOSITION_TYPE);
	zend_string *const_PROPERTY_DECOMPOSITION_TYPE_name = zend_string_init_interned("PROPERTY_DECOMPOSITION_TYPE", sizeof("PROPERTY_DECOMPOSITION_TYPE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_DECOMPOSITION_TYPE_name, &const_PROPERTY_DECOMPOSITION_TYPE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_DECOMPOSITION_TYPE_name);

	zval const_PROPERTY_EAST_ASIAN_WIDTH_value;
	ZVAL_LONG(&const_PROPERTY_EAST_ASIAN_WIDTH_value, UCHAR_EAST_ASIAN_WIDTH);
	zend_string *const_PROPERTY_EAST_ASIAN_WIDTH_name = zend_string_init_interned("PROPERTY_EAST_ASIAN_WIDTH", sizeof("PROPERTY_EAST_ASIAN_WIDTH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_EAST_ASIAN_WIDTH_name, &const_PROPERTY_EAST_ASIAN_WIDTH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_EAST_ASIAN_WIDTH_name);

	zval const_PROPERTY_GENERAL_CATEGORY_value;
	ZVAL_LONG(&const_PROPERTY_GENERAL_CATEGORY_value, UCHAR_GENERAL_CATEGORY);
	zend_string *const_PROPERTY_GENERAL_CATEGORY_name = zend_string_init_interned("PROPERTY_GENERAL_CATEGORY", sizeof("PROPERTY_GENERAL_CATEGORY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_GENERAL_CATEGORY_name, &const_PROPERTY_GENERAL_CATEGORY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_GENERAL_CATEGORY_name);

	zval const_PROPERTY_JOINING_GROUP_value;
	ZVAL_LONG(&const_PROPERTY_JOINING_GROUP_value, UCHAR_JOINING_GROUP);
	zend_string *const_PROPERTY_JOINING_GROUP_name = zend_string_init_interned("PROPERTY_JOINING_GROUP", sizeof("PROPERTY_JOINING_GROUP") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_JOINING_GROUP_name, &const_PROPERTY_JOINING_GROUP_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_JOINING_GROUP_name);

	zval const_PROPERTY_JOINING_TYPE_value;
	ZVAL_LONG(&const_PROPERTY_JOINING_TYPE_value, UCHAR_JOINING_TYPE);
	zend_string *const_PROPERTY_JOINING_TYPE_name = zend_string_init_interned("PROPERTY_JOINING_TYPE", sizeof("PROPERTY_JOINING_TYPE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_JOINING_TYPE_name, &const_PROPERTY_JOINING_TYPE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_JOINING_TYPE_name);

	zval const_PROPERTY_LINE_BREAK_value;
	ZVAL_LONG(&const_PROPERTY_LINE_BREAK_value, UCHAR_LINE_BREAK);
	zend_string *const_PROPERTY_LINE_BREAK_name = zend_string_init_interned("PROPERTY_LINE_BREAK", sizeof("PROPERTY_LINE_BREAK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_LINE_BREAK_name, &const_PROPERTY_LINE_BREAK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_LINE_BREAK_name);

	zval const_PROPERTY_NUMERIC_TYPE_value;
	ZVAL_LONG(&const_PROPERTY_NUMERIC_TYPE_value, UCHAR_NUMERIC_TYPE);
	zend_string *const_PROPERTY_NUMERIC_TYPE_name = zend_string_init_interned("PROPERTY_NUMERIC_TYPE", sizeof("PROPERTY_NUMERIC_TYPE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_NUMERIC_TYPE_name, &const_PROPERTY_NUMERIC_TYPE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_NUMERIC_TYPE_name);

	zval const_PROPERTY_SCRIPT_value;
	ZVAL_LONG(&const_PROPERTY_SCRIPT_value, UCHAR_SCRIPT);
	zend_string *const_PROPERTY_SCRIPT_name = zend_string_init_interned("PROPERTY_SCRIPT", sizeof("PROPERTY_SCRIPT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_SCRIPT_name, &const_PROPERTY_SCRIPT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_SCRIPT_name);

	zval const_PROPERTY_HANGUL_SYLLABLE_TYPE_value;
	ZVAL_LONG(&const_PROPERTY_HANGUL_SYLLABLE_TYPE_value, UCHAR_HANGUL_SYLLABLE_TYPE);
	zend_string *const_PROPERTY_HANGUL_SYLLABLE_TYPE_name = zend_string_init_interned("PROPERTY_HANGUL_SYLLABLE_TYPE", sizeof("PROPERTY_HANGUL_SYLLABLE_TYPE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_HANGUL_SYLLABLE_TYPE_name, &const_PROPERTY_HANGUL_SYLLABLE_TYPE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_HANGUL_SYLLABLE_TYPE_name);

	zval const_PROPERTY_NFD_QUICK_CHECK_value;
	ZVAL_LONG(&const_PROPERTY_NFD_QUICK_CHECK_value, UCHAR_NFD_QUICK_CHECK);
	zend_string *const_PROPERTY_NFD_QUICK_CHECK_name = zend_string_init_interned("PROPERTY_NFD_QUICK_CHECK", sizeof("PROPERTY_NFD_QUICK_CHECK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_NFD_QUICK_CHECK_name, &const_PROPERTY_NFD_QUICK_CHECK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_NFD_QUICK_CHECK_name);

	zval const_PROPERTY_NFKD_QUICK_CHECK_value;
	ZVAL_LONG(&const_PROPERTY_NFKD_QUICK_CHECK_value, UCHAR_NFKD_QUICK_CHECK);
	zend_string *const_PROPERTY_NFKD_QUICK_CHECK_name = zend_string_init_interned("PROPERTY_NFKD_QUICK_CHECK", sizeof("PROPERTY_NFKD_QUICK_CHECK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_NFKD_QUICK_CHECK_name, &const_PROPERTY_NFKD_QUICK_CHECK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_NFKD_QUICK_CHECK_name);

	zval const_PROPERTY_NFC_QUICK_CHECK_value;
	ZVAL_LONG(&const_PROPERTY_NFC_QUICK_CHECK_value, UCHAR_NFC_QUICK_CHECK);
	zend_string *const_PROPERTY_NFC_QUICK_CHECK_name = zend_string_init_interned("PROPERTY_NFC_QUICK_CHECK", sizeof("PROPERTY_NFC_QUICK_CHECK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_NFC_QUICK_CHECK_name, &const_PROPERTY_NFC_QUICK_CHECK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_NFC_QUICK_CHECK_name);

	zval const_PROPERTY_NFKC_QUICK_CHECK_value;
	ZVAL_LONG(&const_PROPERTY_NFKC_QUICK_CHECK_value, UCHAR_NFKC_QUICK_CHECK);
	zend_string *const_PROPERTY_NFKC_QUICK_CHECK_name = zend_string_init_interned("PROPERTY_NFKC_QUICK_CHECK", sizeof("PROPERTY_NFKC_QUICK_CHECK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_NFKC_QUICK_CHECK_name, &const_PROPERTY_NFKC_QUICK_CHECK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_NFKC_QUICK_CHECK_name);

	zval const_PROPERTY_LEAD_CANONICAL_COMBINING_CLASS_value;
	ZVAL_LONG(&const_PROPERTY_LEAD_CANONICAL_COMBINING_CLASS_value, UCHAR_LEAD_CANONICAL_COMBINING_CLASS);
	zend_string *const_PROPERTY_LEAD_CANONICAL_COMBINING_CLASS_name = zend_string_init_interned("PROPERTY_LEAD_CANONICAL_COMBINING_CLASS", sizeof("PROPERTY_LEAD_CANONICAL_COMBINING_CLASS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_LEAD_CANONICAL_COMBINING_CLASS_name, &const_PROPERTY_LEAD_CANONICAL_COMBINING_CLASS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_LEAD_CANONICAL_COMBINING_CLASS_name);

	zval const_PROPERTY_TRAIL_CANONICAL_COMBINING_CLASS_value;
	ZVAL_LONG(&const_PROPERTY_TRAIL_CANONICAL_COMBINING_CLASS_value, UCHAR_TRAIL_CANONICAL_COMBINING_CLASS);
	zend_string *const_PROPERTY_TRAIL_CANONICAL_COMBINING_CLASS_name = zend_string_init_interned("PROPERTY_TRAIL_CANONICAL_COMBINING_CLASS", sizeof("PROPERTY_TRAIL_CANONICAL_COMBINING_CLASS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_TRAIL_CANONICAL_COMBINING_CLASS_name, &const_PROPERTY_TRAIL_CANONICAL_COMBINING_CLASS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_TRAIL_CANONICAL_COMBINING_CLASS_name);

	zval const_PROPERTY_GRAPHEME_CLUSTER_BREAK_value;
	ZVAL_LONG(&const_PROPERTY_GRAPHEME_CLUSTER_BREAK_value, UCHAR_GRAPHEME_CLUSTER_BREAK);
	zend_string *const_PROPERTY_GRAPHEME_CLUSTER_BREAK_name = zend_string_init_interned("PROPERTY_GRAPHEME_CLUSTER_BREAK", sizeof("PROPERTY_GRAPHEME_CLUSTER_BREAK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_GRAPHEME_CLUSTER_BREAK_name, &const_PROPERTY_GRAPHEME_CLUSTER_BREAK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_GRAPHEME_CLUSTER_BREAK_name);

	zval const_PROPERTY_SENTENCE_BREAK_value;
	ZVAL_LONG(&const_PROPERTY_SENTENCE_BREAK_value, UCHAR_SENTENCE_BREAK);
	zend_string *const_PROPERTY_SENTENCE_BREAK_name = zend_string_init_interned("PROPERTY_SENTENCE_BREAK", sizeof("PROPERTY_SENTENCE_BREAK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_SENTENCE_BREAK_name, &const_PROPERTY_SENTENCE_BREAK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_SENTENCE_BREAK_name);

	zval const_PROPERTY_WORD_BREAK_value;
	ZVAL_LONG(&const_PROPERTY_WORD_BREAK_value, UCHAR_WORD_BREAK);
	zend_string *const_PROPERTY_WORD_BREAK_name = zend_string_init_interned("PROPERTY_WORD_BREAK", sizeof("PROPERTY_WORD_BREAK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_WORD_BREAK_name, &const_PROPERTY_WORD_BREAK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_WORD_BREAK_name);
#if U_ICU_VERSION_MAJOR_NUM >= 52

	zval const_PROPERTY_BIDI_PAIRED_BRACKET_TYPE_value;
	ZVAL_LONG(&const_PROPERTY_BIDI_PAIRED_BRACKET_TYPE_value, UCHAR_BIDI_PAIRED_BRACKET_TYPE);
	zend_string *const_PROPERTY_BIDI_PAIRED_BRACKET_TYPE_name = zend_string_init_interned("PROPERTY_BIDI_PAIRED_BRACKET_TYPE", sizeof("PROPERTY_BIDI_PAIRED_BRACKET_TYPE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_BIDI_PAIRED_BRACKET_TYPE_name, &const_PROPERTY_BIDI_PAIRED_BRACKET_TYPE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_BIDI_PAIRED_BRACKET_TYPE_name);
#endif

	zval const_PROPERTY_INT_LIMIT_value;
	ZVAL_LONG(&const_PROPERTY_INT_LIMIT_value, UCHAR_INT_LIMIT);
	zend_string *const_PROPERTY_INT_LIMIT_name = zend_string_init_interned("PROPERTY_INT_LIMIT", sizeof("PROPERTY_INT_LIMIT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_INT_LIMIT_name, &const_PROPERTY_INT_LIMIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_INT_LIMIT_name);

	zval const_PROPERTY_GENERAL_CATEGORY_MASK_value;
	ZVAL_LONG(&const_PROPERTY_GENERAL_CATEGORY_MASK_value, UCHAR_GENERAL_CATEGORY_MASK);
	zend_string *const_PROPERTY_GENERAL_CATEGORY_MASK_name = zend_string_init_interned("PROPERTY_GENERAL_CATEGORY_MASK", sizeof("PROPERTY_GENERAL_CATEGORY_MASK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_GENERAL_CATEGORY_MASK_name, &const_PROPERTY_GENERAL_CATEGORY_MASK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_GENERAL_CATEGORY_MASK_name);

	zval const_PROPERTY_MASK_START_value;
	ZVAL_LONG(&const_PROPERTY_MASK_START_value, UCHAR_MASK_START);
	zend_string *const_PROPERTY_MASK_START_name = zend_string_init_interned("PROPERTY_MASK_START", sizeof("PROPERTY_MASK_START") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_MASK_START_name, &const_PROPERTY_MASK_START_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_MASK_START_name);

	zval const_PROPERTY_MASK_LIMIT_value;
	ZVAL_LONG(&const_PROPERTY_MASK_LIMIT_value, UCHAR_MASK_LIMIT);
	zend_string *const_PROPERTY_MASK_LIMIT_name = zend_string_init_interned("PROPERTY_MASK_LIMIT", sizeof("PROPERTY_MASK_LIMIT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_MASK_LIMIT_name, &const_PROPERTY_MASK_LIMIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_MASK_LIMIT_name);

	zval const_PROPERTY_NUMERIC_VALUE_value;
	ZVAL_LONG(&const_PROPERTY_NUMERIC_VALUE_value, UCHAR_NUMERIC_VALUE);
	zend_string *const_PROPERTY_NUMERIC_VALUE_name = zend_string_init_interned("PROPERTY_NUMERIC_VALUE", sizeof("PROPERTY_NUMERIC_VALUE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_NUMERIC_VALUE_name, &const_PROPERTY_NUMERIC_VALUE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_NUMERIC_VALUE_name);

	zval const_PROPERTY_DOUBLE_START_value;
	ZVAL_LONG(&const_PROPERTY_DOUBLE_START_value, UCHAR_DOUBLE_START);
	zend_string *const_PROPERTY_DOUBLE_START_name = zend_string_init_interned("PROPERTY_DOUBLE_START", sizeof("PROPERTY_DOUBLE_START") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_DOUBLE_START_name, &const_PROPERTY_DOUBLE_START_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_DOUBLE_START_name);

	zval const_PROPERTY_DOUBLE_LIMIT_value;
	ZVAL_LONG(&const_PROPERTY_DOUBLE_LIMIT_value, UCHAR_DOUBLE_LIMIT);
	zend_string *const_PROPERTY_DOUBLE_LIMIT_name = zend_string_init_interned("PROPERTY_DOUBLE_LIMIT", sizeof("PROPERTY_DOUBLE_LIMIT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_DOUBLE_LIMIT_name, &const_PROPERTY_DOUBLE_LIMIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_DOUBLE_LIMIT_name);

	zval const_PROPERTY_AGE_value;
	ZVAL_LONG(&const_PROPERTY_AGE_value, UCHAR_AGE);
	zend_string *const_PROPERTY_AGE_name = zend_string_init_interned("PROPERTY_AGE", sizeof("PROPERTY_AGE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_AGE_name, &const_PROPERTY_AGE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_AGE_name);

	zval const_PROPERTY_STRING_START_value;
	ZVAL_LONG(&const_PROPERTY_STRING_START_value, UCHAR_STRING_START);
	zend_string *const_PROPERTY_STRING_START_name = zend_string_init_interned("PROPERTY_STRING_START", sizeof("PROPERTY_STRING_START") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_STRING_START_name, &const_PROPERTY_STRING_START_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_STRING_START_name);

	zval const_PROPERTY_BIDI_MIRRORING_GLYPH_value;
	ZVAL_LONG(&const_PROPERTY_BIDI_MIRRORING_GLYPH_value, UCHAR_BIDI_MIRRORING_GLYPH);
	zend_string *const_PROPERTY_BIDI_MIRRORING_GLYPH_name = zend_string_init_interned("PROPERTY_BIDI_MIRRORING_GLYPH", sizeof("PROPERTY_BIDI_MIRRORING_GLYPH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_BIDI_MIRRORING_GLYPH_name, &const_PROPERTY_BIDI_MIRRORING_GLYPH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_BIDI_MIRRORING_GLYPH_name);

	zval const_PROPERTY_CASE_FOLDING_value;
	ZVAL_LONG(&const_PROPERTY_CASE_FOLDING_value, UCHAR_CASE_FOLDING);
	zend_string *const_PROPERTY_CASE_FOLDING_name = zend_string_init_interned("PROPERTY_CASE_FOLDING", sizeof("PROPERTY_CASE_FOLDING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_CASE_FOLDING_name, &const_PROPERTY_CASE_FOLDING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_CASE_FOLDING_name);

	zval const_PROPERTY_ISO_COMMENT_value;
	ZVAL_LONG(&const_PROPERTY_ISO_COMMENT_value, UCHAR_ISO_COMMENT);
	zend_string *const_PROPERTY_ISO_COMMENT_name = zend_string_init_interned("PROPERTY_ISO_COMMENT", sizeof("PROPERTY_ISO_COMMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_ISO_COMMENT_name, &const_PROPERTY_ISO_COMMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_ISO_COMMENT_name);

	zval const_PROPERTY_LOWERCASE_MAPPING_value;
	ZVAL_LONG(&const_PROPERTY_LOWERCASE_MAPPING_value, UCHAR_LOWERCASE_MAPPING);
	zend_string *const_PROPERTY_LOWERCASE_MAPPING_name = zend_string_init_interned("PROPERTY_LOWERCASE_MAPPING", sizeof("PROPERTY_LOWERCASE_MAPPING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_LOWERCASE_MAPPING_name, &const_PROPERTY_LOWERCASE_MAPPING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_LOWERCASE_MAPPING_name);

	zval const_PROPERTY_NAME_value;
	ZVAL_LONG(&const_PROPERTY_NAME_value, UCHAR_NAME);
	zend_string *const_PROPERTY_NAME_name = zend_string_init_interned("PROPERTY_NAME", sizeof("PROPERTY_NAME") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_NAME_name, &const_PROPERTY_NAME_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_NAME_name);

	zval const_PROPERTY_SIMPLE_CASE_FOLDING_value;
	ZVAL_LONG(&const_PROPERTY_SIMPLE_CASE_FOLDING_value, UCHAR_SIMPLE_CASE_FOLDING);
	zend_string *const_PROPERTY_SIMPLE_CASE_FOLDING_name = zend_string_init_interned("PROPERTY_SIMPLE_CASE_FOLDING", sizeof("PROPERTY_SIMPLE_CASE_FOLDING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_SIMPLE_CASE_FOLDING_name, &const_PROPERTY_SIMPLE_CASE_FOLDING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_SIMPLE_CASE_FOLDING_name);

	zval const_PROPERTY_SIMPLE_LOWERCASE_MAPPING_value;
	ZVAL_LONG(&const_PROPERTY_SIMPLE_LOWERCASE_MAPPING_value, UCHAR_SIMPLE_LOWERCASE_MAPPING);
	zend_string *const_PROPERTY_SIMPLE_LOWERCASE_MAPPING_name = zend_string_init_interned("PROPERTY_SIMPLE_LOWERCASE_MAPPING", sizeof("PROPERTY_SIMPLE_LOWERCASE_MAPPING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_SIMPLE_LOWERCASE_MAPPING_name, &const_PROPERTY_SIMPLE_LOWERCASE_MAPPING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_SIMPLE_LOWERCASE_MAPPING_name);

	zval const_PROPERTY_SIMPLE_TITLECASE_MAPPING_value;
	ZVAL_LONG(&const_PROPERTY_SIMPLE_TITLECASE_MAPPING_value, UCHAR_SIMPLE_TITLECASE_MAPPING);
	zend_string *const_PROPERTY_SIMPLE_TITLECASE_MAPPING_name = zend_string_init_interned("PROPERTY_SIMPLE_TITLECASE_MAPPING", sizeof("PROPERTY_SIMPLE_TITLECASE_MAPPING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_SIMPLE_TITLECASE_MAPPING_name, &const_PROPERTY_SIMPLE_TITLECASE_MAPPING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_SIMPLE_TITLECASE_MAPPING_name);

	zval const_PROPERTY_SIMPLE_UPPERCASE_MAPPING_value;
	ZVAL_LONG(&const_PROPERTY_SIMPLE_UPPERCASE_MAPPING_value, UCHAR_SIMPLE_UPPERCASE_MAPPING);
	zend_string *const_PROPERTY_SIMPLE_UPPERCASE_MAPPING_name = zend_string_init_interned("PROPERTY_SIMPLE_UPPERCASE_MAPPING", sizeof("PROPERTY_SIMPLE_UPPERCASE_MAPPING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_SIMPLE_UPPERCASE_MAPPING_name, &const_PROPERTY_SIMPLE_UPPERCASE_MAPPING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_SIMPLE_UPPERCASE_MAPPING_name);

	zval const_PROPERTY_TITLECASE_MAPPING_value;
	ZVAL_LONG(&const_PROPERTY_TITLECASE_MAPPING_value, UCHAR_TITLECASE_MAPPING);
	zend_string *const_PROPERTY_TITLECASE_MAPPING_name = zend_string_init_interned("PROPERTY_TITLECASE_MAPPING", sizeof("PROPERTY_TITLECASE_MAPPING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_TITLECASE_MAPPING_name, &const_PROPERTY_TITLECASE_MAPPING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_TITLECASE_MAPPING_name);

	zval const_PROPERTY_UNICODE_1_NAME_value;
	ZVAL_LONG(&const_PROPERTY_UNICODE_1_NAME_value, UCHAR_UNICODE_1_NAME);
	zend_string *const_PROPERTY_UNICODE_1_NAME_name = zend_string_init_interned("PROPERTY_UNICODE_1_NAME", sizeof("PROPERTY_UNICODE_1_NAME") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_UNICODE_1_NAME_name, &const_PROPERTY_UNICODE_1_NAME_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_UNICODE_1_NAME_name);

	zval const_PROPERTY_UPPERCASE_MAPPING_value;
	ZVAL_LONG(&const_PROPERTY_UPPERCASE_MAPPING_value, UCHAR_UPPERCASE_MAPPING);
	zend_string *const_PROPERTY_UPPERCASE_MAPPING_name = zend_string_init_interned("PROPERTY_UPPERCASE_MAPPING", sizeof("PROPERTY_UPPERCASE_MAPPING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_UPPERCASE_MAPPING_name, &const_PROPERTY_UPPERCASE_MAPPING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_UPPERCASE_MAPPING_name);
#if U_ICU_VERSION_MAJOR_NUM >= 52

	zval const_PROPERTY_BIDI_PAIRED_BRACKET_value;
	ZVAL_LONG(&const_PROPERTY_BIDI_PAIRED_BRACKET_value, UCHAR_BIDI_PAIRED_BRACKET);
	zend_string *const_PROPERTY_BIDI_PAIRED_BRACKET_name = zend_string_init_interned("PROPERTY_BIDI_PAIRED_BRACKET", sizeof("PROPERTY_BIDI_PAIRED_BRACKET") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_BIDI_PAIRED_BRACKET_name, &const_PROPERTY_BIDI_PAIRED_BRACKET_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_BIDI_PAIRED_BRACKET_name);
#endif

	zval const_PROPERTY_STRING_LIMIT_value;
	ZVAL_LONG(&const_PROPERTY_STRING_LIMIT_value, UCHAR_STRING_LIMIT);
	zend_string *const_PROPERTY_STRING_LIMIT_name = zend_string_init_interned("PROPERTY_STRING_LIMIT", sizeof("PROPERTY_STRING_LIMIT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_STRING_LIMIT_name, &const_PROPERTY_STRING_LIMIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_STRING_LIMIT_name);

	zval const_PROPERTY_SCRIPT_EXTENSIONS_value;
	ZVAL_LONG(&const_PROPERTY_SCRIPT_EXTENSIONS_value, UCHAR_SCRIPT_EXTENSIONS);
	zend_string *const_PROPERTY_SCRIPT_EXTENSIONS_name = zend_string_init_interned("PROPERTY_SCRIPT_EXTENSIONS", sizeof("PROPERTY_SCRIPT_EXTENSIONS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_SCRIPT_EXTENSIONS_name, &const_PROPERTY_SCRIPT_EXTENSIONS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_SCRIPT_EXTENSIONS_name);

	zval const_PROPERTY_OTHER_PROPERTY_START_value;
	ZVAL_LONG(&const_PROPERTY_OTHER_PROPERTY_START_value, UCHAR_OTHER_PROPERTY_START);
	zend_string *const_PROPERTY_OTHER_PROPERTY_START_name = zend_string_init_interned("PROPERTY_OTHER_PROPERTY_START", sizeof("PROPERTY_OTHER_PROPERTY_START") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_OTHER_PROPERTY_START_name, &const_PROPERTY_OTHER_PROPERTY_START_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_OTHER_PROPERTY_START_name);

	zval const_PROPERTY_OTHER_PROPERTY_LIMIT_value;
	ZVAL_LONG(&const_PROPERTY_OTHER_PROPERTY_LIMIT_value, UCHAR_OTHER_PROPERTY_LIMIT);
	zend_string *const_PROPERTY_OTHER_PROPERTY_LIMIT_name = zend_string_init_interned("PROPERTY_OTHER_PROPERTY_LIMIT", sizeof("PROPERTY_OTHER_PROPERTY_LIMIT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_OTHER_PROPERTY_LIMIT_name, &const_PROPERTY_OTHER_PROPERTY_LIMIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_OTHER_PROPERTY_LIMIT_name);

	zval const_PROPERTY_INVALID_CODE_value;
	ZVAL_LONG(&const_PROPERTY_INVALID_CODE_value, UCHAR_INVALID_CODE);
	zend_string *const_PROPERTY_INVALID_CODE_name = zend_string_init_interned("PROPERTY_INVALID_CODE", sizeof("PROPERTY_INVALID_CODE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_INVALID_CODE_name, &const_PROPERTY_INVALID_CODE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_INVALID_CODE_name);

	zval const_CHAR_CATEGORY_UNASSIGNED_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_UNASSIGNED_value, U_UNASSIGNED);
	zend_string *const_CHAR_CATEGORY_UNASSIGNED_name = zend_string_init_interned("CHAR_CATEGORY_UNASSIGNED", sizeof("CHAR_CATEGORY_UNASSIGNED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_UNASSIGNED_name, &const_CHAR_CATEGORY_UNASSIGNED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_UNASSIGNED_name);

	zval const_CHAR_CATEGORY_GENERAL_OTHER_TYPES_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_GENERAL_OTHER_TYPES_value, U_GENERAL_OTHER_TYPES);
	zend_string *const_CHAR_CATEGORY_GENERAL_OTHER_TYPES_name = zend_string_init_interned("CHAR_CATEGORY_GENERAL_OTHER_TYPES", sizeof("CHAR_CATEGORY_GENERAL_OTHER_TYPES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_GENERAL_OTHER_TYPES_name, &const_CHAR_CATEGORY_GENERAL_OTHER_TYPES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_GENERAL_OTHER_TYPES_name);

	zval const_CHAR_CATEGORY_UPPERCASE_LETTER_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_UPPERCASE_LETTER_value, U_UPPERCASE_LETTER);
	zend_string *const_CHAR_CATEGORY_UPPERCASE_LETTER_name = zend_string_init_interned("CHAR_CATEGORY_UPPERCASE_LETTER", sizeof("CHAR_CATEGORY_UPPERCASE_LETTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_UPPERCASE_LETTER_name, &const_CHAR_CATEGORY_UPPERCASE_LETTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_UPPERCASE_LETTER_name);

	zval const_CHAR_CATEGORY_LOWERCASE_LETTER_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_LOWERCASE_LETTER_value, U_LOWERCASE_LETTER);
	zend_string *const_CHAR_CATEGORY_LOWERCASE_LETTER_name = zend_string_init_interned("CHAR_CATEGORY_LOWERCASE_LETTER", sizeof("CHAR_CATEGORY_LOWERCASE_LETTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_LOWERCASE_LETTER_name, &const_CHAR_CATEGORY_LOWERCASE_LETTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_LOWERCASE_LETTER_name);

	zval const_CHAR_CATEGORY_TITLECASE_LETTER_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_TITLECASE_LETTER_value, U_TITLECASE_LETTER);
	zend_string *const_CHAR_CATEGORY_TITLECASE_LETTER_name = zend_string_init_interned("CHAR_CATEGORY_TITLECASE_LETTER", sizeof("CHAR_CATEGORY_TITLECASE_LETTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_TITLECASE_LETTER_name, &const_CHAR_CATEGORY_TITLECASE_LETTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_TITLECASE_LETTER_name);

	zval const_CHAR_CATEGORY_MODIFIER_LETTER_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_MODIFIER_LETTER_value, U_MODIFIER_LETTER);
	zend_string *const_CHAR_CATEGORY_MODIFIER_LETTER_name = zend_string_init_interned("CHAR_CATEGORY_MODIFIER_LETTER", sizeof("CHAR_CATEGORY_MODIFIER_LETTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_MODIFIER_LETTER_name, &const_CHAR_CATEGORY_MODIFIER_LETTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_MODIFIER_LETTER_name);

	zval const_CHAR_CATEGORY_OTHER_LETTER_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_OTHER_LETTER_value, U_OTHER_LETTER);
	zend_string *const_CHAR_CATEGORY_OTHER_LETTER_name = zend_string_init_interned("CHAR_CATEGORY_OTHER_LETTER", sizeof("CHAR_CATEGORY_OTHER_LETTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_OTHER_LETTER_name, &const_CHAR_CATEGORY_OTHER_LETTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_OTHER_LETTER_name);

	zval const_CHAR_CATEGORY_NON_SPACING_MARK_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_NON_SPACING_MARK_value, U_NON_SPACING_MARK);
	zend_string *const_CHAR_CATEGORY_NON_SPACING_MARK_name = zend_string_init_interned("CHAR_CATEGORY_NON_SPACING_MARK", sizeof("CHAR_CATEGORY_NON_SPACING_MARK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_NON_SPACING_MARK_name, &const_CHAR_CATEGORY_NON_SPACING_MARK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_NON_SPACING_MARK_name);

	zval const_CHAR_CATEGORY_ENCLOSING_MARK_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_ENCLOSING_MARK_value, U_ENCLOSING_MARK);
	zend_string *const_CHAR_CATEGORY_ENCLOSING_MARK_name = zend_string_init_interned("CHAR_CATEGORY_ENCLOSING_MARK", sizeof("CHAR_CATEGORY_ENCLOSING_MARK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_ENCLOSING_MARK_name, &const_CHAR_CATEGORY_ENCLOSING_MARK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_ENCLOSING_MARK_name);

	zval const_CHAR_CATEGORY_COMBINING_SPACING_MARK_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_COMBINING_SPACING_MARK_value, U_COMBINING_SPACING_MARK);
	zend_string *const_CHAR_CATEGORY_COMBINING_SPACING_MARK_name = zend_string_init_interned("CHAR_CATEGORY_COMBINING_SPACING_MARK", sizeof("CHAR_CATEGORY_COMBINING_SPACING_MARK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_COMBINING_SPACING_MARK_name, &const_CHAR_CATEGORY_COMBINING_SPACING_MARK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_COMBINING_SPACING_MARK_name);

	zval const_CHAR_CATEGORY_DECIMAL_DIGIT_NUMBER_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_DECIMAL_DIGIT_NUMBER_value, U_DECIMAL_DIGIT_NUMBER);
	zend_string *const_CHAR_CATEGORY_DECIMAL_DIGIT_NUMBER_name = zend_string_init_interned("CHAR_CATEGORY_DECIMAL_DIGIT_NUMBER", sizeof("CHAR_CATEGORY_DECIMAL_DIGIT_NUMBER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_DECIMAL_DIGIT_NUMBER_name, &const_CHAR_CATEGORY_DECIMAL_DIGIT_NUMBER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_DECIMAL_DIGIT_NUMBER_name);

	zval const_CHAR_CATEGORY_LETTER_NUMBER_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_LETTER_NUMBER_value, U_LETTER_NUMBER);
	zend_string *const_CHAR_CATEGORY_LETTER_NUMBER_name = zend_string_init_interned("CHAR_CATEGORY_LETTER_NUMBER", sizeof("CHAR_CATEGORY_LETTER_NUMBER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_LETTER_NUMBER_name, &const_CHAR_CATEGORY_LETTER_NUMBER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_LETTER_NUMBER_name);

	zval const_CHAR_CATEGORY_OTHER_NUMBER_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_OTHER_NUMBER_value, U_OTHER_NUMBER);
	zend_string *const_CHAR_CATEGORY_OTHER_NUMBER_name = zend_string_init_interned("CHAR_CATEGORY_OTHER_NUMBER", sizeof("CHAR_CATEGORY_OTHER_NUMBER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_OTHER_NUMBER_name, &const_CHAR_CATEGORY_OTHER_NUMBER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_OTHER_NUMBER_name);

	zval const_CHAR_CATEGORY_SPACE_SEPARATOR_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_SPACE_SEPARATOR_value, U_SPACE_SEPARATOR);
	zend_string *const_CHAR_CATEGORY_SPACE_SEPARATOR_name = zend_string_init_interned("CHAR_CATEGORY_SPACE_SEPARATOR", sizeof("CHAR_CATEGORY_SPACE_SEPARATOR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_SPACE_SEPARATOR_name, &const_CHAR_CATEGORY_SPACE_SEPARATOR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_SPACE_SEPARATOR_name);

	zval const_CHAR_CATEGORY_LINE_SEPARATOR_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_LINE_SEPARATOR_value, U_LINE_SEPARATOR);
	zend_string *const_CHAR_CATEGORY_LINE_SEPARATOR_name = zend_string_init_interned("CHAR_CATEGORY_LINE_SEPARATOR", sizeof("CHAR_CATEGORY_LINE_SEPARATOR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_LINE_SEPARATOR_name, &const_CHAR_CATEGORY_LINE_SEPARATOR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_LINE_SEPARATOR_name);

	zval const_CHAR_CATEGORY_PARAGRAPH_SEPARATOR_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_PARAGRAPH_SEPARATOR_value, U_PARAGRAPH_SEPARATOR);
	zend_string *const_CHAR_CATEGORY_PARAGRAPH_SEPARATOR_name = zend_string_init_interned("CHAR_CATEGORY_PARAGRAPH_SEPARATOR", sizeof("CHAR_CATEGORY_PARAGRAPH_SEPARATOR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_PARAGRAPH_SEPARATOR_name, &const_CHAR_CATEGORY_PARAGRAPH_SEPARATOR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_PARAGRAPH_SEPARATOR_name);

	zval const_CHAR_CATEGORY_CONTROL_CHAR_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_CONTROL_CHAR_value, U_CONTROL_CHAR);
	zend_string *const_CHAR_CATEGORY_CONTROL_CHAR_name = zend_string_init_interned("CHAR_CATEGORY_CONTROL_CHAR", sizeof("CHAR_CATEGORY_CONTROL_CHAR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_CONTROL_CHAR_name, &const_CHAR_CATEGORY_CONTROL_CHAR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_CONTROL_CHAR_name);

	zval const_CHAR_CATEGORY_FORMAT_CHAR_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_FORMAT_CHAR_value, U_FORMAT_CHAR);
	zend_string *const_CHAR_CATEGORY_FORMAT_CHAR_name = zend_string_init_interned("CHAR_CATEGORY_FORMAT_CHAR", sizeof("CHAR_CATEGORY_FORMAT_CHAR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_FORMAT_CHAR_name, &const_CHAR_CATEGORY_FORMAT_CHAR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_FORMAT_CHAR_name);

	zval const_CHAR_CATEGORY_PRIVATE_USE_CHAR_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_PRIVATE_USE_CHAR_value, U_PRIVATE_USE_CHAR);
	zend_string *const_CHAR_CATEGORY_PRIVATE_USE_CHAR_name = zend_string_init_interned("CHAR_CATEGORY_PRIVATE_USE_CHAR", sizeof("CHAR_CATEGORY_PRIVATE_USE_CHAR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_PRIVATE_USE_CHAR_name, &const_CHAR_CATEGORY_PRIVATE_USE_CHAR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_PRIVATE_USE_CHAR_name);

	zval const_CHAR_CATEGORY_SURROGATE_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_SURROGATE_value, U_SURROGATE);
	zend_string *const_CHAR_CATEGORY_SURROGATE_name = zend_string_init_interned("CHAR_CATEGORY_SURROGATE", sizeof("CHAR_CATEGORY_SURROGATE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_SURROGATE_name, &const_CHAR_CATEGORY_SURROGATE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_SURROGATE_name);

	zval const_CHAR_CATEGORY_DASH_PUNCTUATION_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_DASH_PUNCTUATION_value, U_DASH_PUNCTUATION);
	zend_string *const_CHAR_CATEGORY_DASH_PUNCTUATION_name = zend_string_init_interned("CHAR_CATEGORY_DASH_PUNCTUATION", sizeof("CHAR_CATEGORY_DASH_PUNCTUATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_DASH_PUNCTUATION_name, &const_CHAR_CATEGORY_DASH_PUNCTUATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_DASH_PUNCTUATION_name);

	zval const_CHAR_CATEGORY_START_PUNCTUATION_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_START_PUNCTUATION_value, U_START_PUNCTUATION);
	zend_string *const_CHAR_CATEGORY_START_PUNCTUATION_name = zend_string_init_interned("CHAR_CATEGORY_START_PUNCTUATION", sizeof("CHAR_CATEGORY_START_PUNCTUATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_START_PUNCTUATION_name, &const_CHAR_CATEGORY_START_PUNCTUATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_START_PUNCTUATION_name);

	zval const_CHAR_CATEGORY_END_PUNCTUATION_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_END_PUNCTUATION_value, U_END_PUNCTUATION);
	zend_string *const_CHAR_CATEGORY_END_PUNCTUATION_name = zend_string_init_interned("CHAR_CATEGORY_END_PUNCTUATION", sizeof("CHAR_CATEGORY_END_PUNCTUATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_END_PUNCTUATION_name, &const_CHAR_CATEGORY_END_PUNCTUATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_END_PUNCTUATION_name);

	zval const_CHAR_CATEGORY_CONNECTOR_PUNCTUATION_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_CONNECTOR_PUNCTUATION_value, U_CONNECTOR_PUNCTUATION);
	zend_string *const_CHAR_CATEGORY_CONNECTOR_PUNCTUATION_name = zend_string_init_interned("CHAR_CATEGORY_CONNECTOR_PUNCTUATION", sizeof("CHAR_CATEGORY_CONNECTOR_PUNCTUATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_CONNECTOR_PUNCTUATION_name, &const_CHAR_CATEGORY_CONNECTOR_PUNCTUATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_CONNECTOR_PUNCTUATION_name);

	zval const_CHAR_CATEGORY_OTHER_PUNCTUATION_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_OTHER_PUNCTUATION_value, U_OTHER_PUNCTUATION);
	zend_string *const_CHAR_CATEGORY_OTHER_PUNCTUATION_name = zend_string_init_interned("CHAR_CATEGORY_OTHER_PUNCTUATION", sizeof("CHAR_CATEGORY_OTHER_PUNCTUATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_OTHER_PUNCTUATION_name, &const_CHAR_CATEGORY_OTHER_PUNCTUATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_OTHER_PUNCTUATION_name);

	zval const_CHAR_CATEGORY_MATH_SYMBOL_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_MATH_SYMBOL_value, U_MATH_SYMBOL);
	zend_string *const_CHAR_CATEGORY_MATH_SYMBOL_name = zend_string_init_interned("CHAR_CATEGORY_MATH_SYMBOL", sizeof("CHAR_CATEGORY_MATH_SYMBOL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_MATH_SYMBOL_name, &const_CHAR_CATEGORY_MATH_SYMBOL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_MATH_SYMBOL_name);

	zval const_CHAR_CATEGORY_CURRENCY_SYMBOL_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_CURRENCY_SYMBOL_value, U_CURRENCY_SYMBOL);
	zend_string *const_CHAR_CATEGORY_CURRENCY_SYMBOL_name = zend_string_init_interned("CHAR_CATEGORY_CURRENCY_SYMBOL", sizeof("CHAR_CATEGORY_CURRENCY_SYMBOL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_CURRENCY_SYMBOL_name, &const_CHAR_CATEGORY_CURRENCY_SYMBOL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_CURRENCY_SYMBOL_name);

	zval const_CHAR_CATEGORY_MODIFIER_SYMBOL_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_MODIFIER_SYMBOL_value, U_MODIFIER_SYMBOL);
	zend_string *const_CHAR_CATEGORY_MODIFIER_SYMBOL_name = zend_string_init_interned("CHAR_CATEGORY_MODIFIER_SYMBOL", sizeof("CHAR_CATEGORY_MODIFIER_SYMBOL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_MODIFIER_SYMBOL_name, &const_CHAR_CATEGORY_MODIFIER_SYMBOL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_MODIFIER_SYMBOL_name);

	zval const_CHAR_CATEGORY_OTHER_SYMBOL_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_OTHER_SYMBOL_value, U_OTHER_SYMBOL);
	zend_string *const_CHAR_CATEGORY_OTHER_SYMBOL_name = zend_string_init_interned("CHAR_CATEGORY_OTHER_SYMBOL", sizeof("CHAR_CATEGORY_OTHER_SYMBOL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_OTHER_SYMBOL_name, &const_CHAR_CATEGORY_OTHER_SYMBOL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_OTHER_SYMBOL_name);

	zval const_CHAR_CATEGORY_INITIAL_PUNCTUATION_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_INITIAL_PUNCTUATION_value, U_INITIAL_PUNCTUATION);
	zend_string *const_CHAR_CATEGORY_INITIAL_PUNCTUATION_name = zend_string_init_interned("CHAR_CATEGORY_INITIAL_PUNCTUATION", sizeof("CHAR_CATEGORY_INITIAL_PUNCTUATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_INITIAL_PUNCTUATION_name, &const_CHAR_CATEGORY_INITIAL_PUNCTUATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_INITIAL_PUNCTUATION_name);

	zval const_CHAR_CATEGORY_FINAL_PUNCTUATION_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_FINAL_PUNCTUATION_value, U_FINAL_PUNCTUATION);
	zend_string *const_CHAR_CATEGORY_FINAL_PUNCTUATION_name = zend_string_init_interned("CHAR_CATEGORY_FINAL_PUNCTUATION", sizeof("CHAR_CATEGORY_FINAL_PUNCTUATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_FINAL_PUNCTUATION_name, &const_CHAR_CATEGORY_FINAL_PUNCTUATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_FINAL_PUNCTUATION_name);

	zval const_CHAR_CATEGORY_CHAR_CATEGORY_COUNT_value;
	ZVAL_LONG(&const_CHAR_CATEGORY_CHAR_CATEGORY_COUNT_value, U_CHAR_CATEGORY_COUNT);
	zend_string *const_CHAR_CATEGORY_CHAR_CATEGORY_COUNT_name = zend_string_init_interned("CHAR_CATEGORY_CHAR_CATEGORY_COUNT", sizeof("CHAR_CATEGORY_CHAR_CATEGORY_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_CATEGORY_CHAR_CATEGORY_COUNT_name, &const_CHAR_CATEGORY_CHAR_CATEGORY_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_CATEGORY_CHAR_CATEGORY_COUNT_name);

	zval const_CHAR_DIRECTION_LEFT_TO_RIGHT_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_LEFT_TO_RIGHT_value, U_LEFT_TO_RIGHT);
	zend_string *const_CHAR_DIRECTION_LEFT_TO_RIGHT_name = zend_string_init_interned("CHAR_DIRECTION_LEFT_TO_RIGHT", sizeof("CHAR_DIRECTION_LEFT_TO_RIGHT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_LEFT_TO_RIGHT_name, &const_CHAR_DIRECTION_LEFT_TO_RIGHT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_LEFT_TO_RIGHT_name);

	zval const_CHAR_DIRECTION_RIGHT_TO_LEFT_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_RIGHT_TO_LEFT_value, U_RIGHT_TO_LEFT);
	zend_string *const_CHAR_DIRECTION_RIGHT_TO_LEFT_name = zend_string_init_interned("CHAR_DIRECTION_RIGHT_TO_LEFT", sizeof("CHAR_DIRECTION_RIGHT_TO_LEFT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_RIGHT_TO_LEFT_name, &const_CHAR_DIRECTION_RIGHT_TO_LEFT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_RIGHT_TO_LEFT_name);

	zval const_CHAR_DIRECTION_EUROPEAN_NUMBER_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_EUROPEAN_NUMBER_value, U_EUROPEAN_NUMBER);
	zend_string *const_CHAR_DIRECTION_EUROPEAN_NUMBER_name = zend_string_init_interned("CHAR_DIRECTION_EUROPEAN_NUMBER", sizeof("CHAR_DIRECTION_EUROPEAN_NUMBER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_EUROPEAN_NUMBER_name, &const_CHAR_DIRECTION_EUROPEAN_NUMBER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_EUROPEAN_NUMBER_name);

	zval const_CHAR_DIRECTION_EUROPEAN_NUMBER_SEPARATOR_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_EUROPEAN_NUMBER_SEPARATOR_value, U_EUROPEAN_NUMBER_SEPARATOR);
	zend_string *const_CHAR_DIRECTION_EUROPEAN_NUMBER_SEPARATOR_name = zend_string_init_interned("CHAR_DIRECTION_EUROPEAN_NUMBER_SEPARATOR", sizeof("CHAR_DIRECTION_EUROPEAN_NUMBER_SEPARATOR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_EUROPEAN_NUMBER_SEPARATOR_name, &const_CHAR_DIRECTION_EUROPEAN_NUMBER_SEPARATOR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_EUROPEAN_NUMBER_SEPARATOR_name);

	zval const_CHAR_DIRECTION_EUROPEAN_NUMBER_TERMINATOR_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_EUROPEAN_NUMBER_TERMINATOR_value, U_EUROPEAN_NUMBER_TERMINATOR);
	zend_string *const_CHAR_DIRECTION_EUROPEAN_NUMBER_TERMINATOR_name = zend_string_init_interned("CHAR_DIRECTION_EUROPEAN_NUMBER_TERMINATOR", sizeof("CHAR_DIRECTION_EUROPEAN_NUMBER_TERMINATOR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_EUROPEAN_NUMBER_TERMINATOR_name, &const_CHAR_DIRECTION_EUROPEAN_NUMBER_TERMINATOR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_EUROPEAN_NUMBER_TERMINATOR_name);

	zval const_CHAR_DIRECTION_ARABIC_NUMBER_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_ARABIC_NUMBER_value, U_ARABIC_NUMBER);
	zend_string *const_CHAR_DIRECTION_ARABIC_NUMBER_name = zend_string_init_interned("CHAR_DIRECTION_ARABIC_NUMBER", sizeof("CHAR_DIRECTION_ARABIC_NUMBER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_ARABIC_NUMBER_name, &const_CHAR_DIRECTION_ARABIC_NUMBER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_ARABIC_NUMBER_name);

	zval const_CHAR_DIRECTION_COMMON_NUMBER_SEPARATOR_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_COMMON_NUMBER_SEPARATOR_value, U_COMMON_NUMBER_SEPARATOR);
	zend_string *const_CHAR_DIRECTION_COMMON_NUMBER_SEPARATOR_name = zend_string_init_interned("CHAR_DIRECTION_COMMON_NUMBER_SEPARATOR", sizeof("CHAR_DIRECTION_COMMON_NUMBER_SEPARATOR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_COMMON_NUMBER_SEPARATOR_name, &const_CHAR_DIRECTION_COMMON_NUMBER_SEPARATOR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_COMMON_NUMBER_SEPARATOR_name);

	zval const_CHAR_DIRECTION_BLOCK_SEPARATOR_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_BLOCK_SEPARATOR_value, U_BLOCK_SEPARATOR);
	zend_string *const_CHAR_DIRECTION_BLOCK_SEPARATOR_name = zend_string_init_interned("CHAR_DIRECTION_BLOCK_SEPARATOR", sizeof("CHAR_DIRECTION_BLOCK_SEPARATOR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_BLOCK_SEPARATOR_name, &const_CHAR_DIRECTION_BLOCK_SEPARATOR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_BLOCK_SEPARATOR_name);

	zval const_CHAR_DIRECTION_SEGMENT_SEPARATOR_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_SEGMENT_SEPARATOR_value, U_SEGMENT_SEPARATOR);
	zend_string *const_CHAR_DIRECTION_SEGMENT_SEPARATOR_name = zend_string_init_interned("CHAR_DIRECTION_SEGMENT_SEPARATOR", sizeof("CHAR_DIRECTION_SEGMENT_SEPARATOR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_SEGMENT_SEPARATOR_name, &const_CHAR_DIRECTION_SEGMENT_SEPARATOR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_SEGMENT_SEPARATOR_name);

	zval const_CHAR_DIRECTION_WHITE_SPACE_NEUTRAL_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_WHITE_SPACE_NEUTRAL_value, U_WHITE_SPACE_NEUTRAL);
	zend_string *const_CHAR_DIRECTION_WHITE_SPACE_NEUTRAL_name = zend_string_init_interned("CHAR_DIRECTION_WHITE_SPACE_NEUTRAL", sizeof("CHAR_DIRECTION_WHITE_SPACE_NEUTRAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_WHITE_SPACE_NEUTRAL_name, &const_CHAR_DIRECTION_WHITE_SPACE_NEUTRAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_WHITE_SPACE_NEUTRAL_name);

	zval const_CHAR_DIRECTION_OTHER_NEUTRAL_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_OTHER_NEUTRAL_value, U_OTHER_NEUTRAL);
	zend_string *const_CHAR_DIRECTION_OTHER_NEUTRAL_name = zend_string_init_interned("CHAR_DIRECTION_OTHER_NEUTRAL", sizeof("CHAR_DIRECTION_OTHER_NEUTRAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_OTHER_NEUTRAL_name, &const_CHAR_DIRECTION_OTHER_NEUTRAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_OTHER_NEUTRAL_name);

	zval const_CHAR_DIRECTION_LEFT_TO_RIGHT_EMBEDDING_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_LEFT_TO_RIGHT_EMBEDDING_value, U_LEFT_TO_RIGHT_EMBEDDING);
	zend_string *const_CHAR_DIRECTION_LEFT_TO_RIGHT_EMBEDDING_name = zend_string_init_interned("CHAR_DIRECTION_LEFT_TO_RIGHT_EMBEDDING", sizeof("CHAR_DIRECTION_LEFT_TO_RIGHT_EMBEDDING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_LEFT_TO_RIGHT_EMBEDDING_name, &const_CHAR_DIRECTION_LEFT_TO_RIGHT_EMBEDDING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_LEFT_TO_RIGHT_EMBEDDING_name);

	zval const_CHAR_DIRECTION_LEFT_TO_RIGHT_OVERRIDE_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_LEFT_TO_RIGHT_OVERRIDE_value, U_LEFT_TO_RIGHT_OVERRIDE);
	zend_string *const_CHAR_DIRECTION_LEFT_TO_RIGHT_OVERRIDE_name = zend_string_init_interned("CHAR_DIRECTION_LEFT_TO_RIGHT_OVERRIDE", sizeof("CHAR_DIRECTION_LEFT_TO_RIGHT_OVERRIDE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_LEFT_TO_RIGHT_OVERRIDE_name, &const_CHAR_DIRECTION_LEFT_TO_RIGHT_OVERRIDE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_LEFT_TO_RIGHT_OVERRIDE_name);

	zval const_CHAR_DIRECTION_RIGHT_TO_LEFT_ARABIC_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_RIGHT_TO_LEFT_ARABIC_value, U_RIGHT_TO_LEFT_ARABIC);
	zend_string *const_CHAR_DIRECTION_RIGHT_TO_LEFT_ARABIC_name = zend_string_init_interned("CHAR_DIRECTION_RIGHT_TO_LEFT_ARABIC", sizeof("CHAR_DIRECTION_RIGHT_TO_LEFT_ARABIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_RIGHT_TO_LEFT_ARABIC_name, &const_CHAR_DIRECTION_RIGHT_TO_LEFT_ARABIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_RIGHT_TO_LEFT_ARABIC_name);

	zval const_CHAR_DIRECTION_RIGHT_TO_LEFT_EMBEDDING_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_RIGHT_TO_LEFT_EMBEDDING_value, U_RIGHT_TO_LEFT_EMBEDDING);
	zend_string *const_CHAR_DIRECTION_RIGHT_TO_LEFT_EMBEDDING_name = zend_string_init_interned("CHAR_DIRECTION_RIGHT_TO_LEFT_EMBEDDING", sizeof("CHAR_DIRECTION_RIGHT_TO_LEFT_EMBEDDING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_RIGHT_TO_LEFT_EMBEDDING_name, &const_CHAR_DIRECTION_RIGHT_TO_LEFT_EMBEDDING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_RIGHT_TO_LEFT_EMBEDDING_name);

	zval const_CHAR_DIRECTION_RIGHT_TO_LEFT_OVERRIDE_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_RIGHT_TO_LEFT_OVERRIDE_value, U_RIGHT_TO_LEFT_OVERRIDE);
	zend_string *const_CHAR_DIRECTION_RIGHT_TO_LEFT_OVERRIDE_name = zend_string_init_interned("CHAR_DIRECTION_RIGHT_TO_LEFT_OVERRIDE", sizeof("CHAR_DIRECTION_RIGHT_TO_LEFT_OVERRIDE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_RIGHT_TO_LEFT_OVERRIDE_name, &const_CHAR_DIRECTION_RIGHT_TO_LEFT_OVERRIDE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_RIGHT_TO_LEFT_OVERRIDE_name);

	zval const_CHAR_DIRECTION_POP_DIRECTIONAL_FORMAT_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_POP_DIRECTIONAL_FORMAT_value, U_POP_DIRECTIONAL_FORMAT);
	zend_string *const_CHAR_DIRECTION_POP_DIRECTIONAL_FORMAT_name = zend_string_init_interned("CHAR_DIRECTION_POP_DIRECTIONAL_FORMAT", sizeof("CHAR_DIRECTION_POP_DIRECTIONAL_FORMAT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_POP_DIRECTIONAL_FORMAT_name, &const_CHAR_DIRECTION_POP_DIRECTIONAL_FORMAT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_POP_DIRECTIONAL_FORMAT_name);

	zval const_CHAR_DIRECTION_DIR_NON_SPACING_MARK_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_DIR_NON_SPACING_MARK_value, U_DIR_NON_SPACING_MARK);
	zend_string *const_CHAR_DIRECTION_DIR_NON_SPACING_MARK_name = zend_string_init_interned("CHAR_DIRECTION_DIR_NON_SPACING_MARK", sizeof("CHAR_DIRECTION_DIR_NON_SPACING_MARK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_DIR_NON_SPACING_MARK_name, &const_CHAR_DIRECTION_DIR_NON_SPACING_MARK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_DIR_NON_SPACING_MARK_name);

	zval const_CHAR_DIRECTION_BOUNDARY_NEUTRAL_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_BOUNDARY_NEUTRAL_value, U_BOUNDARY_NEUTRAL);
	zend_string *const_CHAR_DIRECTION_BOUNDARY_NEUTRAL_name = zend_string_init_interned("CHAR_DIRECTION_BOUNDARY_NEUTRAL", sizeof("CHAR_DIRECTION_BOUNDARY_NEUTRAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_BOUNDARY_NEUTRAL_name, &const_CHAR_DIRECTION_BOUNDARY_NEUTRAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_BOUNDARY_NEUTRAL_name);
#if U_ICU_VERSION_MAJOR_NUM >= 52

	zval const_CHAR_DIRECTION_FIRST_STRONG_ISOLATE_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_FIRST_STRONG_ISOLATE_value, U_FIRST_STRONG_ISOLATE);
	zend_string *const_CHAR_DIRECTION_FIRST_STRONG_ISOLATE_name = zend_string_init_interned("CHAR_DIRECTION_FIRST_STRONG_ISOLATE", sizeof("CHAR_DIRECTION_FIRST_STRONG_ISOLATE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_FIRST_STRONG_ISOLATE_name, &const_CHAR_DIRECTION_FIRST_STRONG_ISOLATE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_FIRST_STRONG_ISOLATE_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 52

	zval const_CHAR_DIRECTION_LEFT_TO_RIGHT_ISOLATE_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_LEFT_TO_RIGHT_ISOLATE_value, U_LEFT_TO_RIGHT_ISOLATE);
	zend_string *const_CHAR_DIRECTION_LEFT_TO_RIGHT_ISOLATE_name = zend_string_init_interned("CHAR_DIRECTION_LEFT_TO_RIGHT_ISOLATE", sizeof("CHAR_DIRECTION_LEFT_TO_RIGHT_ISOLATE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_LEFT_TO_RIGHT_ISOLATE_name, &const_CHAR_DIRECTION_LEFT_TO_RIGHT_ISOLATE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_LEFT_TO_RIGHT_ISOLATE_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 52

	zval const_CHAR_DIRECTION_RIGHT_TO_LEFT_ISOLATE_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_RIGHT_TO_LEFT_ISOLATE_value, U_RIGHT_TO_LEFT_ISOLATE);
	zend_string *const_CHAR_DIRECTION_RIGHT_TO_LEFT_ISOLATE_name = zend_string_init_interned("CHAR_DIRECTION_RIGHT_TO_LEFT_ISOLATE", sizeof("CHAR_DIRECTION_RIGHT_TO_LEFT_ISOLATE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_RIGHT_TO_LEFT_ISOLATE_name, &const_CHAR_DIRECTION_RIGHT_TO_LEFT_ISOLATE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_RIGHT_TO_LEFT_ISOLATE_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 52

	zval const_CHAR_DIRECTION_POP_DIRECTIONAL_ISOLATE_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_POP_DIRECTIONAL_ISOLATE_value, U_POP_DIRECTIONAL_ISOLATE);
	zend_string *const_CHAR_DIRECTION_POP_DIRECTIONAL_ISOLATE_name = zend_string_init_interned("CHAR_DIRECTION_POP_DIRECTIONAL_ISOLATE", sizeof("CHAR_DIRECTION_POP_DIRECTIONAL_ISOLATE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_POP_DIRECTIONAL_ISOLATE_name, &const_CHAR_DIRECTION_POP_DIRECTIONAL_ISOLATE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_POP_DIRECTIONAL_ISOLATE_name);
#endif

	zval const_CHAR_DIRECTION_CHAR_DIRECTION_COUNT_value;
	ZVAL_LONG(&const_CHAR_DIRECTION_CHAR_DIRECTION_COUNT_value, U_CHAR_DIRECTION_COUNT);
	zend_string *const_CHAR_DIRECTION_CHAR_DIRECTION_COUNT_name = zend_string_init_interned("CHAR_DIRECTION_CHAR_DIRECTION_COUNT", sizeof("CHAR_DIRECTION_CHAR_DIRECTION_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_DIRECTION_CHAR_DIRECTION_COUNT_name, &const_CHAR_DIRECTION_CHAR_DIRECTION_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_DIRECTION_CHAR_DIRECTION_COUNT_name);

	zval const_BLOCK_CODE_NO_BLOCK_value;
	ZVAL_LONG(&const_BLOCK_CODE_NO_BLOCK_value, UBLOCK_NO_BLOCK);
	zend_string *const_BLOCK_CODE_NO_BLOCK_name = zend_string_init_interned("BLOCK_CODE_NO_BLOCK", sizeof("BLOCK_CODE_NO_BLOCK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_NO_BLOCK_name, &const_BLOCK_CODE_NO_BLOCK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_NO_BLOCK_name);

	zval const_BLOCK_CODE_BASIC_LATIN_value;
	ZVAL_LONG(&const_BLOCK_CODE_BASIC_LATIN_value, UBLOCK_BASIC_LATIN);
	zend_string *const_BLOCK_CODE_BASIC_LATIN_name = zend_string_init_interned("BLOCK_CODE_BASIC_LATIN", sizeof("BLOCK_CODE_BASIC_LATIN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_BASIC_LATIN_name, &const_BLOCK_CODE_BASIC_LATIN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_BASIC_LATIN_name);

	zval const_BLOCK_CODE_LATIN_1_SUPPLEMENT_value;
	ZVAL_LONG(&const_BLOCK_CODE_LATIN_1_SUPPLEMENT_value, UBLOCK_LATIN_1_SUPPLEMENT);
	zend_string *const_BLOCK_CODE_LATIN_1_SUPPLEMENT_name = zend_string_init_interned("BLOCK_CODE_LATIN_1_SUPPLEMENT", sizeof("BLOCK_CODE_LATIN_1_SUPPLEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LATIN_1_SUPPLEMENT_name, &const_BLOCK_CODE_LATIN_1_SUPPLEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LATIN_1_SUPPLEMENT_name);

	zval const_BLOCK_CODE_LATIN_EXTENDED_A_value;
	ZVAL_LONG(&const_BLOCK_CODE_LATIN_EXTENDED_A_value, UBLOCK_LATIN_EXTENDED_A);
	zend_string *const_BLOCK_CODE_LATIN_EXTENDED_A_name = zend_string_init_interned("BLOCK_CODE_LATIN_EXTENDED_A", sizeof("BLOCK_CODE_LATIN_EXTENDED_A") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LATIN_EXTENDED_A_name, &const_BLOCK_CODE_LATIN_EXTENDED_A_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LATIN_EXTENDED_A_name);

	zval const_BLOCK_CODE_LATIN_EXTENDED_B_value;
	ZVAL_LONG(&const_BLOCK_CODE_LATIN_EXTENDED_B_value, UBLOCK_LATIN_EXTENDED_B);
	zend_string *const_BLOCK_CODE_LATIN_EXTENDED_B_name = zend_string_init_interned("BLOCK_CODE_LATIN_EXTENDED_B", sizeof("BLOCK_CODE_LATIN_EXTENDED_B") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LATIN_EXTENDED_B_name, &const_BLOCK_CODE_LATIN_EXTENDED_B_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LATIN_EXTENDED_B_name);

	zval const_BLOCK_CODE_IPA_EXTENSIONS_value;
	ZVAL_LONG(&const_BLOCK_CODE_IPA_EXTENSIONS_value, UBLOCK_IPA_EXTENSIONS);
	zend_string *const_BLOCK_CODE_IPA_EXTENSIONS_name = zend_string_init_interned("BLOCK_CODE_IPA_EXTENSIONS", sizeof("BLOCK_CODE_IPA_EXTENSIONS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_IPA_EXTENSIONS_name, &const_BLOCK_CODE_IPA_EXTENSIONS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_IPA_EXTENSIONS_name);

	zval const_BLOCK_CODE_SPACING_MODIFIER_LETTERS_value;
	ZVAL_LONG(&const_BLOCK_CODE_SPACING_MODIFIER_LETTERS_value, UBLOCK_SPACING_MODIFIER_LETTERS);
	zend_string *const_BLOCK_CODE_SPACING_MODIFIER_LETTERS_name = zend_string_init_interned("BLOCK_CODE_SPACING_MODIFIER_LETTERS", sizeof("BLOCK_CODE_SPACING_MODIFIER_LETTERS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SPACING_MODIFIER_LETTERS_name, &const_BLOCK_CODE_SPACING_MODIFIER_LETTERS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SPACING_MODIFIER_LETTERS_name);

	zval const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_value;
	ZVAL_LONG(&const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_value, UBLOCK_COMBINING_DIACRITICAL_MARKS);
	zend_string *const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_name = zend_string_init_interned("BLOCK_CODE_COMBINING_DIACRITICAL_MARKS", sizeof("BLOCK_CODE_COMBINING_DIACRITICAL_MARKS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_name, &const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_name);

	zval const_BLOCK_CODE_GREEK_value;
	ZVAL_LONG(&const_BLOCK_CODE_GREEK_value, UBLOCK_GREEK);
	zend_string *const_BLOCK_CODE_GREEK_name = zend_string_init_interned("BLOCK_CODE_GREEK", sizeof("BLOCK_CODE_GREEK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_GREEK_name, &const_BLOCK_CODE_GREEK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_GREEK_name);

	zval const_BLOCK_CODE_CYRILLIC_value;
	ZVAL_LONG(&const_BLOCK_CODE_CYRILLIC_value, UBLOCK_CYRILLIC);
	zend_string *const_BLOCK_CODE_CYRILLIC_name = zend_string_init_interned("BLOCK_CODE_CYRILLIC", sizeof("BLOCK_CODE_CYRILLIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CYRILLIC_name, &const_BLOCK_CODE_CYRILLIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CYRILLIC_name);

	zval const_BLOCK_CODE_ARMENIAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_ARMENIAN_value, UBLOCK_ARMENIAN);
	zend_string *const_BLOCK_CODE_ARMENIAN_name = zend_string_init_interned("BLOCK_CODE_ARMENIAN", sizeof("BLOCK_CODE_ARMENIAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ARMENIAN_name, &const_BLOCK_CODE_ARMENIAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ARMENIAN_name);

	zval const_BLOCK_CODE_HEBREW_value;
	ZVAL_LONG(&const_BLOCK_CODE_HEBREW_value, UBLOCK_HEBREW);
	zend_string *const_BLOCK_CODE_HEBREW_name = zend_string_init_interned("BLOCK_CODE_HEBREW", sizeof("BLOCK_CODE_HEBREW") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_HEBREW_name, &const_BLOCK_CODE_HEBREW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_HEBREW_name);

	zval const_BLOCK_CODE_ARABIC_value;
	ZVAL_LONG(&const_BLOCK_CODE_ARABIC_value, UBLOCK_ARABIC);
	zend_string *const_BLOCK_CODE_ARABIC_name = zend_string_init_interned("BLOCK_CODE_ARABIC", sizeof("BLOCK_CODE_ARABIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ARABIC_name, &const_BLOCK_CODE_ARABIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ARABIC_name);

	zval const_BLOCK_CODE_SYRIAC_value;
	ZVAL_LONG(&const_BLOCK_CODE_SYRIAC_value, UBLOCK_SYRIAC);
	zend_string *const_BLOCK_CODE_SYRIAC_name = zend_string_init_interned("BLOCK_CODE_SYRIAC", sizeof("BLOCK_CODE_SYRIAC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SYRIAC_name, &const_BLOCK_CODE_SYRIAC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SYRIAC_name);

	zval const_BLOCK_CODE_THAANA_value;
	ZVAL_LONG(&const_BLOCK_CODE_THAANA_value, UBLOCK_THAANA);
	zend_string *const_BLOCK_CODE_THAANA_name = zend_string_init_interned("BLOCK_CODE_THAANA", sizeof("BLOCK_CODE_THAANA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_THAANA_name, &const_BLOCK_CODE_THAANA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_THAANA_name);

	zval const_BLOCK_CODE_DEVANAGARI_value;
	ZVAL_LONG(&const_BLOCK_CODE_DEVANAGARI_value, UBLOCK_DEVANAGARI);
	zend_string *const_BLOCK_CODE_DEVANAGARI_name = zend_string_init_interned("BLOCK_CODE_DEVANAGARI", sizeof("BLOCK_CODE_DEVANAGARI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_DEVANAGARI_name, &const_BLOCK_CODE_DEVANAGARI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_DEVANAGARI_name);

	zval const_BLOCK_CODE_BENGALI_value;
	ZVAL_LONG(&const_BLOCK_CODE_BENGALI_value, UBLOCK_BENGALI);
	zend_string *const_BLOCK_CODE_BENGALI_name = zend_string_init_interned("BLOCK_CODE_BENGALI", sizeof("BLOCK_CODE_BENGALI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_BENGALI_name, &const_BLOCK_CODE_BENGALI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_BENGALI_name);

	zval const_BLOCK_CODE_GURMUKHI_value;
	ZVAL_LONG(&const_BLOCK_CODE_GURMUKHI_value, UBLOCK_GURMUKHI);
	zend_string *const_BLOCK_CODE_GURMUKHI_name = zend_string_init_interned("BLOCK_CODE_GURMUKHI", sizeof("BLOCK_CODE_GURMUKHI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_GURMUKHI_name, &const_BLOCK_CODE_GURMUKHI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_GURMUKHI_name);

	zval const_BLOCK_CODE_GUJARATI_value;
	ZVAL_LONG(&const_BLOCK_CODE_GUJARATI_value, UBLOCK_GUJARATI);
	zend_string *const_BLOCK_CODE_GUJARATI_name = zend_string_init_interned("BLOCK_CODE_GUJARATI", sizeof("BLOCK_CODE_GUJARATI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_GUJARATI_name, &const_BLOCK_CODE_GUJARATI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_GUJARATI_name);

	zval const_BLOCK_CODE_ORIYA_value;
	ZVAL_LONG(&const_BLOCK_CODE_ORIYA_value, UBLOCK_ORIYA);
	zend_string *const_BLOCK_CODE_ORIYA_name = zend_string_init_interned("BLOCK_CODE_ORIYA", sizeof("BLOCK_CODE_ORIYA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ORIYA_name, &const_BLOCK_CODE_ORIYA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ORIYA_name);

	zval const_BLOCK_CODE_TAMIL_value;
	ZVAL_LONG(&const_BLOCK_CODE_TAMIL_value, UBLOCK_TAMIL);
	zend_string *const_BLOCK_CODE_TAMIL_name = zend_string_init_interned("BLOCK_CODE_TAMIL", sizeof("BLOCK_CODE_TAMIL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_TAMIL_name, &const_BLOCK_CODE_TAMIL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_TAMIL_name);

	zval const_BLOCK_CODE_TELUGU_value;
	ZVAL_LONG(&const_BLOCK_CODE_TELUGU_value, UBLOCK_TELUGU);
	zend_string *const_BLOCK_CODE_TELUGU_name = zend_string_init_interned("BLOCK_CODE_TELUGU", sizeof("BLOCK_CODE_TELUGU") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_TELUGU_name, &const_BLOCK_CODE_TELUGU_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_TELUGU_name);

	zval const_BLOCK_CODE_KANNADA_value;
	ZVAL_LONG(&const_BLOCK_CODE_KANNADA_value, UBLOCK_KANNADA);
	zend_string *const_BLOCK_CODE_KANNADA_name = zend_string_init_interned("BLOCK_CODE_KANNADA", sizeof("BLOCK_CODE_KANNADA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_KANNADA_name, &const_BLOCK_CODE_KANNADA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_KANNADA_name);

	zval const_BLOCK_CODE_MALAYALAM_value;
	ZVAL_LONG(&const_BLOCK_CODE_MALAYALAM_value, UBLOCK_MALAYALAM);
	zend_string *const_BLOCK_CODE_MALAYALAM_name = zend_string_init_interned("BLOCK_CODE_MALAYALAM", sizeof("BLOCK_CODE_MALAYALAM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MALAYALAM_name, &const_BLOCK_CODE_MALAYALAM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MALAYALAM_name);

	zval const_BLOCK_CODE_SINHALA_value;
	ZVAL_LONG(&const_BLOCK_CODE_SINHALA_value, UBLOCK_SINHALA);
	zend_string *const_BLOCK_CODE_SINHALA_name = zend_string_init_interned("BLOCK_CODE_SINHALA", sizeof("BLOCK_CODE_SINHALA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SINHALA_name, &const_BLOCK_CODE_SINHALA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SINHALA_name);

	zval const_BLOCK_CODE_THAI_value;
	ZVAL_LONG(&const_BLOCK_CODE_THAI_value, UBLOCK_THAI);
	zend_string *const_BLOCK_CODE_THAI_name = zend_string_init_interned("BLOCK_CODE_THAI", sizeof("BLOCK_CODE_THAI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_THAI_name, &const_BLOCK_CODE_THAI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_THAI_name);

	zval const_BLOCK_CODE_LAO_value;
	ZVAL_LONG(&const_BLOCK_CODE_LAO_value, UBLOCK_LAO);
	zend_string *const_BLOCK_CODE_LAO_name = zend_string_init_interned("BLOCK_CODE_LAO", sizeof("BLOCK_CODE_LAO") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LAO_name, &const_BLOCK_CODE_LAO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LAO_name);

	zval const_BLOCK_CODE_TIBETAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_TIBETAN_value, UBLOCK_TIBETAN);
	zend_string *const_BLOCK_CODE_TIBETAN_name = zend_string_init_interned("BLOCK_CODE_TIBETAN", sizeof("BLOCK_CODE_TIBETAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_TIBETAN_name, &const_BLOCK_CODE_TIBETAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_TIBETAN_name);

	zval const_BLOCK_CODE_MYANMAR_value;
	ZVAL_LONG(&const_BLOCK_CODE_MYANMAR_value, UBLOCK_MYANMAR);
	zend_string *const_BLOCK_CODE_MYANMAR_name = zend_string_init_interned("BLOCK_CODE_MYANMAR", sizeof("BLOCK_CODE_MYANMAR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MYANMAR_name, &const_BLOCK_CODE_MYANMAR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MYANMAR_name);

	zval const_BLOCK_CODE_GEORGIAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_GEORGIAN_value, UBLOCK_GEORGIAN);
	zend_string *const_BLOCK_CODE_GEORGIAN_name = zend_string_init_interned("BLOCK_CODE_GEORGIAN", sizeof("BLOCK_CODE_GEORGIAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_GEORGIAN_name, &const_BLOCK_CODE_GEORGIAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_GEORGIAN_name);

	zval const_BLOCK_CODE_HANGUL_JAMO_value;
	ZVAL_LONG(&const_BLOCK_CODE_HANGUL_JAMO_value, UBLOCK_HANGUL_JAMO);
	zend_string *const_BLOCK_CODE_HANGUL_JAMO_name = zend_string_init_interned("BLOCK_CODE_HANGUL_JAMO", sizeof("BLOCK_CODE_HANGUL_JAMO") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_HANGUL_JAMO_name, &const_BLOCK_CODE_HANGUL_JAMO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_HANGUL_JAMO_name);

	zval const_BLOCK_CODE_ETHIOPIC_value;
	ZVAL_LONG(&const_BLOCK_CODE_ETHIOPIC_value, UBLOCK_ETHIOPIC);
	zend_string *const_BLOCK_CODE_ETHIOPIC_name = zend_string_init_interned("BLOCK_CODE_ETHIOPIC", sizeof("BLOCK_CODE_ETHIOPIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ETHIOPIC_name, &const_BLOCK_CODE_ETHIOPIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ETHIOPIC_name);

	zval const_BLOCK_CODE_CHEROKEE_value;
	ZVAL_LONG(&const_BLOCK_CODE_CHEROKEE_value, UBLOCK_CHEROKEE);
	zend_string *const_BLOCK_CODE_CHEROKEE_name = zend_string_init_interned("BLOCK_CODE_CHEROKEE", sizeof("BLOCK_CODE_CHEROKEE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CHEROKEE_name, &const_BLOCK_CODE_CHEROKEE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CHEROKEE_name);

	zval const_BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_value;
	ZVAL_LONG(&const_BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_value, UBLOCK_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS);
	zend_string *const_BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_name = zend_string_init_interned("BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS", sizeof("BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_name, &const_BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_name);

	zval const_BLOCK_CODE_OGHAM_value;
	ZVAL_LONG(&const_BLOCK_CODE_OGHAM_value, UBLOCK_OGHAM);
	zend_string *const_BLOCK_CODE_OGHAM_name = zend_string_init_interned("BLOCK_CODE_OGHAM", sizeof("BLOCK_CODE_OGHAM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_OGHAM_name, &const_BLOCK_CODE_OGHAM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_OGHAM_name);

	zval const_BLOCK_CODE_RUNIC_value;
	ZVAL_LONG(&const_BLOCK_CODE_RUNIC_value, UBLOCK_RUNIC);
	zend_string *const_BLOCK_CODE_RUNIC_name = zend_string_init_interned("BLOCK_CODE_RUNIC", sizeof("BLOCK_CODE_RUNIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_RUNIC_name, &const_BLOCK_CODE_RUNIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_RUNIC_name);

	zval const_BLOCK_CODE_KHMER_value;
	ZVAL_LONG(&const_BLOCK_CODE_KHMER_value, UBLOCK_KHMER);
	zend_string *const_BLOCK_CODE_KHMER_name = zend_string_init_interned("BLOCK_CODE_KHMER", sizeof("BLOCK_CODE_KHMER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_KHMER_name, &const_BLOCK_CODE_KHMER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_KHMER_name);

	zval const_BLOCK_CODE_MONGOLIAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_MONGOLIAN_value, UBLOCK_MONGOLIAN);
	zend_string *const_BLOCK_CODE_MONGOLIAN_name = zend_string_init_interned("BLOCK_CODE_MONGOLIAN", sizeof("BLOCK_CODE_MONGOLIAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MONGOLIAN_name, &const_BLOCK_CODE_MONGOLIAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MONGOLIAN_name);

	zval const_BLOCK_CODE_LATIN_EXTENDED_ADDITIONAL_value;
	ZVAL_LONG(&const_BLOCK_CODE_LATIN_EXTENDED_ADDITIONAL_value, UBLOCK_LATIN_EXTENDED_ADDITIONAL);
	zend_string *const_BLOCK_CODE_LATIN_EXTENDED_ADDITIONAL_name = zend_string_init_interned("BLOCK_CODE_LATIN_EXTENDED_ADDITIONAL", sizeof("BLOCK_CODE_LATIN_EXTENDED_ADDITIONAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LATIN_EXTENDED_ADDITIONAL_name, &const_BLOCK_CODE_LATIN_EXTENDED_ADDITIONAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LATIN_EXTENDED_ADDITIONAL_name);

	zval const_BLOCK_CODE_GREEK_EXTENDED_value;
	ZVAL_LONG(&const_BLOCK_CODE_GREEK_EXTENDED_value, UBLOCK_GREEK_EXTENDED);
	zend_string *const_BLOCK_CODE_GREEK_EXTENDED_name = zend_string_init_interned("BLOCK_CODE_GREEK_EXTENDED", sizeof("BLOCK_CODE_GREEK_EXTENDED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_GREEK_EXTENDED_name, &const_BLOCK_CODE_GREEK_EXTENDED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_GREEK_EXTENDED_name);

	zval const_BLOCK_CODE_GENERAL_PUNCTUATION_value;
	ZVAL_LONG(&const_BLOCK_CODE_GENERAL_PUNCTUATION_value, UBLOCK_GENERAL_PUNCTUATION);
	zend_string *const_BLOCK_CODE_GENERAL_PUNCTUATION_name = zend_string_init_interned("BLOCK_CODE_GENERAL_PUNCTUATION", sizeof("BLOCK_CODE_GENERAL_PUNCTUATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_GENERAL_PUNCTUATION_name, &const_BLOCK_CODE_GENERAL_PUNCTUATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_GENERAL_PUNCTUATION_name);

	zval const_BLOCK_CODE_SUPERSCRIPTS_AND_SUBSCRIPTS_value;
	ZVAL_LONG(&const_BLOCK_CODE_SUPERSCRIPTS_AND_SUBSCRIPTS_value, UBLOCK_SUPERSCRIPTS_AND_SUBSCRIPTS);
	zend_string *const_BLOCK_CODE_SUPERSCRIPTS_AND_SUBSCRIPTS_name = zend_string_init_interned("BLOCK_CODE_SUPERSCRIPTS_AND_SUBSCRIPTS", sizeof("BLOCK_CODE_SUPERSCRIPTS_AND_SUBSCRIPTS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SUPERSCRIPTS_AND_SUBSCRIPTS_name, &const_BLOCK_CODE_SUPERSCRIPTS_AND_SUBSCRIPTS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SUPERSCRIPTS_AND_SUBSCRIPTS_name);

	zval const_BLOCK_CODE_CURRENCY_SYMBOLS_value;
	ZVAL_LONG(&const_BLOCK_CODE_CURRENCY_SYMBOLS_value, UBLOCK_CURRENCY_SYMBOLS);
	zend_string *const_BLOCK_CODE_CURRENCY_SYMBOLS_name = zend_string_init_interned("BLOCK_CODE_CURRENCY_SYMBOLS", sizeof("BLOCK_CODE_CURRENCY_SYMBOLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CURRENCY_SYMBOLS_name, &const_BLOCK_CODE_CURRENCY_SYMBOLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CURRENCY_SYMBOLS_name);

	zval const_BLOCK_CODE_COMBINING_MARKS_FOR_SYMBOLS_value;
	ZVAL_LONG(&const_BLOCK_CODE_COMBINING_MARKS_FOR_SYMBOLS_value, UBLOCK_COMBINING_MARKS_FOR_SYMBOLS);
	zend_string *const_BLOCK_CODE_COMBINING_MARKS_FOR_SYMBOLS_name = zend_string_init_interned("BLOCK_CODE_COMBINING_MARKS_FOR_SYMBOLS", sizeof("BLOCK_CODE_COMBINING_MARKS_FOR_SYMBOLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_COMBINING_MARKS_FOR_SYMBOLS_name, &const_BLOCK_CODE_COMBINING_MARKS_FOR_SYMBOLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_COMBINING_MARKS_FOR_SYMBOLS_name);

	zval const_BLOCK_CODE_LETTERLIKE_SYMBOLS_value;
	ZVAL_LONG(&const_BLOCK_CODE_LETTERLIKE_SYMBOLS_value, UBLOCK_LETTERLIKE_SYMBOLS);
	zend_string *const_BLOCK_CODE_LETTERLIKE_SYMBOLS_name = zend_string_init_interned("BLOCK_CODE_LETTERLIKE_SYMBOLS", sizeof("BLOCK_CODE_LETTERLIKE_SYMBOLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LETTERLIKE_SYMBOLS_name, &const_BLOCK_CODE_LETTERLIKE_SYMBOLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LETTERLIKE_SYMBOLS_name);

	zval const_BLOCK_CODE_NUMBER_FORMS_value;
	ZVAL_LONG(&const_BLOCK_CODE_NUMBER_FORMS_value, UBLOCK_NUMBER_FORMS);
	zend_string *const_BLOCK_CODE_NUMBER_FORMS_name = zend_string_init_interned("BLOCK_CODE_NUMBER_FORMS", sizeof("BLOCK_CODE_NUMBER_FORMS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_NUMBER_FORMS_name, &const_BLOCK_CODE_NUMBER_FORMS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_NUMBER_FORMS_name);

	zval const_BLOCK_CODE_ARROWS_value;
	ZVAL_LONG(&const_BLOCK_CODE_ARROWS_value, UBLOCK_ARROWS);
	zend_string *const_BLOCK_CODE_ARROWS_name = zend_string_init_interned("BLOCK_CODE_ARROWS", sizeof("BLOCK_CODE_ARROWS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ARROWS_name, &const_BLOCK_CODE_ARROWS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ARROWS_name);

	zval const_BLOCK_CODE_MATHEMATICAL_OPERATORS_value;
	ZVAL_LONG(&const_BLOCK_CODE_MATHEMATICAL_OPERATORS_value, UBLOCK_MATHEMATICAL_OPERATORS);
	zend_string *const_BLOCK_CODE_MATHEMATICAL_OPERATORS_name = zend_string_init_interned("BLOCK_CODE_MATHEMATICAL_OPERATORS", sizeof("BLOCK_CODE_MATHEMATICAL_OPERATORS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MATHEMATICAL_OPERATORS_name, &const_BLOCK_CODE_MATHEMATICAL_OPERATORS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MATHEMATICAL_OPERATORS_name);

	zval const_BLOCK_CODE_MISCELLANEOUS_TECHNICAL_value;
	ZVAL_LONG(&const_BLOCK_CODE_MISCELLANEOUS_TECHNICAL_value, UBLOCK_MISCELLANEOUS_TECHNICAL);
	zend_string *const_BLOCK_CODE_MISCELLANEOUS_TECHNICAL_name = zend_string_init_interned("BLOCK_CODE_MISCELLANEOUS_TECHNICAL", sizeof("BLOCK_CODE_MISCELLANEOUS_TECHNICAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MISCELLANEOUS_TECHNICAL_name, &const_BLOCK_CODE_MISCELLANEOUS_TECHNICAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MISCELLANEOUS_TECHNICAL_name);

	zval const_BLOCK_CODE_CONTROL_PICTURES_value;
	ZVAL_LONG(&const_BLOCK_CODE_CONTROL_PICTURES_value, UBLOCK_CONTROL_PICTURES);
	zend_string *const_BLOCK_CODE_CONTROL_PICTURES_name = zend_string_init_interned("BLOCK_CODE_CONTROL_PICTURES", sizeof("BLOCK_CODE_CONTROL_PICTURES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CONTROL_PICTURES_name, &const_BLOCK_CODE_CONTROL_PICTURES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CONTROL_PICTURES_name);

	zval const_BLOCK_CODE_OPTICAL_CHARACTER_RECOGNITION_value;
	ZVAL_LONG(&const_BLOCK_CODE_OPTICAL_CHARACTER_RECOGNITION_value, UBLOCK_OPTICAL_CHARACTER_RECOGNITION);
	zend_string *const_BLOCK_CODE_OPTICAL_CHARACTER_RECOGNITION_name = zend_string_init_interned("BLOCK_CODE_OPTICAL_CHARACTER_RECOGNITION", sizeof("BLOCK_CODE_OPTICAL_CHARACTER_RECOGNITION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_OPTICAL_CHARACTER_RECOGNITION_name, &const_BLOCK_CODE_OPTICAL_CHARACTER_RECOGNITION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_OPTICAL_CHARACTER_RECOGNITION_name);

	zval const_BLOCK_CODE_ENCLOSED_ALPHANUMERICS_value;
	ZVAL_LONG(&const_BLOCK_CODE_ENCLOSED_ALPHANUMERICS_value, UBLOCK_ENCLOSED_ALPHANUMERICS);
	zend_string *const_BLOCK_CODE_ENCLOSED_ALPHANUMERICS_name = zend_string_init_interned("BLOCK_CODE_ENCLOSED_ALPHANUMERICS", sizeof("BLOCK_CODE_ENCLOSED_ALPHANUMERICS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ENCLOSED_ALPHANUMERICS_name, &const_BLOCK_CODE_ENCLOSED_ALPHANUMERICS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ENCLOSED_ALPHANUMERICS_name);

	zval const_BLOCK_CODE_BOX_DRAWING_value;
	ZVAL_LONG(&const_BLOCK_CODE_BOX_DRAWING_value, UBLOCK_BOX_DRAWING);
	zend_string *const_BLOCK_CODE_BOX_DRAWING_name = zend_string_init_interned("BLOCK_CODE_BOX_DRAWING", sizeof("BLOCK_CODE_BOX_DRAWING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_BOX_DRAWING_name, &const_BLOCK_CODE_BOX_DRAWING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_BOX_DRAWING_name);

	zval const_BLOCK_CODE_BLOCK_ELEMENTS_value;
	ZVAL_LONG(&const_BLOCK_CODE_BLOCK_ELEMENTS_value, UBLOCK_BLOCK_ELEMENTS);
	zend_string *const_BLOCK_CODE_BLOCK_ELEMENTS_name = zend_string_init_interned("BLOCK_CODE_BLOCK_ELEMENTS", sizeof("BLOCK_CODE_BLOCK_ELEMENTS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_BLOCK_ELEMENTS_name, &const_BLOCK_CODE_BLOCK_ELEMENTS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_BLOCK_ELEMENTS_name);

	zval const_BLOCK_CODE_GEOMETRIC_SHAPES_value;
	ZVAL_LONG(&const_BLOCK_CODE_GEOMETRIC_SHAPES_value, UBLOCK_GEOMETRIC_SHAPES);
	zend_string *const_BLOCK_CODE_GEOMETRIC_SHAPES_name = zend_string_init_interned("BLOCK_CODE_GEOMETRIC_SHAPES", sizeof("BLOCK_CODE_GEOMETRIC_SHAPES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_GEOMETRIC_SHAPES_name, &const_BLOCK_CODE_GEOMETRIC_SHAPES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_GEOMETRIC_SHAPES_name);

	zval const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_value;
	ZVAL_LONG(&const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_value, UBLOCK_MISCELLANEOUS_SYMBOLS);
	zend_string *const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_name = zend_string_init_interned("BLOCK_CODE_MISCELLANEOUS_SYMBOLS", sizeof("BLOCK_CODE_MISCELLANEOUS_SYMBOLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_name, &const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_name);

	zval const_BLOCK_CODE_DINGBATS_value;
	ZVAL_LONG(&const_BLOCK_CODE_DINGBATS_value, UBLOCK_DINGBATS);
	zend_string *const_BLOCK_CODE_DINGBATS_name = zend_string_init_interned("BLOCK_CODE_DINGBATS", sizeof("BLOCK_CODE_DINGBATS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_DINGBATS_name, &const_BLOCK_CODE_DINGBATS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_DINGBATS_name);

	zval const_BLOCK_CODE_BRAILLE_PATTERNS_value;
	ZVAL_LONG(&const_BLOCK_CODE_BRAILLE_PATTERNS_value, UBLOCK_BRAILLE_PATTERNS);
	zend_string *const_BLOCK_CODE_BRAILLE_PATTERNS_name = zend_string_init_interned("BLOCK_CODE_BRAILLE_PATTERNS", sizeof("BLOCK_CODE_BRAILLE_PATTERNS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_BRAILLE_PATTERNS_name, &const_BLOCK_CODE_BRAILLE_PATTERNS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_BRAILLE_PATTERNS_name);

	zval const_BLOCK_CODE_CJK_RADICALS_SUPPLEMENT_value;
	ZVAL_LONG(&const_BLOCK_CODE_CJK_RADICALS_SUPPLEMENT_value, UBLOCK_CJK_RADICALS_SUPPLEMENT);
	zend_string *const_BLOCK_CODE_CJK_RADICALS_SUPPLEMENT_name = zend_string_init_interned("BLOCK_CODE_CJK_RADICALS_SUPPLEMENT", sizeof("BLOCK_CODE_CJK_RADICALS_SUPPLEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CJK_RADICALS_SUPPLEMENT_name, &const_BLOCK_CODE_CJK_RADICALS_SUPPLEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CJK_RADICALS_SUPPLEMENT_name);

	zval const_BLOCK_CODE_KANGXI_RADICALS_value;
	ZVAL_LONG(&const_BLOCK_CODE_KANGXI_RADICALS_value, UBLOCK_KANGXI_RADICALS);
	zend_string *const_BLOCK_CODE_KANGXI_RADICALS_name = zend_string_init_interned("BLOCK_CODE_KANGXI_RADICALS", sizeof("BLOCK_CODE_KANGXI_RADICALS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_KANGXI_RADICALS_name, &const_BLOCK_CODE_KANGXI_RADICALS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_KANGXI_RADICALS_name);

	zval const_BLOCK_CODE_IDEOGRAPHIC_DESCRIPTION_CHARACTERS_value;
	ZVAL_LONG(&const_BLOCK_CODE_IDEOGRAPHIC_DESCRIPTION_CHARACTERS_value, UBLOCK_IDEOGRAPHIC_DESCRIPTION_CHARACTERS);
	zend_string *const_BLOCK_CODE_IDEOGRAPHIC_DESCRIPTION_CHARACTERS_name = zend_string_init_interned("BLOCK_CODE_IDEOGRAPHIC_DESCRIPTION_CHARACTERS", sizeof("BLOCK_CODE_IDEOGRAPHIC_DESCRIPTION_CHARACTERS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_IDEOGRAPHIC_DESCRIPTION_CHARACTERS_name, &const_BLOCK_CODE_IDEOGRAPHIC_DESCRIPTION_CHARACTERS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_IDEOGRAPHIC_DESCRIPTION_CHARACTERS_name);

	zval const_BLOCK_CODE_CJK_SYMBOLS_AND_PUNCTUATION_value;
	ZVAL_LONG(&const_BLOCK_CODE_CJK_SYMBOLS_AND_PUNCTUATION_value, UBLOCK_CJK_SYMBOLS_AND_PUNCTUATION);
	zend_string *const_BLOCK_CODE_CJK_SYMBOLS_AND_PUNCTUATION_name = zend_string_init_interned("BLOCK_CODE_CJK_SYMBOLS_AND_PUNCTUATION", sizeof("BLOCK_CODE_CJK_SYMBOLS_AND_PUNCTUATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CJK_SYMBOLS_AND_PUNCTUATION_name, &const_BLOCK_CODE_CJK_SYMBOLS_AND_PUNCTUATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CJK_SYMBOLS_AND_PUNCTUATION_name);

	zval const_BLOCK_CODE_HIRAGANA_value;
	ZVAL_LONG(&const_BLOCK_CODE_HIRAGANA_value, UBLOCK_HIRAGANA);
	zend_string *const_BLOCK_CODE_HIRAGANA_name = zend_string_init_interned("BLOCK_CODE_HIRAGANA", sizeof("BLOCK_CODE_HIRAGANA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_HIRAGANA_name, &const_BLOCK_CODE_HIRAGANA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_HIRAGANA_name);

	zval const_BLOCK_CODE_KATAKANA_value;
	ZVAL_LONG(&const_BLOCK_CODE_KATAKANA_value, UBLOCK_KATAKANA);
	zend_string *const_BLOCK_CODE_KATAKANA_name = zend_string_init_interned("BLOCK_CODE_KATAKANA", sizeof("BLOCK_CODE_KATAKANA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_KATAKANA_name, &const_BLOCK_CODE_KATAKANA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_KATAKANA_name);

	zval const_BLOCK_CODE_BOPOMOFO_value;
	ZVAL_LONG(&const_BLOCK_CODE_BOPOMOFO_value, UBLOCK_BOPOMOFO);
	zend_string *const_BLOCK_CODE_BOPOMOFO_name = zend_string_init_interned("BLOCK_CODE_BOPOMOFO", sizeof("BLOCK_CODE_BOPOMOFO") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_BOPOMOFO_name, &const_BLOCK_CODE_BOPOMOFO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_BOPOMOFO_name);

	zval const_BLOCK_CODE_HANGUL_COMPATIBILITY_JAMO_value;
	ZVAL_LONG(&const_BLOCK_CODE_HANGUL_COMPATIBILITY_JAMO_value, UBLOCK_HANGUL_COMPATIBILITY_JAMO);
	zend_string *const_BLOCK_CODE_HANGUL_COMPATIBILITY_JAMO_name = zend_string_init_interned("BLOCK_CODE_HANGUL_COMPATIBILITY_JAMO", sizeof("BLOCK_CODE_HANGUL_COMPATIBILITY_JAMO") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_HANGUL_COMPATIBILITY_JAMO_name, &const_BLOCK_CODE_HANGUL_COMPATIBILITY_JAMO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_HANGUL_COMPATIBILITY_JAMO_name);

	zval const_BLOCK_CODE_KANBUN_value;
	ZVAL_LONG(&const_BLOCK_CODE_KANBUN_value, UBLOCK_KANBUN);
	zend_string *const_BLOCK_CODE_KANBUN_name = zend_string_init_interned("BLOCK_CODE_KANBUN", sizeof("BLOCK_CODE_KANBUN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_KANBUN_name, &const_BLOCK_CODE_KANBUN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_KANBUN_name);

	zval const_BLOCK_CODE_BOPOMOFO_EXTENDED_value;
	ZVAL_LONG(&const_BLOCK_CODE_BOPOMOFO_EXTENDED_value, UBLOCK_BOPOMOFO_EXTENDED);
	zend_string *const_BLOCK_CODE_BOPOMOFO_EXTENDED_name = zend_string_init_interned("BLOCK_CODE_BOPOMOFO_EXTENDED", sizeof("BLOCK_CODE_BOPOMOFO_EXTENDED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_BOPOMOFO_EXTENDED_name, &const_BLOCK_CODE_BOPOMOFO_EXTENDED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_BOPOMOFO_EXTENDED_name);

	zval const_BLOCK_CODE_ENCLOSED_CJK_LETTERS_AND_MONTHS_value;
	ZVAL_LONG(&const_BLOCK_CODE_ENCLOSED_CJK_LETTERS_AND_MONTHS_value, UBLOCK_ENCLOSED_CJK_LETTERS_AND_MONTHS);
	zend_string *const_BLOCK_CODE_ENCLOSED_CJK_LETTERS_AND_MONTHS_name = zend_string_init_interned("BLOCK_CODE_ENCLOSED_CJK_LETTERS_AND_MONTHS", sizeof("BLOCK_CODE_ENCLOSED_CJK_LETTERS_AND_MONTHS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ENCLOSED_CJK_LETTERS_AND_MONTHS_name, &const_BLOCK_CODE_ENCLOSED_CJK_LETTERS_AND_MONTHS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ENCLOSED_CJK_LETTERS_AND_MONTHS_name);

	zval const_BLOCK_CODE_CJK_COMPATIBILITY_value;
	ZVAL_LONG(&const_BLOCK_CODE_CJK_COMPATIBILITY_value, UBLOCK_CJK_COMPATIBILITY);
	zend_string *const_BLOCK_CODE_CJK_COMPATIBILITY_name = zend_string_init_interned("BLOCK_CODE_CJK_COMPATIBILITY", sizeof("BLOCK_CODE_CJK_COMPATIBILITY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CJK_COMPATIBILITY_name, &const_BLOCK_CODE_CJK_COMPATIBILITY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CJK_COMPATIBILITY_name);

	zval const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A_value;
	ZVAL_LONG(&const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A_value, UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A);
	zend_string *const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A_name = zend_string_init_interned("BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A", sizeof("BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A_name, &const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A_name);

	zval const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_value;
	ZVAL_LONG(&const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_value, UBLOCK_CJK_UNIFIED_IDEOGRAPHS);
	zend_string *const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_name = zend_string_init_interned("BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS", sizeof("BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_name, &const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_name);

	zval const_BLOCK_CODE_YI_SYLLABLES_value;
	ZVAL_LONG(&const_BLOCK_CODE_YI_SYLLABLES_value, UBLOCK_YI_SYLLABLES);
	zend_string *const_BLOCK_CODE_YI_SYLLABLES_name = zend_string_init_interned("BLOCK_CODE_YI_SYLLABLES", sizeof("BLOCK_CODE_YI_SYLLABLES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_YI_SYLLABLES_name, &const_BLOCK_CODE_YI_SYLLABLES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_YI_SYLLABLES_name);

	zval const_BLOCK_CODE_YI_RADICALS_value;
	ZVAL_LONG(&const_BLOCK_CODE_YI_RADICALS_value, UBLOCK_YI_RADICALS);
	zend_string *const_BLOCK_CODE_YI_RADICALS_name = zend_string_init_interned("BLOCK_CODE_YI_RADICALS", sizeof("BLOCK_CODE_YI_RADICALS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_YI_RADICALS_name, &const_BLOCK_CODE_YI_RADICALS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_YI_RADICALS_name);

	zval const_BLOCK_CODE_HANGUL_SYLLABLES_value;
	ZVAL_LONG(&const_BLOCK_CODE_HANGUL_SYLLABLES_value, UBLOCK_HANGUL_SYLLABLES);
	zend_string *const_BLOCK_CODE_HANGUL_SYLLABLES_name = zend_string_init_interned("BLOCK_CODE_HANGUL_SYLLABLES", sizeof("BLOCK_CODE_HANGUL_SYLLABLES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_HANGUL_SYLLABLES_name, &const_BLOCK_CODE_HANGUL_SYLLABLES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_HANGUL_SYLLABLES_name);

	zval const_BLOCK_CODE_HIGH_SURROGATES_value;
	ZVAL_LONG(&const_BLOCK_CODE_HIGH_SURROGATES_value, UBLOCK_HIGH_SURROGATES);
	zend_string *const_BLOCK_CODE_HIGH_SURROGATES_name = zend_string_init_interned("BLOCK_CODE_HIGH_SURROGATES", sizeof("BLOCK_CODE_HIGH_SURROGATES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_HIGH_SURROGATES_name, &const_BLOCK_CODE_HIGH_SURROGATES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_HIGH_SURROGATES_name);

	zval const_BLOCK_CODE_HIGH_PRIVATE_USE_SURROGATES_value;
	ZVAL_LONG(&const_BLOCK_CODE_HIGH_PRIVATE_USE_SURROGATES_value, UBLOCK_HIGH_PRIVATE_USE_SURROGATES);
	zend_string *const_BLOCK_CODE_HIGH_PRIVATE_USE_SURROGATES_name = zend_string_init_interned("BLOCK_CODE_HIGH_PRIVATE_USE_SURROGATES", sizeof("BLOCK_CODE_HIGH_PRIVATE_USE_SURROGATES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_HIGH_PRIVATE_USE_SURROGATES_name, &const_BLOCK_CODE_HIGH_PRIVATE_USE_SURROGATES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_HIGH_PRIVATE_USE_SURROGATES_name);

	zval const_BLOCK_CODE_LOW_SURROGATES_value;
	ZVAL_LONG(&const_BLOCK_CODE_LOW_SURROGATES_value, UBLOCK_LOW_SURROGATES);
	zend_string *const_BLOCK_CODE_LOW_SURROGATES_name = zend_string_init_interned("BLOCK_CODE_LOW_SURROGATES", sizeof("BLOCK_CODE_LOW_SURROGATES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LOW_SURROGATES_name, &const_BLOCK_CODE_LOW_SURROGATES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LOW_SURROGATES_name);

	zval const_BLOCK_CODE_PRIVATE_USE_AREA_value;
	ZVAL_LONG(&const_BLOCK_CODE_PRIVATE_USE_AREA_value, UBLOCK_PRIVATE_USE_AREA);
	zend_string *const_BLOCK_CODE_PRIVATE_USE_AREA_name = zend_string_init_interned("BLOCK_CODE_PRIVATE_USE_AREA", sizeof("BLOCK_CODE_PRIVATE_USE_AREA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_PRIVATE_USE_AREA_name, &const_BLOCK_CODE_PRIVATE_USE_AREA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_PRIVATE_USE_AREA_name);

	zval const_BLOCK_CODE_PRIVATE_USE_value;
	ZVAL_LONG(&const_BLOCK_CODE_PRIVATE_USE_value, UBLOCK_PRIVATE_USE);
	zend_string *const_BLOCK_CODE_PRIVATE_USE_name = zend_string_init_interned("BLOCK_CODE_PRIVATE_USE", sizeof("BLOCK_CODE_PRIVATE_USE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_PRIVATE_USE_name, &const_BLOCK_CODE_PRIVATE_USE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_PRIVATE_USE_name);

	zval const_BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS_value;
	ZVAL_LONG(&const_BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS_value, UBLOCK_CJK_COMPATIBILITY_IDEOGRAPHS);
	zend_string *const_BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS_name = zend_string_init_interned("BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS", sizeof("BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS_name, &const_BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS_name);

	zval const_BLOCK_CODE_ALPHABETIC_PRESENTATION_FORMS_value;
	ZVAL_LONG(&const_BLOCK_CODE_ALPHABETIC_PRESENTATION_FORMS_value, UBLOCK_ALPHABETIC_PRESENTATION_FORMS);
	zend_string *const_BLOCK_CODE_ALPHABETIC_PRESENTATION_FORMS_name = zend_string_init_interned("BLOCK_CODE_ALPHABETIC_PRESENTATION_FORMS", sizeof("BLOCK_CODE_ALPHABETIC_PRESENTATION_FORMS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ALPHABETIC_PRESENTATION_FORMS_name, &const_BLOCK_CODE_ALPHABETIC_PRESENTATION_FORMS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ALPHABETIC_PRESENTATION_FORMS_name);

	zval const_BLOCK_CODE_ARABIC_PRESENTATION_FORMS_A_value;
	ZVAL_LONG(&const_BLOCK_CODE_ARABIC_PRESENTATION_FORMS_A_value, UBLOCK_ARABIC_PRESENTATION_FORMS_A);
	zend_string *const_BLOCK_CODE_ARABIC_PRESENTATION_FORMS_A_name = zend_string_init_interned("BLOCK_CODE_ARABIC_PRESENTATION_FORMS_A", sizeof("BLOCK_CODE_ARABIC_PRESENTATION_FORMS_A") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ARABIC_PRESENTATION_FORMS_A_name, &const_BLOCK_CODE_ARABIC_PRESENTATION_FORMS_A_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ARABIC_PRESENTATION_FORMS_A_name);

	zval const_BLOCK_CODE_COMBINING_HALF_MARKS_value;
	ZVAL_LONG(&const_BLOCK_CODE_COMBINING_HALF_MARKS_value, UBLOCK_COMBINING_HALF_MARKS);
	zend_string *const_BLOCK_CODE_COMBINING_HALF_MARKS_name = zend_string_init_interned("BLOCK_CODE_COMBINING_HALF_MARKS", sizeof("BLOCK_CODE_COMBINING_HALF_MARKS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_COMBINING_HALF_MARKS_name, &const_BLOCK_CODE_COMBINING_HALF_MARKS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_COMBINING_HALF_MARKS_name);

	zval const_BLOCK_CODE_CJK_COMPATIBILITY_FORMS_value;
	ZVAL_LONG(&const_BLOCK_CODE_CJK_COMPATIBILITY_FORMS_value, UBLOCK_CJK_COMPATIBILITY_FORMS);
	zend_string *const_BLOCK_CODE_CJK_COMPATIBILITY_FORMS_name = zend_string_init_interned("BLOCK_CODE_CJK_COMPATIBILITY_FORMS", sizeof("BLOCK_CODE_CJK_COMPATIBILITY_FORMS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CJK_COMPATIBILITY_FORMS_name, &const_BLOCK_CODE_CJK_COMPATIBILITY_FORMS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CJK_COMPATIBILITY_FORMS_name);

	zval const_BLOCK_CODE_SMALL_FORM_VARIANTS_value;
	ZVAL_LONG(&const_BLOCK_CODE_SMALL_FORM_VARIANTS_value, UBLOCK_SMALL_FORM_VARIANTS);
	zend_string *const_BLOCK_CODE_SMALL_FORM_VARIANTS_name = zend_string_init_interned("BLOCK_CODE_SMALL_FORM_VARIANTS", sizeof("BLOCK_CODE_SMALL_FORM_VARIANTS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SMALL_FORM_VARIANTS_name, &const_BLOCK_CODE_SMALL_FORM_VARIANTS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SMALL_FORM_VARIANTS_name);

	zval const_BLOCK_CODE_ARABIC_PRESENTATION_FORMS_B_value;
	ZVAL_LONG(&const_BLOCK_CODE_ARABIC_PRESENTATION_FORMS_B_value, UBLOCK_ARABIC_PRESENTATION_FORMS_B);
	zend_string *const_BLOCK_CODE_ARABIC_PRESENTATION_FORMS_B_name = zend_string_init_interned("BLOCK_CODE_ARABIC_PRESENTATION_FORMS_B", sizeof("BLOCK_CODE_ARABIC_PRESENTATION_FORMS_B") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ARABIC_PRESENTATION_FORMS_B_name, &const_BLOCK_CODE_ARABIC_PRESENTATION_FORMS_B_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ARABIC_PRESENTATION_FORMS_B_name);

	zval const_BLOCK_CODE_SPECIALS_value;
	ZVAL_LONG(&const_BLOCK_CODE_SPECIALS_value, UBLOCK_SPECIALS);
	zend_string *const_BLOCK_CODE_SPECIALS_name = zend_string_init_interned("BLOCK_CODE_SPECIALS", sizeof("BLOCK_CODE_SPECIALS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SPECIALS_name, &const_BLOCK_CODE_SPECIALS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SPECIALS_name);

	zval const_BLOCK_CODE_HALFWIDTH_AND_FULLWIDTH_FORMS_value;
	ZVAL_LONG(&const_BLOCK_CODE_HALFWIDTH_AND_FULLWIDTH_FORMS_value, UBLOCK_HALFWIDTH_AND_FULLWIDTH_FORMS);
	zend_string *const_BLOCK_CODE_HALFWIDTH_AND_FULLWIDTH_FORMS_name = zend_string_init_interned("BLOCK_CODE_HALFWIDTH_AND_FULLWIDTH_FORMS", sizeof("BLOCK_CODE_HALFWIDTH_AND_FULLWIDTH_FORMS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_HALFWIDTH_AND_FULLWIDTH_FORMS_name, &const_BLOCK_CODE_HALFWIDTH_AND_FULLWIDTH_FORMS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_HALFWIDTH_AND_FULLWIDTH_FORMS_name);

	zval const_BLOCK_CODE_OLD_ITALIC_value;
	ZVAL_LONG(&const_BLOCK_CODE_OLD_ITALIC_value, UBLOCK_OLD_ITALIC);
	zend_string *const_BLOCK_CODE_OLD_ITALIC_name = zend_string_init_interned("BLOCK_CODE_OLD_ITALIC", sizeof("BLOCK_CODE_OLD_ITALIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_OLD_ITALIC_name, &const_BLOCK_CODE_OLD_ITALIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_OLD_ITALIC_name);

	zval const_BLOCK_CODE_GOTHIC_value;
	ZVAL_LONG(&const_BLOCK_CODE_GOTHIC_value, UBLOCK_GOTHIC);
	zend_string *const_BLOCK_CODE_GOTHIC_name = zend_string_init_interned("BLOCK_CODE_GOTHIC", sizeof("BLOCK_CODE_GOTHIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_GOTHIC_name, &const_BLOCK_CODE_GOTHIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_GOTHIC_name);

	zval const_BLOCK_CODE_DESERET_value;
	ZVAL_LONG(&const_BLOCK_CODE_DESERET_value, UBLOCK_DESERET);
	zend_string *const_BLOCK_CODE_DESERET_name = zend_string_init_interned("BLOCK_CODE_DESERET", sizeof("BLOCK_CODE_DESERET") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_DESERET_name, &const_BLOCK_CODE_DESERET_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_DESERET_name);

	zval const_BLOCK_CODE_BYZANTINE_MUSICAL_SYMBOLS_value;
	ZVAL_LONG(&const_BLOCK_CODE_BYZANTINE_MUSICAL_SYMBOLS_value, UBLOCK_BYZANTINE_MUSICAL_SYMBOLS);
	zend_string *const_BLOCK_CODE_BYZANTINE_MUSICAL_SYMBOLS_name = zend_string_init_interned("BLOCK_CODE_BYZANTINE_MUSICAL_SYMBOLS", sizeof("BLOCK_CODE_BYZANTINE_MUSICAL_SYMBOLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_BYZANTINE_MUSICAL_SYMBOLS_name, &const_BLOCK_CODE_BYZANTINE_MUSICAL_SYMBOLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_BYZANTINE_MUSICAL_SYMBOLS_name);

	zval const_BLOCK_CODE_MUSICAL_SYMBOLS_value;
	ZVAL_LONG(&const_BLOCK_CODE_MUSICAL_SYMBOLS_value, UBLOCK_MUSICAL_SYMBOLS);
	zend_string *const_BLOCK_CODE_MUSICAL_SYMBOLS_name = zend_string_init_interned("BLOCK_CODE_MUSICAL_SYMBOLS", sizeof("BLOCK_CODE_MUSICAL_SYMBOLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MUSICAL_SYMBOLS_name, &const_BLOCK_CODE_MUSICAL_SYMBOLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MUSICAL_SYMBOLS_name);

	zval const_BLOCK_CODE_MATHEMATICAL_ALPHANUMERIC_SYMBOLS_value;
	ZVAL_LONG(&const_BLOCK_CODE_MATHEMATICAL_ALPHANUMERIC_SYMBOLS_value, UBLOCK_MATHEMATICAL_ALPHANUMERIC_SYMBOLS);
	zend_string *const_BLOCK_CODE_MATHEMATICAL_ALPHANUMERIC_SYMBOLS_name = zend_string_init_interned("BLOCK_CODE_MATHEMATICAL_ALPHANUMERIC_SYMBOLS", sizeof("BLOCK_CODE_MATHEMATICAL_ALPHANUMERIC_SYMBOLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MATHEMATICAL_ALPHANUMERIC_SYMBOLS_name, &const_BLOCK_CODE_MATHEMATICAL_ALPHANUMERIC_SYMBOLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MATHEMATICAL_ALPHANUMERIC_SYMBOLS_name);

	zval const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B_value;
	ZVAL_LONG(&const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B_value, UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B);
	zend_string *const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B_name = zend_string_init_interned("BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B", sizeof("BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B_name, &const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B_name);

	zval const_BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT_value;
	ZVAL_LONG(&const_BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT_value, UBLOCK_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT);
	zend_string *const_BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT_name = zend_string_init_interned("BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT", sizeof("BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT_name, &const_BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT_name);

	zval const_BLOCK_CODE_TAGS_value;
	ZVAL_LONG(&const_BLOCK_CODE_TAGS_value, UBLOCK_TAGS);
	zend_string *const_BLOCK_CODE_TAGS_name = zend_string_init_interned("BLOCK_CODE_TAGS", sizeof("BLOCK_CODE_TAGS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_TAGS_name, &const_BLOCK_CODE_TAGS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_TAGS_name);

	zval const_BLOCK_CODE_CYRILLIC_SUPPLEMENT_value;
	ZVAL_LONG(&const_BLOCK_CODE_CYRILLIC_SUPPLEMENT_value, UBLOCK_CYRILLIC_SUPPLEMENT);
	zend_string *const_BLOCK_CODE_CYRILLIC_SUPPLEMENT_name = zend_string_init_interned("BLOCK_CODE_CYRILLIC_SUPPLEMENT", sizeof("BLOCK_CODE_CYRILLIC_SUPPLEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CYRILLIC_SUPPLEMENT_name, &const_BLOCK_CODE_CYRILLIC_SUPPLEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CYRILLIC_SUPPLEMENT_name);

	zval const_BLOCK_CODE_CYRILLIC_SUPPLEMENTARY_value;
	ZVAL_LONG(&const_BLOCK_CODE_CYRILLIC_SUPPLEMENTARY_value, UBLOCK_CYRILLIC_SUPPLEMENTARY);
	zend_string *const_BLOCK_CODE_CYRILLIC_SUPPLEMENTARY_name = zend_string_init_interned("BLOCK_CODE_CYRILLIC_SUPPLEMENTARY", sizeof("BLOCK_CODE_CYRILLIC_SUPPLEMENTARY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CYRILLIC_SUPPLEMENTARY_name, &const_BLOCK_CODE_CYRILLIC_SUPPLEMENTARY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CYRILLIC_SUPPLEMENTARY_name);

	zval const_BLOCK_CODE_TAGALOG_value;
	ZVAL_LONG(&const_BLOCK_CODE_TAGALOG_value, UBLOCK_TAGALOG);
	zend_string *const_BLOCK_CODE_TAGALOG_name = zend_string_init_interned("BLOCK_CODE_TAGALOG", sizeof("BLOCK_CODE_TAGALOG") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_TAGALOG_name, &const_BLOCK_CODE_TAGALOG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_TAGALOG_name);

	zval const_BLOCK_CODE_HANUNOO_value;
	ZVAL_LONG(&const_BLOCK_CODE_HANUNOO_value, UBLOCK_HANUNOO);
	zend_string *const_BLOCK_CODE_HANUNOO_name = zend_string_init_interned("BLOCK_CODE_HANUNOO", sizeof("BLOCK_CODE_HANUNOO") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_HANUNOO_name, &const_BLOCK_CODE_HANUNOO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_HANUNOO_name);

	zval const_BLOCK_CODE_BUHID_value;
	ZVAL_LONG(&const_BLOCK_CODE_BUHID_value, UBLOCK_BUHID);
	zend_string *const_BLOCK_CODE_BUHID_name = zend_string_init_interned("BLOCK_CODE_BUHID", sizeof("BLOCK_CODE_BUHID") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_BUHID_name, &const_BLOCK_CODE_BUHID_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_BUHID_name);

	zval const_BLOCK_CODE_TAGBANWA_value;
	ZVAL_LONG(&const_BLOCK_CODE_TAGBANWA_value, UBLOCK_TAGBANWA);
	zend_string *const_BLOCK_CODE_TAGBANWA_name = zend_string_init_interned("BLOCK_CODE_TAGBANWA", sizeof("BLOCK_CODE_TAGBANWA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_TAGBANWA_name, &const_BLOCK_CODE_TAGBANWA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_TAGBANWA_name);

	zval const_BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A_value;
	ZVAL_LONG(&const_BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A_value, UBLOCK_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A);
	zend_string *const_BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A_name = zend_string_init_interned("BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A", sizeof("BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A_name, &const_BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A_name);

	zval const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_A_value;
	ZVAL_LONG(&const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_A_value, UBLOCK_SUPPLEMENTAL_ARROWS_A);
	zend_string *const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_A_name = zend_string_init_interned("BLOCK_CODE_SUPPLEMENTAL_ARROWS_A", sizeof("BLOCK_CODE_SUPPLEMENTAL_ARROWS_A") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_A_name, &const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_A_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_A_name);

	zval const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_B_value;
	ZVAL_LONG(&const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_B_value, UBLOCK_SUPPLEMENTAL_ARROWS_B);
	zend_string *const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_B_name = zend_string_init_interned("BLOCK_CODE_SUPPLEMENTAL_ARROWS_B", sizeof("BLOCK_CODE_SUPPLEMENTAL_ARROWS_B") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_B_name, &const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_B_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_B_name);

	zval const_BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B_value;
	ZVAL_LONG(&const_BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B_value, UBLOCK_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B);
	zend_string *const_BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B_name = zend_string_init_interned("BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B", sizeof("BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B_name, &const_BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B_name);

	zval const_BLOCK_CODE_SUPPLEMENTAL_MATHEMATICAL_OPERATORS_value;
	ZVAL_LONG(&const_BLOCK_CODE_SUPPLEMENTAL_MATHEMATICAL_OPERATORS_value, UBLOCK_SUPPLEMENTAL_MATHEMATICAL_OPERATORS);
	zend_string *const_BLOCK_CODE_SUPPLEMENTAL_MATHEMATICAL_OPERATORS_name = zend_string_init_interned("BLOCK_CODE_SUPPLEMENTAL_MATHEMATICAL_OPERATORS", sizeof("BLOCK_CODE_SUPPLEMENTAL_MATHEMATICAL_OPERATORS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SUPPLEMENTAL_MATHEMATICAL_OPERATORS_name, &const_BLOCK_CODE_SUPPLEMENTAL_MATHEMATICAL_OPERATORS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SUPPLEMENTAL_MATHEMATICAL_OPERATORS_name);

	zval const_BLOCK_CODE_KATAKANA_PHONETIC_EXTENSIONS_value;
	ZVAL_LONG(&const_BLOCK_CODE_KATAKANA_PHONETIC_EXTENSIONS_value, UBLOCK_KATAKANA_PHONETIC_EXTENSIONS);
	zend_string *const_BLOCK_CODE_KATAKANA_PHONETIC_EXTENSIONS_name = zend_string_init_interned("BLOCK_CODE_KATAKANA_PHONETIC_EXTENSIONS", sizeof("BLOCK_CODE_KATAKANA_PHONETIC_EXTENSIONS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_KATAKANA_PHONETIC_EXTENSIONS_name, &const_BLOCK_CODE_KATAKANA_PHONETIC_EXTENSIONS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_KATAKANA_PHONETIC_EXTENSIONS_name);

	zval const_BLOCK_CODE_VARIATION_SELECTORS_value;
	ZVAL_LONG(&const_BLOCK_CODE_VARIATION_SELECTORS_value, UBLOCK_VARIATION_SELECTORS);
	zend_string *const_BLOCK_CODE_VARIATION_SELECTORS_name = zend_string_init_interned("BLOCK_CODE_VARIATION_SELECTORS", sizeof("BLOCK_CODE_VARIATION_SELECTORS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_VARIATION_SELECTORS_name, &const_BLOCK_CODE_VARIATION_SELECTORS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_VARIATION_SELECTORS_name);

	zval const_BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_A_value;
	ZVAL_LONG(&const_BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_A_value, UBLOCK_SUPPLEMENTARY_PRIVATE_USE_AREA_A);
	zend_string *const_BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_A_name = zend_string_init_interned("BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_A", sizeof("BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_A") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_A_name, &const_BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_A_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_A_name);

	zval const_BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_B_value;
	ZVAL_LONG(&const_BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_B_value, UBLOCK_SUPPLEMENTARY_PRIVATE_USE_AREA_B);
	zend_string *const_BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_B_name = zend_string_init_interned("BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_B", sizeof("BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_B") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_B_name, &const_BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_B_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_B_name);

	zval const_BLOCK_CODE_LIMBU_value;
	ZVAL_LONG(&const_BLOCK_CODE_LIMBU_value, UBLOCK_LIMBU);
	zend_string *const_BLOCK_CODE_LIMBU_name = zend_string_init_interned("BLOCK_CODE_LIMBU", sizeof("BLOCK_CODE_LIMBU") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LIMBU_name, &const_BLOCK_CODE_LIMBU_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LIMBU_name);

	zval const_BLOCK_CODE_TAI_LE_value;
	ZVAL_LONG(&const_BLOCK_CODE_TAI_LE_value, UBLOCK_TAI_LE);
	zend_string *const_BLOCK_CODE_TAI_LE_name = zend_string_init_interned("BLOCK_CODE_TAI_LE", sizeof("BLOCK_CODE_TAI_LE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_TAI_LE_name, &const_BLOCK_CODE_TAI_LE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_TAI_LE_name);

	zval const_BLOCK_CODE_KHMER_SYMBOLS_value;
	ZVAL_LONG(&const_BLOCK_CODE_KHMER_SYMBOLS_value, UBLOCK_KHMER_SYMBOLS);
	zend_string *const_BLOCK_CODE_KHMER_SYMBOLS_name = zend_string_init_interned("BLOCK_CODE_KHMER_SYMBOLS", sizeof("BLOCK_CODE_KHMER_SYMBOLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_KHMER_SYMBOLS_name, &const_BLOCK_CODE_KHMER_SYMBOLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_KHMER_SYMBOLS_name);

	zval const_BLOCK_CODE_PHONETIC_EXTENSIONS_value;
	ZVAL_LONG(&const_BLOCK_CODE_PHONETIC_EXTENSIONS_value, UBLOCK_PHONETIC_EXTENSIONS);
	zend_string *const_BLOCK_CODE_PHONETIC_EXTENSIONS_name = zend_string_init_interned("BLOCK_CODE_PHONETIC_EXTENSIONS", sizeof("BLOCK_CODE_PHONETIC_EXTENSIONS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_PHONETIC_EXTENSIONS_name, &const_BLOCK_CODE_PHONETIC_EXTENSIONS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_PHONETIC_EXTENSIONS_name);

	zval const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_ARROWS_value;
	ZVAL_LONG(&const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_ARROWS_value, UBLOCK_MISCELLANEOUS_SYMBOLS_AND_ARROWS);
	zend_string *const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_ARROWS_name = zend_string_init_interned("BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_ARROWS", sizeof("BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_ARROWS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_ARROWS_name, &const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_ARROWS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_ARROWS_name);

	zval const_BLOCK_CODE_YIJING_HEXAGRAM_SYMBOLS_value;
	ZVAL_LONG(&const_BLOCK_CODE_YIJING_HEXAGRAM_SYMBOLS_value, UBLOCK_YIJING_HEXAGRAM_SYMBOLS);
	zend_string *const_BLOCK_CODE_YIJING_HEXAGRAM_SYMBOLS_name = zend_string_init_interned("BLOCK_CODE_YIJING_HEXAGRAM_SYMBOLS", sizeof("BLOCK_CODE_YIJING_HEXAGRAM_SYMBOLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_YIJING_HEXAGRAM_SYMBOLS_name, &const_BLOCK_CODE_YIJING_HEXAGRAM_SYMBOLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_YIJING_HEXAGRAM_SYMBOLS_name);

	zval const_BLOCK_CODE_LINEAR_B_SYLLABARY_value;
	ZVAL_LONG(&const_BLOCK_CODE_LINEAR_B_SYLLABARY_value, UBLOCK_LINEAR_B_SYLLABARY);
	zend_string *const_BLOCK_CODE_LINEAR_B_SYLLABARY_name = zend_string_init_interned("BLOCK_CODE_LINEAR_B_SYLLABARY", sizeof("BLOCK_CODE_LINEAR_B_SYLLABARY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LINEAR_B_SYLLABARY_name, &const_BLOCK_CODE_LINEAR_B_SYLLABARY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LINEAR_B_SYLLABARY_name);

	zval const_BLOCK_CODE_LINEAR_B_IDEOGRAMS_value;
	ZVAL_LONG(&const_BLOCK_CODE_LINEAR_B_IDEOGRAMS_value, UBLOCK_LINEAR_B_IDEOGRAMS);
	zend_string *const_BLOCK_CODE_LINEAR_B_IDEOGRAMS_name = zend_string_init_interned("BLOCK_CODE_LINEAR_B_IDEOGRAMS", sizeof("BLOCK_CODE_LINEAR_B_IDEOGRAMS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LINEAR_B_IDEOGRAMS_name, &const_BLOCK_CODE_LINEAR_B_IDEOGRAMS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LINEAR_B_IDEOGRAMS_name);

	zval const_BLOCK_CODE_AEGEAN_NUMBERS_value;
	ZVAL_LONG(&const_BLOCK_CODE_AEGEAN_NUMBERS_value, UBLOCK_AEGEAN_NUMBERS);
	zend_string *const_BLOCK_CODE_AEGEAN_NUMBERS_name = zend_string_init_interned("BLOCK_CODE_AEGEAN_NUMBERS", sizeof("BLOCK_CODE_AEGEAN_NUMBERS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_AEGEAN_NUMBERS_name, &const_BLOCK_CODE_AEGEAN_NUMBERS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_AEGEAN_NUMBERS_name);

	zval const_BLOCK_CODE_UGARITIC_value;
	ZVAL_LONG(&const_BLOCK_CODE_UGARITIC_value, UBLOCK_UGARITIC);
	zend_string *const_BLOCK_CODE_UGARITIC_name = zend_string_init_interned("BLOCK_CODE_UGARITIC", sizeof("BLOCK_CODE_UGARITIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_UGARITIC_name, &const_BLOCK_CODE_UGARITIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_UGARITIC_name);

	zval const_BLOCK_CODE_SHAVIAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_SHAVIAN_value, UBLOCK_SHAVIAN);
	zend_string *const_BLOCK_CODE_SHAVIAN_name = zend_string_init_interned("BLOCK_CODE_SHAVIAN", sizeof("BLOCK_CODE_SHAVIAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SHAVIAN_name, &const_BLOCK_CODE_SHAVIAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SHAVIAN_name);

	zval const_BLOCK_CODE_OSMANYA_value;
	ZVAL_LONG(&const_BLOCK_CODE_OSMANYA_value, UBLOCK_OSMANYA);
	zend_string *const_BLOCK_CODE_OSMANYA_name = zend_string_init_interned("BLOCK_CODE_OSMANYA", sizeof("BLOCK_CODE_OSMANYA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_OSMANYA_name, &const_BLOCK_CODE_OSMANYA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_OSMANYA_name);

	zval const_BLOCK_CODE_CYPRIOT_SYLLABARY_value;
	ZVAL_LONG(&const_BLOCK_CODE_CYPRIOT_SYLLABARY_value, UBLOCK_CYPRIOT_SYLLABARY);
	zend_string *const_BLOCK_CODE_CYPRIOT_SYLLABARY_name = zend_string_init_interned("BLOCK_CODE_CYPRIOT_SYLLABARY", sizeof("BLOCK_CODE_CYPRIOT_SYLLABARY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CYPRIOT_SYLLABARY_name, &const_BLOCK_CODE_CYPRIOT_SYLLABARY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CYPRIOT_SYLLABARY_name);

	zval const_BLOCK_CODE_TAI_XUAN_JING_SYMBOLS_value;
	ZVAL_LONG(&const_BLOCK_CODE_TAI_XUAN_JING_SYMBOLS_value, UBLOCK_TAI_XUAN_JING_SYMBOLS);
	zend_string *const_BLOCK_CODE_TAI_XUAN_JING_SYMBOLS_name = zend_string_init_interned("BLOCK_CODE_TAI_XUAN_JING_SYMBOLS", sizeof("BLOCK_CODE_TAI_XUAN_JING_SYMBOLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_TAI_XUAN_JING_SYMBOLS_name, &const_BLOCK_CODE_TAI_XUAN_JING_SYMBOLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_TAI_XUAN_JING_SYMBOLS_name);

	zval const_BLOCK_CODE_VARIATION_SELECTORS_SUPPLEMENT_value;
	ZVAL_LONG(&const_BLOCK_CODE_VARIATION_SELECTORS_SUPPLEMENT_value, UBLOCK_VARIATION_SELECTORS_SUPPLEMENT);
	zend_string *const_BLOCK_CODE_VARIATION_SELECTORS_SUPPLEMENT_name = zend_string_init_interned("BLOCK_CODE_VARIATION_SELECTORS_SUPPLEMENT", sizeof("BLOCK_CODE_VARIATION_SELECTORS_SUPPLEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_VARIATION_SELECTORS_SUPPLEMENT_name, &const_BLOCK_CODE_VARIATION_SELECTORS_SUPPLEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_VARIATION_SELECTORS_SUPPLEMENT_name);

	zval const_BLOCK_CODE_ANCIENT_GREEK_MUSICAL_NOTATION_value;
	ZVAL_LONG(&const_BLOCK_CODE_ANCIENT_GREEK_MUSICAL_NOTATION_value, UBLOCK_ANCIENT_GREEK_MUSICAL_NOTATION);
	zend_string *const_BLOCK_CODE_ANCIENT_GREEK_MUSICAL_NOTATION_name = zend_string_init_interned("BLOCK_CODE_ANCIENT_GREEK_MUSICAL_NOTATION", sizeof("BLOCK_CODE_ANCIENT_GREEK_MUSICAL_NOTATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ANCIENT_GREEK_MUSICAL_NOTATION_name, &const_BLOCK_CODE_ANCIENT_GREEK_MUSICAL_NOTATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ANCIENT_GREEK_MUSICAL_NOTATION_name);

	zval const_BLOCK_CODE_ANCIENT_GREEK_NUMBERS_value;
	ZVAL_LONG(&const_BLOCK_CODE_ANCIENT_GREEK_NUMBERS_value, UBLOCK_ANCIENT_GREEK_NUMBERS);
	zend_string *const_BLOCK_CODE_ANCIENT_GREEK_NUMBERS_name = zend_string_init_interned("BLOCK_CODE_ANCIENT_GREEK_NUMBERS", sizeof("BLOCK_CODE_ANCIENT_GREEK_NUMBERS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ANCIENT_GREEK_NUMBERS_name, &const_BLOCK_CODE_ANCIENT_GREEK_NUMBERS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ANCIENT_GREEK_NUMBERS_name);

	zval const_BLOCK_CODE_ARABIC_SUPPLEMENT_value;
	ZVAL_LONG(&const_BLOCK_CODE_ARABIC_SUPPLEMENT_value, UBLOCK_ARABIC_SUPPLEMENT);
	zend_string *const_BLOCK_CODE_ARABIC_SUPPLEMENT_name = zend_string_init_interned("BLOCK_CODE_ARABIC_SUPPLEMENT", sizeof("BLOCK_CODE_ARABIC_SUPPLEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ARABIC_SUPPLEMENT_name, &const_BLOCK_CODE_ARABIC_SUPPLEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ARABIC_SUPPLEMENT_name);

	zval const_BLOCK_CODE_BUGINESE_value;
	ZVAL_LONG(&const_BLOCK_CODE_BUGINESE_value, UBLOCK_BUGINESE);
	zend_string *const_BLOCK_CODE_BUGINESE_name = zend_string_init_interned("BLOCK_CODE_BUGINESE", sizeof("BLOCK_CODE_BUGINESE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_BUGINESE_name, &const_BLOCK_CODE_BUGINESE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_BUGINESE_name);

	zval const_BLOCK_CODE_CJK_STROKES_value;
	ZVAL_LONG(&const_BLOCK_CODE_CJK_STROKES_value, UBLOCK_CJK_STROKES);
	zend_string *const_BLOCK_CODE_CJK_STROKES_name = zend_string_init_interned("BLOCK_CODE_CJK_STROKES", sizeof("BLOCK_CODE_CJK_STROKES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CJK_STROKES_name, &const_BLOCK_CODE_CJK_STROKES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CJK_STROKES_name);

	zval const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_SUPPLEMENT_value;
	ZVAL_LONG(&const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_SUPPLEMENT_value, UBLOCK_COMBINING_DIACRITICAL_MARKS_SUPPLEMENT);
	zend_string *const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_SUPPLEMENT_name = zend_string_init_interned("BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_SUPPLEMENT", sizeof("BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_SUPPLEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_SUPPLEMENT_name, &const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_SUPPLEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_SUPPLEMENT_name);

	zval const_BLOCK_CODE_COPTIC_value;
	ZVAL_LONG(&const_BLOCK_CODE_COPTIC_value, UBLOCK_COPTIC);
	zend_string *const_BLOCK_CODE_COPTIC_name = zend_string_init_interned("BLOCK_CODE_COPTIC", sizeof("BLOCK_CODE_COPTIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_COPTIC_name, &const_BLOCK_CODE_COPTIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_COPTIC_name);

	zval const_BLOCK_CODE_ETHIOPIC_EXTENDED_value;
	ZVAL_LONG(&const_BLOCK_CODE_ETHIOPIC_EXTENDED_value, UBLOCK_ETHIOPIC_EXTENDED);
	zend_string *const_BLOCK_CODE_ETHIOPIC_EXTENDED_name = zend_string_init_interned("BLOCK_CODE_ETHIOPIC_EXTENDED", sizeof("BLOCK_CODE_ETHIOPIC_EXTENDED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ETHIOPIC_EXTENDED_name, &const_BLOCK_CODE_ETHIOPIC_EXTENDED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ETHIOPIC_EXTENDED_name);

	zval const_BLOCK_CODE_ETHIOPIC_SUPPLEMENT_value;
	ZVAL_LONG(&const_BLOCK_CODE_ETHIOPIC_SUPPLEMENT_value, UBLOCK_ETHIOPIC_SUPPLEMENT);
	zend_string *const_BLOCK_CODE_ETHIOPIC_SUPPLEMENT_name = zend_string_init_interned("BLOCK_CODE_ETHIOPIC_SUPPLEMENT", sizeof("BLOCK_CODE_ETHIOPIC_SUPPLEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ETHIOPIC_SUPPLEMENT_name, &const_BLOCK_CODE_ETHIOPIC_SUPPLEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ETHIOPIC_SUPPLEMENT_name);

	zval const_BLOCK_CODE_GEORGIAN_SUPPLEMENT_value;
	ZVAL_LONG(&const_BLOCK_CODE_GEORGIAN_SUPPLEMENT_value, UBLOCK_GEORGIAN_SUPPLEMENT);
	zend_string *const_BLOCK_CODE_GEORGIAN_SUPPLEMENT_name = zend_string_init_interned("BLOCK_CODE_GEORGIAN_SUPPLEMENT", sizeof("BLOCK_CODE_GEORGIAN_SUPPLEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_GEORGIAN_SUPPLEMENT_name, &const_BLOCK_CODE_GEORGIAN_SUPPLEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_GEORGIAN_SUPPLEMENT_name);

	zval const_BLOCK_CODE_GLAGOLITIC_value;
	ZVAL_LONG(&const_BLOCK_CODE_GLAGOLITIC_value, UBLOCK_GLAGOLITIC);
	zend_string *const_BLOCK_CODE_GLAGOLITIC_name = zend_string_init_interned("BLOCK_CODE_GLAGOLITIC", sizeof("BLOCK_CODE_GLAGOLITIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_GLAGOLITIC_name, &const_BLOCK_CODE_GLAGOLITIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_GLAGOLITIC_name);

	zval const_BLOCK_CODE_KHAROSHTHI_value;
	ZVAL_LONG(&const_BLOCK_CODE_KHAROSHTHI_value, UBLOCK_KHAROSHTHI);
	zend_string *const_BLOCK_CODE_KHAROSHTHI_name = zend_string_init_interned("BLOCK_CODE_KHAROSHTHI", sizeof("BLOCK_CODE_KHAROSHTHI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_KHAROSHTHI_name, &const_BLOCK_CODE_KHAROSHTHI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_KHAROSHTHI_name);

	zval const_BLOCK_CODE_MODIFIER_TONE_LETTERS_value;
	ZVAL_LONG(&const_BLOCK_CODE_MODIFIER_TONE_LETTERS_value, UBLOCK_MODIFIER_TONE_LETTERS);
	zend_string *const_BLOCK_CODE_MODIFIER_TONE_LETTERS_name = zend_string_init_interned("BLOCK_CODE_MODIFIER_TONE_LETTERS", sizeof("BLOCK_CODE_MODIFIER_TONE_LETTERS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MODIFIER_TONE_LETTERS_name, &const_BLOCK_CODE_MODIFIER_TONE_LETTERS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MODIFIER_TONE_LETTERS_name);

	zval const_BLOCK_CODE_NEW_TAI_LUE_value;
	ZVAL_LONG(&const_BLOCK_CODE_NEW_TAI_LUE_value, UBLOCK_NEW_TAI_LUE);
	zend_string *const_BLOCK_CODE_NEW_TAI_LUE_name = zend_string_init_interned("BLOCK_CODE_NEW_TAI_LUE", sizeof("BLOCK_CODE_NEW_TAI_LUE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_NEW_TAI_LUE_name, &const_BLOCK_CODE_NEW_TAI_LUE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_NEW_TAI_LUE_name);

	zval const_BLOCK_CODE_OLD_PERSIAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_OLD_PERSIAN_value, UBLOCK_OLD_PERSIAN);
	zend_string *const_BLOCK_CODE_OLD_PERSIAN_name = zend_string_init_interned("BLOCK_CODE_OLD_PERSIAN", sizeof("BLOCK_CODE_OLD_PERSIAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_OLD_PERSIAN_name, &const_BLOCK_CODE_OLD_PERSIAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_OLD_PERSIAN_name);

	zval const_BLOCK_CODE_PHONETIC_EXTENSIONS_SUPPLEMENT_value;
	ZVAL_LONG(&const_BLOCK_CODE_PHONETIC_EXTENSIONS_SUPPLEMENT_value, UBLOCK_PHONETIC_EXTENSIONS_SUPPLEMENT);
	zend_string *const_BLOCK_CODE_PHONETIC_EXTENSIONS_SUPPLEMENT_name = zend_string_init_interned("BLOCK_CODE_PHONETIC_EXTENSIONS_SUPPLEMENT", sizeof("BLOCK_CODE_PHONETIC_EXTENSIONS_SUPPLEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_PHONETIC_EXTENSIONS_SUPPLEMENT_name, &const_BLOCK_CODE_PHONETIC_EXTENSIONS_SUPPLEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_PHONETIC_EXTENSIONS_SUPPLEMENT_name);

	zval const_BLOCK_CODE_SUPPLEMENTAL_PUNCTUATION_value;
	ZVAL_LONG(&const_BLOCK_CODE_SUPPLEMENTAL_PUNCTUATION_value, UBLOCK_SUPPLEMENTAL_PUNCTUATION);
	zend_string *const_BLOCK_CODE_SUPPLEMENTAL_PUNCTUATION_name = zend_string_init_interned("BLOCK_CODE_SUPPLEMENTAL_PUNCTUATION", sizeof("BLOCK_CODE_SUPPLEMENTAL_PUNCTUATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SUPPLEMENTAL_PUNCTUATION_name, &const_BLOCK_CODE_SUPPLEMENTAL_PUNCTUATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SUPPLEMENTAL_PUNCTUATION_name);

	zval const_BLOCK_CODE_SYLOTI_NAGRI_value;
	ZVAL_LONG(&const_BLOCK_CODE_SYLOTI_NAGRI_value, UBLOCK_SYLOTI_NAGRI);
	zend_string *const_BLOCK_CODE_SYLOTI_NAGRI_name = zend_string_init_interned("BLOCK_CODE_SYLOTI_NAGRI", sizeof("BLOCK_CODE_SYLOTI_NAGRI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SYLOTI_NAGRI_name, &const_BLOCK_CODE_SYLOTI_NAGRI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SYLOTI_NAGRI_name);

	zval const_BLOCK_CODE_TIFINAGH_value;
	ZVAL_LONG(&const_BLOCK_CODE_TIFINAGH_value, UBLOCK_TIFINAGH);
	zend_string *const_BLOCK_CODE_TIFINAGH_name = zend_string_init_interned("BLOCK_CODE_TIFINAGH", sizeof("BLOCK_CODE_TIFINAGH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_TIFINAGH_name, &const_BLOCK_CODE_TIFINAGH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_TIFINAGH_name);

	zval const_BLOCK_CODE_VERTICAL_FORMS_value;
	ZVAL_LONG(&const_BLOCK_CODE_VERTICAL_FORMS_value, UBLOCK_VERTICAL_FORMS);
	zend_string *const_BLOCK_CODE_VERTICAL_FORMS_name = zend_string_init_interned("BLOCK_CODE_VERTICAL_FORMS", sizeof("BLOCK_CODE_VERTICAL_FORMS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_VERTICAL_FORMS_name, &const_BLOCK_CODE_VERTICAL_FORMS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_VERTICAL_FORMS_name);

	zval const_BLOCK_CODE_NKO_value;
	ZVAL_LONG(&const_BLOCK_CODE_NKO_value, UBLOCK_NKO);
	zend_string *const_BLOCK_CODE_NKO_name = zend_string_init_interned("BLOCK_CODE_NKO", sizeof("BLOCK_CODE_NKO") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_NKO_name, &const_BLOCK_CODE_NKO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_NKO_name);

	zval const_BLOCK_CODE_BALINESE_value;
	ZVAL_LONG(&const_BLOCK_CODE_BALINESE_value, UBLOCK_BALINESE);
	zend_string *const_BLOCK_CODE_BALINESE_name = zend_string_init_interned("BLOCK_CODE_BALINESE", sizeof("BLOCK_CODE_BALINESE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_BALINESE_name, &const_BLOCK_CODE_BALINESE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_BALINESE_name);

	zval const_BLOCK_CODE_LATIN_EXTENDED_C_value;
	ZVAL_LONG(&const_BLOCK_CODE_LATIN_EXTENDED_C_value, UBLOCK_LATIN_EXTENDED_C);
	zend_string *const_BLOCK_CODE_LATIN_EXTENDED_C_name = zend_string_init_interned("BLOCK_CODE_LATIN_EXTENDED_C", sizeof("BLOCK_CODE_LATIN_EXTENDED_C") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LATIN_EXTENDED_C_name, &const_BLOCK_CODE_LATIN_EXTENDED_C_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LATIN_EXTENDED_C_name);

	zval const_BLOCK_CODE_LATIN_EXTENDED_D_value;
	ZVAL_LONG(&const_BLOCK_CODE_LATIN_EXTENDED_D_value, UBLOCK_LATIN_EXTENDED_D);
	zend_string *const_BLOCK_CODE_LATIN_EXTENDED_D_name = zend_string_init_interned("BLOCK_CODE_LATIN_EXTENDED_D", sizeof("BLOCK_CODE_LATIN_EXTENDED_D") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LATIN_EXTENDED_D_name, &const_BLOCK_CODE_LATIN_EXTENDED_D_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LATIN_EXTENDED_D_name);

	zval const_BLOCK_CODE_PHAGS_PA_value;
	ZVAL_LONG(&const_BLOCK_CODE_PHAGS_PA_value, UBLOCK_PHAGS_PA);
	zend_string *const_BLOCK_CODE_PHAGS_PA_name = zend_string_init_interned("BLOCK_CODE_PHAGS_PA", sizeof("BLOCK_CODE_PHAGS_PA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_PHAGS_PA_name, &const_BLOCK_CODE_PHAGS_PA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_PHAGS_PA_name);

	zval const_BLOCK_CODE_PHOENICIAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_PHOENICIAN_value, UBLOCK_PHOENICIAN);
	zend_string *const_BLOCK_CODE_PHOENICIAN_name = zend_string_init_interned("BLOCK_CODE_PHOENICIAN", sizeof("BLOCK_CODE_PHOENICIAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_PHOENICIAN_name, &const_BLOCK_CODE_PHOENICIAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_PHOENICIAN_name);

	zval const_BLOCK_CODE_CUNEIFORM_value;
	ZVAL_LONG(&const_BLOCK_CODE_CUNEIFORM_value, UBLOCK_CUNEIFORM);
	zend_string *const_BLOCK_CODE_CUNEIFORM_name = zend_string_init_interned("BLOCK_CODE_CUNEIFORM", sizeof("BLOCK_CODE_CUNEIFORM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CUNEIFORM_name, &const_BLOCK_CODE_CUNEIFORM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CUNEIFORM_name);

	zval const_BLOCK_CODE_CUNEIFORM_NUMBERS_AND_PUNCTUATION_value;
	ZVAL_LONG(&const_BLOCK_CODE_CUNEIFORM_NUMBERS_AND_PUNCTUATION_value, UBLOCK_CUNEIFORM_NUMBERS_AND_PUNCTUATION);
	zend_string *const_BLOCK_CODE_CUNEIFORM_NUMBERS_AND_PUNCTUATION_name = zend_string_init_interned("BLOCK_CODE_CUNEIFORM_NUMBERS_AND_PUNCTUATION", sizeof("BLOCK_CODE_CUNEIFORM_NUMBERS_AND_PUNCTUATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CUNEIFORM_NUMBERS_AND_PUNCTUATION_name, &const_BLOCK_CODE_CUNEIFORM_NUMBERS_AND_PUNCTUATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CUNEIFORM_NUMBERS_AND_PUNCTUATION_name);

	zval const_BLOCK_CODE_COUNTING_ROD_NUMERALS_value;
	ZVAL_LONG(&const_BLOCK_CODE_COUNTING_ROD_NUMERALS_value, UBLOCK_COUNTING_ROD_NUMERALS);
	zend_string *const_BLOCK_CODE_COUNTING_ROD_NUMERALS_name = zend_string_init_interned("BLOCK_CODE_COUNTING_ROD_NUMERALS", sizeof("BLOCK_CODE_COUNTING_ROD_NUMERALS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_COUNTING_ROD_NUMERALS_name, &const_BLOCK_CODE_COUNTING_ROD_NUMERALS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_COUNTING_ROD_NUMERALS_name);

	zval const_BLOCK_CODE_SUNDANESE_value;
	ZVAL_LONG(&const_BLOCK_CODE_SUNDANESE_value, UBLOCK_SUNDANESE);
	zend_string *const_BLOCK_CODE_SUNDANESE_name = zend_string_init_interned("BLOCK_CODE_SUNDANESE", sizeof("BLOCK_CODE_SUNDANESE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SUNDANESE_name, &const_BLOCK_CODE_SUNDANESE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SUNDANESE_name);

	zval const_BLOCK_CODE_LEPCHA_value;
	ZVAL_LONG(&const_BLOCK_CODE_LEPCHA_value, UBLOCK_LEPCHA);
	zend_string *const_BLOCK_CODE_LEPCHA_name = zend_string_init_interned("BLOCK_CODE_LEPCHA", sizeof("BLOCK_CODE_LEPCHA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LEPCHA_name, &const_BLOCK_CODE_LEPCHA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LEPCHA_name);

	zval const_BLOCK_CODE_OL_CHIKI_value;
	ZVAL_LONG(&const_BLOCK_CODE_OL_CHIKI_value, UBLOCK_OL_CHIKI);
	zend_string *const_BLOCK_CODE_OL_CHIKI_name = zend_string_init_interned("BLOCK_CODE_OL_CHIKI", sizeof("BLOCK_CODE_OL_CHIKI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_OL_CHIKI_name, &const_BLOCK_CODE_OL_CHIKI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_OL_CHIKI_name);

	zval const_BLOCK_CODE_CYRILLIC_EXTENDED_A_value;
	ZVAL_LONG(&const_BLOCK_CODE_CYRILLIC_EXTENDED_A_value, UBLOCK_CYRILLIC_EXTENDED_A);
	zend_string *const_BLOCK_CODE_CYRILLIC_EXTENDED_A_name = zend_string_init_interned("BLOCK_CODE_CYRILLIC_EXTENDED_A", sizeof("BLOCK_CODE_CYRILLIC_EXTENDED_A") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CYRILLIC_EXTENDED_A_name, &const_BLOCK_CODE_CYRILLIC_EXTENDED_A_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CYRILLIC_EXTENDED_A_name);

	zval const_BLOCK_CODE_VAI_value;
	ZVAL_LONG(&const_BLOCK_CODE_VAI_value, UBLOCK_VAI);
	zend_string *const_BLOCK_CODE_VAI_name = zend_string_init_interned("BLOCK_CODE_VAI", sizeof("BLOCK_CODE_VAI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_VAI_name, &const_BLOCK_CODE_VAI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_VAI_name);

	zval const_BLOCK_CODE_CYRILLIC_EXTENDED_B_value;
	ZVAL_LONG(&const_BLOCK_CODE_CYRILLIC_EXTENDED_B_value, UBLOCK_CYRILLIC_EXTENDED_B);
	zend_string *const_BLOCK_CODE_CYRILLIC_EXTENDED_B_name = zend_string_init_interned("BLOCK_CODE_CYRILLIC_EXTENDED_B", sizeof("BLOCK_CODE_CYRILLIC_EXTENDED_B") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CYRILLIC_EXTENDED_B_name, &const_BLOCK_CODE_CYRILLIC_EXTENDED_B_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CYRILLIC_EXTENDED_B_name);

	zval const_BLOCK_CODE_SAURASHTRA_value;
	ZVAL_LONG(&const_BLOCK_CODE_SAURASHTRA_value, UBLOCK_SAURASHTRA);
	zend_string *const_BLOCK_CODE_SAURASHTRA_name = zend_string_init_interned("BLOCK_CODE_SAURASHTRA", sizeof("BLOCK_CODE_SAURASHTRA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SAURASHTRA_name, &const_BLOCK_CODE_SAURASHTRA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SAURASHTRA_name);

	zval const_BLOCK_CODE_KAYAH_LI_value;
	ZVAL_LONG(&const_BLOCK_CODE_KAYAH_LI_value, UBLOCK_KAYAH_LI);
	zend_string *const_BLOCK_CODE_KAYAH_LI_name = zend_string_init_interned("BLOCK_CODE_KAYAH_LI", sizeof("BLOCK_CODE_KAYAH_LI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_KAYAH_LI_name, &const_BLOCK_CODE_KAYAH_LI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_KAYAH_LI_name);

	zval const_BLOCK_CODE_REJANG_value;
	ZVAL_LONG(&const_BLOCK_CODE_REJANG_value, UBLOCK_REJANG);
	zend_string *const_BLOCK_CODE_REJANG_name = zend_string_init_interned("BLOCK_CODE_REJANG", sizeof("BLOCK_CODE_REJANG") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_REJANG_name, &const_BLOCK_CODE_REJANG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_REJANG_name);

	zval const_BLOCK_CODE_CHAM_value;
	ZVAL_LONG(&const_BLOCK_CODE_CHAM_value, UBLOCK_CHAM);
	zend_string *const_BLOCK_CODE_CHAM_name = zend_string_init_interned("BLOCK_CODE_CHAM", sizeof("BLOCK_CODE_CHAM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CHAM_name, &const_BLOCK_CODE_CHAM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CHAM_name);

	zval const_BLOCK_CODE_ANCIENT_SYMBOLS_value;
	ZVAL_LONG(&const_BLOCK_CODE_ANCIENT_SYMBOLS_value, UBLOCK_ANCIENT_SYMBOLS);
	zend_string *const_BLOCK_CODE_ANCIENT_SYMBOLS_name = zend_string_init_interned("BLOCK_CODE_ANCIENT_SYMBOLS", sizeof("BLOCK_CODE_ANCIENT_SYMBOLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ANCIENT_SYMBOLS_name, &const_BLOCK_CODE_ANCIENT_SYMBOLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ANCIENT_SYMBOLS_name);

	zval const_BLOCK_CODE_PHAISTOS_DISC_value;
	ZVAL_LONG(&const_BLOCK_CODE_PHAISTOS_DISC_value, UBLOCK_PHAISTOS_DISC);
	zend_string *const_BLOCK_CODE_PHAISTOS_DISC_name = zend_string_init_interned("BLOCK_CODE_PHAISTOS_DISC", sizeof("BLOCK_CODE_PHAISTOS_DISC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_PHAISTOS_DISC_name, &const_BLOCK_CODE_PHAISTOS_DISC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_PHAISTOS_DISC_name);

	zval const_BLOCK_CODE_LYCIAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_LYCIAN_value, UBLOCK_LYCIAN);
	zend_string *const_BLOCK_CODE_LYCIAN_name = zend_string_init_interned("BLOCK_CODE_LYCIAN", sizeof("BLOCK_CODE_LYCIAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LYCIAN_name, &const_BLOCK_CODE_LYCIAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LYCIAN_name);

	zval const_BLOCK_CODE_CARIAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_CARIAN_value, UBLOCK_CARIAN);
	zend_string *const_BLOCK_CODE_CARIAN_name = zend_string_init_interned("BLOCK_CODE_CARIAN", sizeof("BLOCK_CODE_CARIAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CARIAN_name, &const_BLOCK_CODE_CARIAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CARIAN_name);

	zval const_BLOCK_CODE_LYDIAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_LYDIAN_value, UBLOCK_LYDIAN);
	zend_string *const_BLOCK_CODE_LYDIAN_name = zend_string_init_interned("BLOCK_CODE_LYDIAN", sizeof("BLOCK_CODE_LYDIAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LYDIAN_name, &const_BLOCK_CODE_LYDIAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LYDIAN_name);

	zval const_BLOCK_CODE_MAHJONG_TILES_value;
	ZVAL_LONG(&const_BLOCK_CODE_MAHJONG_TILES_value, UBLOCK_MAHJONG_TILES);
	zend_string *const_BLOCK_CODE_MAHJONG_TILES_name = zend_string_init_interned("BLOCK_CODE_MAHJONG_TILES", sizeof("BLOCK_CODE_MAHJONG_TILES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MAHJONG_TILES_name, &const_BLOCK_CODE_MAHJONG_TILES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MAHJONG_TILES_name);

	zval const_BLOCK_CODE_DOMINO_TILES_value;
	ZVAL_LONG(&const_BLOCK_CODE_DOMINO_TILES_value, UBLOCK_DOMINO_TILES);
	zend_string *const_BLOCK_CODE_DOMINO_TILES_name = zend_string_init_interned("BLOCK_CODE_DOMINO_TILES", sizeof("BLOCK_CODE_DOMINO_TILES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_DOMINO_TILES_name, &const_BLOCK_CODE_DOMINO_TILES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_DOMINO_TILES_name);

	zval const_BLOCK_CODE_SAMARITAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_SAMARITAN_value, UBLOCK_SAMARITAN);
	zend_string *const_BLOCK_CODE_SAMARITAN_name = zend_string_init_interned("BLOCK_CODE_SAMARITAN", sizeof("BLOCK_CODE_SAMARITAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SAMARITAN_name, &const_BLOCK_CODE_SAMARITAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SAMARITAN_name);

	zval const_BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED_value;
	ZVAL_LONG(&const_BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED_value, UBLOCK_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED);
	zend_string *const_BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED_name = zend_string_init_interned("BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED", sizeof("BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED_name, &const_BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED_name);

	zval const_BLOCK_CODE_TAI_THAM_value;
	ZVAL_LONG(&const_BLOCK_CODE_TAI_THAM_value, UBLOCK_TAI_THAM);
	zend_string *const_BLOCK_CODE_TAI_THAM_name = zend_string_init_interned("BLOCK_CODE_TAI_THAM", sizeof("BLOCK_CODE_TAI_THAM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_TAI_THAM_name, &const_BLOCK_CODE_TAI_THAM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_TAI_THAM_name);

	zval const_BLOCK_CODE_VEDIC_EXTENSIONS_value;
	ZVAL_LONG(&const_BLOCK_CODE_VEDIC_EXTENSIONS_value, UBLOCK_VEDIC_EXTENSIONS);
	zend_string *const_BLOCK_CODE_VEDIC_EXTENSIONS_name = zend_string_init_interned("BLOCK_CODE_VEDIC_EXTENSIONS", sizeof("BLOCK_CODE_VEDIC_EXTENSIONS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_VEDIC_EXTENSIONS_name, &const_BLOCK_CODE_VEDIC_EXTENSIONS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_VEDIC_EXTENSIONS_name);

	zval const_BLOCK_CODE_LISU_value;
	ZVAL_LONG(&const_BLOCK_CODE_LISU_value, UBLOCK_LISU);
	zend_string *const_BLOCK_CODE_LISU_name = zend_string_init_interned("BLOCK_CODE_LISU", sizeof("BLOCK_CODE_LISU") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LISU_name, &const_BLOCK_CODE_LISU_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LISU_name);

	zval const_BLOCK_CODE_BAMUM_value;
	ZVAL_LONG(&const_BLOCK_CODE_BAMUM_value, UBLOCK_BAMUM);
	zend_string *const_BLOCK_CODE_BAMUM_name = zend_string_init_interned("BLOCK_CODE_BAMUM", sizeof("BLOCK_CODE_BAMUM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_BAMUM_name, &const_BLOCK_CODE_BAMUM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_BAMUM_name);

	zval const_BLOCK_CODE_COMMON_INDIC_NUMBER_FORMS_value;
	ZVAL_LONG(&const_BLOCK_CODE_COMMON_INDIC_NUMBER_FORMS_value, UBLOCK_COMMON_INDIC_NUMBER_FORMS);
	zend_string *const_BLOCK_CODE_COMMON_INDIC_NUMBER_FORMS_name = zend_string_init_interned("BLOCK_CODE_COMMON_INDIC_NUMBER_FORMS", sizeof("BLOCK_CODE_COMMON_INDIC_NUMBER_FORMS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_COMMON_INDIC_NUMBER_FORMS_name, &const_BLOCK_CODE_COMMON_INDIC_NUMBER_FORMS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_COMMON_INDIC_NUMBER_FORMS_name);

	zval const_BLOCK_CODE_DEVANAGARI_EXTENDED_value;
	ZVAL_LONG(&const_BLOCK_CODE_DEVANAGARI_EXTENDED_value, UBLOCK_DEVANAGARI_EXTENDED);
	zend_string *const_BLOCK_CODE_DEVANAGARI_EXTENDED_name = zend_string_init_interned("BLOCK_CODE_DEVANAGARI_EXTENDED", sizeof("BLOCK_CODE_DEVANAGARI_EXTENDED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_DEVANAGARI_EXTENDED_name, &const_BLOCK_CODE_DEVANAGARI_EXTENDED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_DEVANAGARI_EXTENDED_name);

	zval const_BLOCK_CODE_HANGUL_JAMO_EXTENDED_A_value;
	ZVAL_LONG(&const_BLOCK_CODE_HANGUL_JAMO_EXTENDED_A_value, UBLOCK_HANGUL_JAMO_EXTENDED_A);
	zend_string *const_BLOCK_CODE_HANGUL_JAMO_EXTENDED_A_name = zend_string_init_interned("BLOCK_CODE_HANGUL_JAMO_EXTENDED_A", sizeof("BLOCK_CODE_HANGUL_JAMO_EXTENDED_A") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_HANGUL_JAMO_EXTENDED_A_name, &const_BLOCK_CODE_HANGUL_JAMO_EXTENDED_A_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_HANGUL_JAMO_EXTENDED_A_name);

	zval const_BLOCK_CODE_JAVANESE_value;
	ZVAL_LONG(&const_BLOCK_CODE_JAVANESE_value, UBLOCK_JAVANESE);
	zend_string *const_BLOCK_CODE_JAVANESE_name = zend_string_init_interned("BLOCK_CODE_JAVANESE", sizeof("BLOCK_CODE_JAVANESE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_JAVANESE_name, &const_BLOCK_CODE_JAVANESE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_JAVANESE_name);

	zval const_BLOCK_CODE_MYANMAR_EXTENDED_A_value;
	ZVAL_LONG(&const_BLOCK_CODE_MYANMAR_EXTENDED_A_value, UBLOCK_MYANMAR_EXTENDED_A);
	zend_string *const_BLOCK_CODE_MYANMAR_EXTENDED_A_name = zend_string_init_interned("BLOCK_CODE_MYANMAR_EXTENDED_A", sizeof("BLOCK_CODE_MYANMAR_EXTENDED_A") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MYANMAR_EXTENDED_A_name, &const_BLOCK_CODE_MYANMAR_EXTENDED_A_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MYANMAR_EXTENDED_A_name);

	zval const_BLOCK_CODE_TAI_VIET_value;
	ZVAL_LONG(&const_BLOCK_CODE_TAI_VIET_value, UBLOCK_TAI_VIET);
	zend_string *const_BLOCK_CODE_TAI_VIET_name = zend_string_init_interned("BLOCK_CODE_TAI_VIET", sizeof("BLOCK_CODE_TAI_VIET") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_TAI_VIET_name, &const_BLOCK_CODE_TAI_VIET_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_TAI_VIET_name);

	zval const_BLOCK_CODE_MEETEI_MAYEK_value;
	ZVAL_LONG(&const_BLOCK_CODE_MEETEI_MAYEK_value, UBLOCK_MEETEI_MAYEK);
	zend_string *const_BLOCK_CODE_MEETEI_MAYEK_name = zend_string_init_interned("BLOCK_CODE_MEETEI_MAYEK", sizeof("BLOCK_CODE_MEETEI_MAYEK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MEETEI_MAYEK_name, &const_BLOCK_CODE_MEETEI_MAYEK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MEETEI_MAYEK_name);

	zval const_BLOCK_CODE_HANGUL_JAMO_EXTENDED_B_value;
	ZVAL_LONG(&const_BLOCK_CODE_HANGUL_JAMO_EXTENDED_B_value, UBLOCK_HANGUL_JAMO_EXTENDED_B);
	zend_string *const_BLOCK_CODE_HANGUL_JAMO_EXTENDED_B_name = zend_string_init_interned("BLOCK_CODE_HANGUL_JAMO_EXTENDED_B", sizeof("BLOCK_CODE_HANGUL_JAMO_EXTENDED_B") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_HANGUL_JAMO_EXTENDED_B_name, &const_BLOCK_CODE_HANGUL_JAMO_EXTENDED_B_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_HANGUL_JAMO_EXTENDED_B_name);

	zval const_BLOCK_CODE_IMPERIAL_ARAMAIC_value;
	ZVAL_LONG(&const_BLOCK_CODE_IMPERIAL_ARAMAIC_value, UBLOCK_IMPERIAL_ARAMAIC);
	zend_string *const_BLOCK_CODE_IMPERIAL_ARAMAIC_name = zend_string_init_interned("BLOCK_CODE_IMPERIAL_ARAMAIC", sizeof("BLOCK_CODE_IMPERIAL_ARAMAIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_IMPERIAL_ARAMAIC_name, &const_BLOCK_CODE_IMPERIAL_ARAMAIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_IMPERIAL_ARAMAIC_name);

	zval const_BLOCK_CODE_OLD_SOUTH_ARABIAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_OLD_SOUTH_ARABIAN_value, UBLOCK_OLD_SOUTH_ARABIAN);
	zend_string *const_BLOCK_CODE_OLD_SOUTH_ARABIAN_name = zend_string_init_interned("BLOCK_CODE_OLD_SOUTH_ARABIAN", sizeof("BLOCK_CODE_OLD_SOUTH_ARABIAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_OLD_SOUTH_ARABIAN_name, &const_BLOCK_CODE_OLD_SOUTH_ARABIAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_OLD_SOUTH_ARABIAN_name);

	zval const_BLOCK_CODE_AVESTAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_AVESTAN_value, UBLOCK_AVESTAN);
	zend_string *const_BLOCK_CODE_AVESTAN_name = zend_string_init_interned("BLOCK_CODE_AVESTAN", sizeof("BLOCK_CODE_AVESTAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_AVESTAN_name, &const_BLOCK_CODE_AVESTAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_AVESTAN_name);

	zval const_BLOCK_CODE_INSCRIPTIONAL_PARTHIAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_INSCRIPTIONAL_PARTHIAN_value, UBLOCK_INSCRIPTIONAL_PARTHIAN);
	zend_string *const_BLOCK_CODE_INSCRIPTIONAL_PARTHIAN_name = zend_string_init_interned("BLOCK_CODE_INSCRIPTIONAL_PARTHIAN", sizeof("BLOCK_CODE_INSCRIPTIONAL_PARTHIAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_INSCRIPTIONAL_PARTHIAN_name, &const_BLOCK_CODE_INSCRIPTIONAL_PARTHIAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_INSCRIPTIONAL_PARTHIAN_name);

	zval const_BLOCK_CODE_INSCRIPTIONAL_PAHLAVI_value;
	ZVAL_LONG(&const_BLOCK_CODE_INSCRIPTIONAL_PAHLAVI_value, UBLOCK_INSCRIPTIONAL_PAHLAVI);
	zend_string *const_BLOCK_CODE_INSCRIPTIONAL_PAHLAVI_name = zend_string_init_interned("BLOCK_CODE_INSCRIPTIONAL_PAHLAVI", sizeof("BLOCK_CODE_INSCRIPTIONAL_PAHLAVI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_INSCRIPTIONAL_PAHLAVI_name, &const_BLOCK_CODE_INSCRIPTIONAL_PAHLAVI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_INSCRIPTIONAL_PAHLAVI_name);

	zval const_BLOCK_CODE_OLD_TURKIC_value;
	ZVAL_LONG(&const_BLOCK_CODE_OLD_TURKIC_value, UBLOCK_OLD_TURKIC);
	zend_string *const_BLOCK_CODE_OLD_TURKIC_name = zend_string_init_interned("BLOCK_CODE_OLD_TURKIC", sizeof("BLOCK_CODE_OLD_TURKIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_OLD_TURKIC_name, &const_BLOCK_CODE_OLD_TURKIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_OLD_TURKIC_name);

	zval const_BLOCK_CODE_RUMI_NUMERAL_SYMBOLS_value;
	ZVAL_LONG(&const_BLOCK_CODE_RUMI_NUMERAL_SYMBOLS_value, UBLOCK_RUMI_NUMERAL_SYMBOLS);
	zend_string *const_BLOCK_CODE_RUMI_NUMERAL_SYMBOLS_name = zend_string_init_interned("BLOCK_CODE_RUMI_NUMERAL_SYMBOLS", sizeof("BLOCK_CODE_RUMI_NUMERAL_SYMBOLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_RUMI_NUMERAL_SYMBOLS_name, &const_BLOCK_CODE_RUMI_NUMERAL_SYMBOLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_RUMI_NUMERAL_SYMBOLS_name);

	zval const_BLOCK_CODE_KAITHI_value;
	ZVAL_LONG(&const_BLOCK_CODE_KAITHI_value, UBLOCK_KAITHI);
	zend_string *const_BLOCK_CODE_KAITHI_name = zend_string_init_interned("BLOCK_CODE_KAITHI", sizeof("BLOCK_CODE_KAITHI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_KAITHI_name, &const_BLOCK_CODE_KAITHI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_KAITHI_name);

	zval const_BLOCK_CODE_EGYPTIAN_HIEROGLYPHS_value;
	ZVAL_LONG(&const_BLOCK_CODE_EGYPTIAN_HIEROGLYPHS_value, UBLOCK_EGYPTIAN_HIEROGLYPHS);
	zend_string *const_BLOCK_CODE_EGYPTIAN_HIEROGLYPHS_name = zend_string_init_interned("BLOCK_CODE_EGYPTIAN_HIEROGLYPHS", sizeof("BLOCK_CODE_EGYPTIAN_HIEROGLYPHS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_EGYPTIAN_HIEROGLYPHS_name, &const_BLOCK_CODE_EGYPTIAN_HIEROGLYPHS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_EGYPTIAN_HIEROGLYPHS_name);

	zval const_BLOCK_CODE_ENCLOSED_ALPHANUMERIC_SUPPLEMENT_value;
	ZVAL_LONG(&const_BLOCK_CODE_ENCLOSED_ALPHANUMERIC_SUPPLEMENT_value, UBLOCK_ENCLOSED_ALPHANUMERIC_SUPPLEMENT);
	zend_string *const_BLOCK_CODE_ENCLOSED_ALPHANUMERIC_SUPPLEMENT_name = zend_string_init_interned("BLOCK_CODE_ENCLOSED_ALPHANUMERIC_SUPPLEMENT", sizeof("BLOCK_CODE_ENCLOSED_ALPHANUMERIC_SUPPLEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ENCLOSED_ALPHANUMERIC_SUPPLEMENT_name, &const_BLOCK_CODE_ENCLOSED_ALPHANUMERIC_SUPPLEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ENCLOSED_ALPHANUMERIC_SUPPLEMENT_name);

	zval const_BLOCK_CODE_ENCLOSED_IDEOGRAPHIC_SUPPLEMENT_value;
	ZVAL_LONG(&const_BLOCK_CODE_ENCLOSED_IDEOGRAPHIC_SUPPLEMENT_value, UBLOCK_ENCLOSED_IDEOGRAPHIC_SUPPLEMENT);
	zend_string *const_BLOCK_CODE_ENCLOSED_IDEOGRAPHIC_SUPPLEMENT_name = zend_string_init_interned("BLOCK_CODE_ENCLOSED_IDEOGRAPHIC_SUPPLEMENT", sizeof("BLOCK_CODE_ENCLOSED_IDEOGRAPHIC_SUPPLEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ENCLOSED_IDEOGRAPHIC_SUPPLEMENT_name, &const_BLOCK_CODE_ENCLOSED_IDEOGRAPHIC_SUPPLEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ENCLOSED_IDEOGRAPHIC_SUPPLEMENT_name);

	zval const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C_value;
	ZVAL_LONG(&const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C_value, UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C);
	zend_string *const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C_name = zend_string_init_interned("BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C", sizeof("BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C_name, &const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C_name);

	zval const_BLOCK_CODE_MANDAIC_value;
	ZVAL_LONG(&const_BLOCK_CODE_MANDAIC_value, UBLOCK_MANDAIC);
	zend_string *const_BLOCK_CODE_MANDAIC_name = zend_string_init_interned("BLOCK_CODE_MANDAIC", sizeof("BLOCK_CODE_MANDAIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MANDAIC_name, &const_BLOCK_CODE_MANDAIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MANDAIC_name);

	zval const_BLOCK_CODE_BATAK_value;
	ZVAL_LONG(&const_BLOCK_CODE_BATAK_value, UBLOCK_BATAK);
	zend_string *const_BLOCK_CODE_BATAK_name = zend_string_init_interned("BLOCK_CODE_BATAK", sizeof("BLOCK_CODE_BATAK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_BATAK_name, &const_BLOCK_CODE_BATAK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_BATAK_name);

	zval const_BLOCK_CODE_ETHIOPIC_EXTENDED_A_value;
	ZVAL_LONG(&const_BLOCK_CODE_ETHIOPIC_EXTENDED_A_value, UBLOCK_ETHIOPIC_EXTENDED_A);
	zend_string *const_BLOCK_CODE_ETHIOPIC_EXTENDED_A_name = zend_string_init_interned("BLOCK_CODE_ETHIOPIC_EXTENDED_A", sizeof("BLOCK_CODE_ETHIOPIC_EXTENDED_A") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ETHIOPIC_EXTENDED_A_name, &const_BLOCK_CODE_ETHIOPIC_EXTENDED_A_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ETHIOPIC_EXTENDED_A_name);

	zval const_BLOCK_CODE_BRAHMI_value;
	ZVAL_LONG(&const_BLOCK_CODE_BRAHMI_value, UBLOCK_BRAHMI);
	zend_string *const_BLOCK_CODE_BRAHMI_name = zend_string_init_interned("BLOCK_CODE_BRAHMI", sizeof("BLOCK_CODE_BRAHMI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_BRAHMI_name, &const_BLOCK_CODE_BRAHMI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_BRAHMI_name);

	zval const_BLOCK_CODE_BAMUM_SUPPLEMENT_value;
	ZVAL_LONG(&const_BLOCK_CODE_BAMUM_SUPPLEMENT_value, UBLOCK_BAMUM_SUPPLEMENT);
	zend_string *const_BLOCK_CODE_BAMUM_SUPPLEMENT_name = zend_string_init_interned("BLOCK_CODE_BAMUM_SUPPLEMENT", sizeof("BLOCK_CODE_BAMUM_SUPPLEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_BAMUM_SUPPLEMENT_name, &const_BLOCK_CODE_BAMUM_SUPPLEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_BAMUM_SUPPLEMENT_name);

	zval const_BLOCK_CODE_KANA_SUPPLEMENT_value;
	ZVAL_LONG(&const_BLOCK_CODE_KANA_SUPPLEMENT_value, UBLOCK_KANA_SUPPLEMENT);
	zend_string *const_BLOCK_CODE_KANA_SUPPLEMENT_name = zend_string_init_interned("BLOCK_CODE_KANA_SUPPLEMENT", sizeof("BLOCK_CODE_KANA_SUPPLEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_KANA_SUPPLEMENT_name, &const_BLOCK_CODE_KANA_SUPPLEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_KANA_SUPPLEMENT_name);

	zval const_BLOCK_CODE_PLAYING_CARDS_value;
	ZVAL_LONG(&const_BLOCK_CODE_PLAYING_CARDS_value, UBLOCK_PLAYING_CARDS);
	zend_string *const_BLOCK_CODE_PLAYING_CARDS_name = zend_string_init_interned("BLOCK_CODE_PLAYING_CARDS", sizeof("BLOCK_CODE_PLAYING_CARDS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_PLAYING_CARDS_name, &const_BLOCK_CODE_PLAYING_CARDS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_PLAYING_CARDS_name);

	zval const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS_value;
	ZVAL_LONG(&const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS_value, UBLOCK_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS);
	zend_string *const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS_name = zend_string_init_interned("BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS", sizeof("BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS_name, &const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS_name);

	zval const_BLOCK_CODE_EMOTICONS_value;
	ZVAL_LONG(&const_BLOCK_CODE_EMOTICONS_value, UBLOCK_EMOTICONS);
	zend_string *const_BLOCK_CODE_EMOTICONS_name = zend_string_init_interned("BLOCK_CODE_EMOTICONS", sizeof("BLOCK_CODE_EMOTICONS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_EMOTICONS_name, &const_BLOCK_CODE_EMOTICONS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_EMOTICONS_name);

	zval const_BLOCK_CODE_TRANSPORT_AND_MAP_SYMBOLS_value;
	ZVAL_LONG(&const_BLOCK_CODE_TRANSPORT_AND_MAP_SYMBOLS_value, UBLOCK_TRANSPORT_AND_MAP_SYMBOLS);
	zend_string *const_BLOCK_CODE_TRANSPORT_AND_MAP_SYMBOLS_name = zend_string_init_interned("BLOCK_CODE_TRANSPORT_AND_MAP_SYMBOLS", sizeof("BLOCK_CODE_TRANSPORT_AND_MAP_SYMBOLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_TRANSPORT_AND_MAP_SYMBOLS_name, &const_BLOCK_CODE_TRANSPORT_AND_MAP_SYMBOLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_TRANSPORT_AND_MAP_SYMBOLS_name);

	zval const_BLOCK_CODE_ALCHEMICAL_SYMBOLS_value;
	ZVAL_LONG(&const_BLOCK_CODE_ALCHEMICAL_SYMBOLS_value, UBLOCK_ALCHEMICAL_SYMBOLS);
	zend_string *const_BLOCK_CODE_ALCHEMICAL_SYMBOLS_name = zend_string_init_interned("BLOCK_CODE_ALCHEMICAL_SYMBOLS", sizeof("BLOCK_CODE_ALCHEMICAL_SYMBOLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ALCHEMICAL_SYMBOLS_name, &const_BLOCK_CODE_ALCHEMICAL_SYMBOLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ALCHEMICAL_SYMBOLS_name);

	zval const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D_value;
	ZVAL_LONG(&const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D_value, UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D);
	zend_string *const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D_name = zend_string_init_interned("BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D", sizeof("BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D_name, &const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D_name);

	zval const_BLOCK_CODE_ARABIC_EXTENDED_A_value;
	ZVAL_LONG(&const_BLOCK_CODE_ARABIC_EXTENDED_A_value, UBLOCK_ARABIC_EXTENDED_A);
	zend_string *const_BLOCK_CODE_ARABIC_EXTENDED_A_name = zend_string_init_interned("BLOCK_CODE_ARABIC_EXTENDED_A", sizeof("BLOCK_CODE_ARABIC_EXTENDED_A") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ARABIC_EXTENDED_A_name, &const_BLOCK_CODE_ARABIC_EXTENDED_A_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ARABIC_EXTENDED_A_name);

	zval const_BLOCK_CODE_ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS_value;
	ZVAL_LONG(&const_BLOCK_CODE_ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS_value, UBLOCK_ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS);
	zend_string *const_BLOCK_CODE_ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS_name = zend_string_init_interned("BLOCK_CODE_ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS", sizeof("BLOCK_CODE_ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS_name, &const_BLOCK_CODE_ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS_name);

	zval const_BLOCK_CODE_CHAKMA_value;
	ZVAL_LONG(&const_BLOCK_CODE_CHAKMA_value, UBLOCK_CHAKMA);
	zend_string *const_BLOCK_CODE_CHAKMA_name = zend_string_init_interned("BLOCK_CODE_CHAKMA", sizeof("BLOCK_CODE_CHAKMA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CHAKMA_name, &const_BLOCK_CODE_CHAKMA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CHAKMA_name);

	zval const_BLOCK_CODE_MEETEI_MAYEK_EXTENSIONS_value;
	ZVAL_LONG(&const_BLOCK_CODE_MEETEI_MAYEK_EXTENSIONS_value, UBLOCK_MEETEI_MAYEK_EXTENSIONS);
	zend_string *const_BLOCK_CODE_MEETEI_MAYEK_EXTENSIONS_name = zend_string_init_interned("BLOCK_CODE_MEETEI_MAYEK_EXTENSIONS", sizeof("BLOCK_CODE_MEETEI_MAYEK_EXTENSIONS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MEETEI_MAYEK_EXTENSIONS_name, &const_BLOCK_CODE_MEETEI_MAYEK_EXTENSIONS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MEETEI_MAYEK_EXTENSIONS_name);

	zval const_BLOCK_CODE_MEROITIC_CURSIVE_value;
	ZVAL_LONG(&const_BLOCK_CODE_MEROITIC_CURSIVE_value, UBLOCK_MEROITIC_CURSIVE);
	zend_string *const_BLOCK_CODE_MEROITIC_CURSIVE_name = zend_string_init_interned("BLOCK_CODE_MEROITIC_CURSIVE", sizeof("BLOCK_CODE_MEROITIC_CURSIVE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MEROITIC_CURSIVE_name, &const_BLOCK_CODE_MEROITIC_CURSIVE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MEROITIC_CURSIVE_name);

	zval const_BLOCK_CODE_MEROITIC_HIEROGLYPHS_value;
	ZVAL_LONG(&const_BLOCK_CODE_MEROITIC_HIEROGLYPHS_value, UBLOCK_MEROITIC_HIEROGLYPHS);
	zend_string *const_BLOCK_CODE_MEROITIC_HIEROGLYPHS_name = zend_string_init_interned("BLOCK_CODE_MEROITIC_HIEROGLYPHS", sizeof("BLOCK_CODE_MEROITIC_HIEROGLYPHS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MEROITIC_HIEROGLYPHS_name, &const_BLOCK_CODE_MEROITIC_HIEROGLYPHS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MEROITIC_HIEROGLYPHS_name);

	zval const_BLOCK_CODE_MIAO_value;
	ZVAL_LONG(&const_BLOCK_CODE_MIAO_value, UBLOCK_MIAO);
	zend_string *const_BLOCK_CODE_MIAO_name = zend_string_init_interned("BLOCK_CODE_MIAO", sizeof("BLOCK_CODE_MIAO") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MIAO_name, &const_BLOCK_CODE_MIAO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MIAO_name);

	zval const_BLOCK_CODE_SHARADA_value;
	ZVAL_LONG(&const_BLOCK_CODE_SHARADA_value, UBLOCK_SHARADA);
	zend_string *const_BLOCK_CODE_SHARADA_name = zend_string_init_interned("BLOCK_CODE_SHARADA", sizeof("BLOCK_CODE_SHARADA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SHARADA_name, &const_BLOCK_CODE_SHARADA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SHARADA_name);

	zval const_BLOCK_CODE_SORA_SOMPENG_value;
	ZVAL_LONG(&const_BLOCK_CODE_SORA_SOMPENG_value, UBLOCK_SORA_SOMPENG);
	zend_string *const_BLOCK_CODE_SORA_SOMPENG_name = zend_string_init_interned("BLOCK_CODE_SORA_SOMPENG", sizeof("BLOCK_CODE_SORA_SOMPENG") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SORA_SOMPENG_name, &const_BLOCK_CODE_SORA_SOMPENG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SORA_SOMPENG_name);

	zval const_BLOCK_CODE_SUNDANESE_SUPPLEMENT_value;
	ZVAL_LONG(&const_BLOCK_CODE_SUNDANESE_SUPPLEMENT_value, UBLOCK_SUNDANESE_SUPPLEMENT);
	zend_string *const_BLOCK_CODE_SUNDANESE_SUPPLEMENT_name = zend_string_init_interned("BLOCK_CODE_SUNDANESE_SUPPLEMENT", sizeof("BLOCK_CODE_SUNDANESE_SUPPLEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SUNDANESE_SUPPLEMENT_name, &const_BLOCK_CODE_SUNDANESE_SUPPLEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SUNDANESE_SUPPLEMENT_name);

	zval const_BLOCK_CODE_TAKRI_value;
	ZVAL_LONG(&const_BLOCK_CODE_TAKRI_value, UBLOCK_TAKRI);
	zend_string *const_BLOCK_CODE_TAKRI_name = zend_string_init_interned("BLOCK_CODE_TAKRI", sizeof("BLOCK_CODE_TAKRI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_TAKRI_name, &const_BLOCK_CODE_TAKRI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_TAKRI_name);
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_BASSA_VAH_value;
	ZVAL_LONG(&const_BLOCK_CODE_BASSA_VAH_value, UBLOCK_BASSA_VAH);
	zend_string *const_BLOCK_CODE_BASSA_VAH_name = zend_string_init_interned("BLOCK_CODE_BASSA_VAH", sizeof("BLOCK_CODE_BASSA_VAH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_BASSA_VAH_name, &const_BLOCK_CODE_BASSA_VAH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_BASSA_VAH_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_CAUCASIAN_ALBANIAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_CAUCASIAN_ALBANIAN_value, UBLOCK_CAUCASIAN_ALBANIAN);
	zend_string *const_BLOCK_CODE_CAUCASIAN_ALBANIAN_name = zend_string_init_interned("BLOCK_CODE_CAUCASIAN_ALBANIAN", sizeof("BLOCK_CODE_CAUCASIAN_ALBANIAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_CAUCASIAN_ALBANIAN_name, &const_BLOCK_CODE_CAUCASIAN_ALBANIAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_CAUCASIAN_ALBANIAN_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_COPTIC_EPACT_NUMBERS_value;
	ZVAL_LONG(&const_BLOCK_CODE_COPTIC_EPACT_NUMBERS_value, UBLOCK_COPTIC_EPACT_NUMBERS);
	zend_string *const_BLOCK_CODE_COPTIC_EPACT_NUMBERS_name = zend_string_init_interned("BLOCK_CODE_COPTIC_EPACT_NUMBERS", sizeof("BLOCK_CODE_COPTIC_EPACT_NUMBERS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_COPTIC_EPACT_NUMBERS_name, &const_BLOCK_CODE_COPTIC_EPACT_NUMBERS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_COPTIC_EPACT_NUMBERS_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_EXTENDED_value;
	ZVAL_LONG(&const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_EXTENDED_value, UBLOCK_COMBINING_DIACRITICAL_MARKS_EXTENDED);
	zend_string *const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_EXTENDED_name = zend_string_init_interned("BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_EXTENDED", sizeof("BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_EXTENDED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_EXTENDED_name, &const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_EXTENDED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_EXTENDED_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_DUPLOYAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_DUPLOYAN_value, UBLOCK_DUPLOYAN);
	zend_string *const_BLOCK_CODE_DUPLOYAN_name = zend_string_init_interned("BLOCK_CODE_DUPLOYAN", sizeof("BLOCK_CODE_DUPLOYAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_DUPLOYAN_name, &const_BLOCK_CODE_DUPLOYAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_DUPLOYAN_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_ELBASAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_ELBASAN_value, UBLOCK_ELBASAN);
	zend_string *const_BLOCK_CODE_ELBASAN_name = zend_string_init_interned("BLOCK_CODE_ELBASAN", sizeof("BLOCK_CODE_ELBASAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ELBASAN_name, &const_BLOCK_CODE_ELBASAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ELBASAN_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_GEOMETRIC_SHAPES_EXTENDED_value;
	ZVAL_LONG(&const_BLOCK_CODE_GEOMETRIC_SHAPES_EXTENDED_value, UBLOCK_GEOMETRIC_SHAPES_EXTENDED);
	zend_string *const_BLOCK_CODE_GEOMETRIC_SHAPES_EXTENDED_name = zend_string_init_interned("BLOCK_CODE_GEOMETRIC_SHAPES_EXTENDED", sizeof("BLOCK_CODE_GEOMETRIC_SHAPES_EXTENDED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_GEOMETRIC_SHAPES_EXTENDED_name, &const_BLOCK_CODE_GEOMETRIC_SHAPES_EXTENDED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_GEOMETRIC_SHAPES_EXTENDED_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_GRANTHA_value;
	ZVAL_LONG(&const_BLOCK_CODE_GRANTHA_value, UBLOCK_GRANTHA);
	zend_string *const_BLOCK_CODE_GRANTHA_name = zend_string_init_interned("BLOCK_CODE_GRANTHA", sizeof("BLOCK_CODE_GRANTHA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_GRANTHA_name, &const_BLOCK_CODE_GRANTHA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_GRANTHA_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_KHOJKI_value;
	ZVAL_LONG(&const_BLOCK_CODE_KHOJKI_value, UBLOCK_KHOJKI);
	zend_string *const_BLOCK_CODE_KHOJKI_name = zend_string_init_interned("BLOCK_CODE_KHOJKI", sizeof("BLOCK_CODE_KHOJKI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_KHOJKI_name, &const_BLOCK_CODE_KHOJKI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_KHOJKI_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_KHUDAWADI_value;
	ZVAL_LONG(&const_BLOCK_CODE_KHUDAWADI_value, UBLOCK_KHUDAWADI);
	zend_string *const_BLOCK_CODE_KHUDAWADI_name = zend_string_init_interned("BLOCK_CODE_KHUDAWADI", sizeof("BLOCK_CODE_KHUDAWADI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_KHUDAWADI_name, &const_BLOCK_CODE_KHUDAWADI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_KHUDAWADI_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_LATIN_EXTENDED_E_value;
	ZVAL_LONG(&const_BLOCK_CODE_LATIN_EXTENDED_E_value, UBLOCK_LATIN_EXTENDED_E);
	zend_string *const_BLOCK_CODE_LATIN_EXTENDED_E_name = zend_string_init_interned("BLOCK_CODE_LATIN_EXTENDED_E", sizeof("BLOCK_CODE_LATIN_EXTENDED_E") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LATIN_EXTENDED_E_name, &const_BLOCK_CODE_LATIN_EXTENDED_E_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LATIN_EXTENDED_E_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_LINEAR_A_value;
	ZVAL_LONG(&const_BLOCK_CODE_LINEAR_A_value, UBLOCK_LINEAR_A);
	zend_string *const_BLOCK_CODE_LINEAR_A_name = zend_string_init_interned("BLOCK_CODE_LINEAR_A", sizeof("BLOCK_CODE_LINEAR_A") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_LINEAR_A_name, &const_BLOCK_CODE_LINEAR_A_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_LINEAR_A_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_MAHAJANI_value;
	ZVAL_LONG(&const_BLOCK_CODE_MAHAJANI_value, UBLOCK_MAHAJANI);
	zend_string *const_BLOCK_CODE_MAHAJANI_name = zend_string_init_interned("BLOCK_CODE_MAHAJANI", sizeof("BLOCK_CODE_MAHAJANI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MAHAJANI_name, &const_BLOCK_CODE_MAHAJANI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MAHAJANI_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_MANICHAEAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_MANICHAEAN_value, UBLOCK_MANICHAEAN);
	zend_string *const_BLOCK_CODE_MANICHAEAN_name = zend_string_init_interned("BLOCK_CODE_MANICHAEAN", sizeof("BLOCK_CODE_MANICHAEAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MANICHAEAN_name, &const_BLOCK_CODE_MANICHAEAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MANICHAEAN_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_MENDE_KIKAKUI_value;
	ZVAL_LONG(&const_BLOCK_CODE_MENDE_KIKAKUI_value, UBLOCK_MENDE_KIKAKUI);
	zend_string *const_BLOCK_CODE_MENDE_KIKAKUI_name = zend_string_init_interned("BLOCK_CODE_MENDE_KIKAKUI", sizeof("BLOCK_CODE_MENDE_KIKAKUI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MENDE_KIKAKUI_name, &const_BLOCK_CODE_MENDE_KIKAKUI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MENDE_KIKAKUI_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_MODI_value;
	ZVAL_LONG(&const_BLOCK_CODE_MODI_value, UBLOCK_MODI);
	zend_string *const_BLOCK_CODE_MODI_name = zend_string_init_interned("BLOCK_CODE_MODI", sizeof("BLOCK_CODE_MODI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MODI_name, &const_BLOCK_CODE_MODI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MODI_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_MRO_value;
	ZVAL_LONG(&const_BLOCK_CODE_MRO_value, UBLOCK_MRO);
	zend_string *const_BLOCK_CODE_MRO_name = zend_string_init_interned("BLOCK_CODE_MRO", sizeof("BLOCK_CODE_MRO") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MRO_name, &const_BLOCK_CODE_MRO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MRO_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_MYANMAR_EXTENDED_B_value;
	ZVAL_LONG(&const_BLOCK_CODE_MYANMAR_EXTENDED_B_value, UBLOCK_MYANMAR_EXTENDED_B);
	zend_string *const_BLOCK_CODE_MYANMAR_EXTENDED_B_name = zend_string_init_interned("BLOCK_CODE_MYANMAR_EXTENDED_B", sizeof("BLOCK_CODE_MYANMAR_EXTENDED_B") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_MYANMAR_EXTENDED_B_name, &const_BLOCK_CODE_MYANMAR_EXTENDED_B_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_MYANMAR_EXTENDED_B_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_NABATAEAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_NABATAEAN_value, UBLOCK_NABATAEAN);
	zend_string *const_BLOCK_CODE_NABATAEAN_name = zend_string_init_interned("BLOCK_CODE_NABATAEAN", sizeof("BLOCK_CODE_NABATAEAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_NABATAEAN_name, &const_BLOCK_CODE_NABATAEAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_NABATAEAN_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_OLD_NORTH_ARABIAN_value;
	ZVAL_LONG(&const_BLOCK_CODE_OLD_NORTH_ARABIAN_value, UBLOCK_OLD_NORTH_ARABIAN);
	zend_string *const_BLOCK_CODE_OLD_NORTH_ARABIAN_name = zend_string_init_interned("BLOCK_CODE_OLD_NORTH_ARABIAN", sizeof("BLOCK_CODE_OLD_NORTH_ARABIAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_OLD_NORTH_ARABIAN_name, &const_BLOCK_CODE_OLD_NORTH_ARABIAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_OLD_NORTH_ARABIAN_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_OLD_PERMIC_value;
	ZVAL_LONG(&const_BLOCK_CODE_OLD_PERMIC_value, UBLOCK_OLD_PERMIC);
	zend_string *const_BLOCK_CODE_OLD_PERMIC_name = zend_string_init_interned("BLOCK_CODE_OLD_PERMIC", sizeof("BLOCK_CODE_OLD_PERMIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_OLD_PERMIC_name, &const_BLOCK_CODE_OLD_PERMIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_OLD_PERMIC_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_ORNAMENTAL_DINGBATS_value;
	ZVAL_LONG(&const_BLOCK_CODE_ORNAMENTAL_DINGBATS_value, UBLOCK_ORNAMENTAL_DINGBATS);
	zend_string *const_BLOCK_CODE_ORNAMENTAL_DINGBATS_name = zend_string_init_interned("BLOCK_CODE_ORNAMENTAL_DINGBATS", sizeof("BLOCK_CODE_ORNAMENTAL_DINGBATS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_ORNAMENTAL_DINGBATS_name, &const_BLOCK_CODE_ORNAMENTAL_DINGBATS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_ORNAMENTAL_DINGBATS_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_PAHAWH_HMONG_value;
	ZVAL_LONG(&const_BLOCK_CODE_PAHAWH_HMONG_value, UBLOCK_PAHAWH_HMONG);
	zend_string *const_BLOCK_CODE_PAHAWH_HMONG_name = zend_string_init_interned("BLOCK_CODE_PAHAWH_HMONG", sizeof("BLOCK_CODE_PAHAWH_HMONG") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_PAHAWH_HMONG_name, &const_BLOCK_CODE_PAHAWH_HMONG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_PAHAWH_HMONG_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_PALMYRENE_value;
	ZVAL_LONG(&const_BLOCK_CODE_PALMYRENE_value, UBLOCK_PALMYRENE);
	zend_string *const_BLOCK_CODE_PALMYRENE_name = zend_string_init_interned("BLOCK_CODE_PALMYRENE", sizeof("BLOCK_CODE_PALMYRENE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_PALMYRENE_name, &const_BLOCK_CODE_PALMYRENE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_PALMYRENE_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_PAU_CIN_HAU_value;
	ZVAL_LONG(&const_BLOCK_CODE_PAU_CIN_HAU_value, UBLOCK_PAU_CIN_HAU);
	zend_string *const_BLOCK_CODE_PAU_CIN_HAU_name = zend_string_init_interned("BLOCK_CODE_PAU_CIN_HAU", sizeof("BLOCK_CODE_PAU_CIN_HAU") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_PAU_CIN_HAU_name, &const_BLOCK_CODE_PAU_CIN_HAU_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_PAU_CIN_HAU_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_PSALTER_PAHLAVI_value;
	ZVAL_LONG(&const_BLOCK_CODE_PSALTER_PAHLAVI_value, UBLOCK_PSALTER_PAHLAVI);
	zend_string *const_BLOCK_CODE_PSALTER_PAHLAVI_name = zend_string_init_interned("BLOCK_CODE_PSALTER_PAHLAVI", sizeof("BLOCK_CODE_PSALTER_PAHLAVI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_PSALTER_PAHLAVI_name, &const_BLOCK_CODE_PSALTER_PAHLAVI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_PSALTER_PAHLAVI_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_SHORTHAND_FORMAT_CONTROLS_value;
	ZVAL_LONG(&const_BLOCK_CODE_SHORTHAND_FORMAT_CONTROLS_value, UBLOCK_SHORTHAND_FORMAT_CONTROLS);
	zend_string *const_BLOCK_CODE_SHORTHAND_FORMAT_CONTROLS_name = zend_string_init_interned("BLOCK_CODE_SHORTHAND_FORMAT_CONTROLS", sizeof("BLOCK_CODE_SHORTHAND_FORMAT_CONTROLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SHORTHAND_FORMAT_CONTROLS_name, &const_BLOCK_CODE_SHORTHAND_FORMAT_CONTROLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SHORTHAND_FORMAT_CONTROLS_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_SIDDHAM_value;
	ZVAL_LONG(&const_BLOCK_CODE_SIDDHAM_value, UBLOCK_SIDDHAM);
	zend_string *const_BLOCK_CODE_SIDDHAM_name = zend_string_init_interned("BLOCK_CODE_SIDDHAM", sizeof("BLOCK_CODE_SIDDHAM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SIDDHAM_name, &const_BLOCK_CODE_SIDDHAM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SIDDHAM_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_SINHALA_ARCHAIC_NUMBERS_value;
	ZVAL_LONG(&const_BLOCK_CODE_SINHALA_ARCHAIC_NUMBERS_value, UBLOCK_SINHALA_ARCHAIC_NUMBERS);
	zend_string *const_BLOCK_CODE_SINHALA_ARCHAIC_NUMBERS_name = zend_string_init_interned("BLOCK_CODE_SINHALA_ARCHAIC_NUMBERS", sizeof("BLOCK_CODE_SINHALA_ARCHAIC_NUMBERS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SINHALA_ARCHAIC_NUMBERS_name, &const_BLOCK_CODE_SINHALA_ARCHAIC_NUMBERS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SINHALA_ARCHAIC_NUMBERS_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_C_value;
	ZVAL_LONG(&const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_C_value, UBLOCK_SUPPLEMENTAL_ARROWS_C);
	zend_string *const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_C_name = zend_string_init_interned("BLOCK_CODE_SUPPLEMENTAL_ARROWS_C", sizeof("BLOCK_CODE_SUPPLEMENTAL_ARROWS_C") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_C_name, &const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_C_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_SUPPLEMENTAL_ARROWS_C_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_TIRHUTA_value;
	ZVAL_LONG(&const_BLOCK_CODE_TIRHUTA_value, UBLOCK_TIRHUTA);
	zend_string *const_BLOCK_CODE_TIRHUTA_name = zend_string_init_interned("BLOCK_CODE_TIRHUTA", sizeof("BLOCK_CODE_TIRHUTA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_TIRHUTA_name, &const_BLOCK_CODE_TIRHUTA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_TIRHUTA_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_BLOCK_CODE_WARANG_CITI_value;
	ZVAL_LONG(&const_BLOCK_CODE_WARANG_CITI_value, UBLOCK_WARANG_CITI);
	zend_string *const_BLOCK_CODE_WARANG_CITI_name = zend_string_init_interned("BLOCK_CODE_WARANG_CITI", sizeof("BLOCK_CODE_WARANG_CITI") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_WARANG_CITI_name, &const_BLOCK_CODE_WARANG_CITI_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_WARANG_CITI_name);
#endif

	zval const_BLOCK_CODE_COUNT_value;
	ZVAL_LONG(&const_BLOCK_CODE_COUNT_value, UBLOCK_COUNT);
	zend_string *const_BLOCK_CODE_COUNT_name = zend_string_init_interned("BLOCK_CODE_COUNT", sizeof("BLOCK_CODE_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_COUNT_name, &const_BLOCK_CODE_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_COUNT_name);

	zval const_BLOCK_CODE_INVALID_CODE_value;
	ZVAL_LONG(&const_BLOCK_CODE_INVALID_CODE_value, UBLOCK_INVALID_CODE);
	zend_string *const_BLOCK_CODE_INVALID_CODE_name = zend_string_init_interned("BLOCK_CODE_INVALID_CODE", sizeof("BLOCK_CODE_INVALID_CODE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BLOCK_CODE_INVALID_CODE_name, &const_BLOCK_CODE_INVALID_CODE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BLOCK_CODE_INVALID_CODE_name);
#if U_ICU_VERSION_MAJOR_NUM >= 52

	zval const_BPT_NONE_value;
	ZVAL_LONG(&const_BPT_NONE_value, U_BPT_NONE);
	zend_string *const_BPT_NONE_name = zend_string_init_interned("BPT_NONE", sizeof("BPT_NONE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BPT_NONE_name, &const_BPT_NONE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BPT_NONE_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 52

	zval const_BPT_OPEN_value;
	ZVAL_LONG(&const_BPT_OPEN_value, U_BPT_OPEN);
	zend_string *const_BPT_OPEN_name = zend_string_init_interned("BPT_OPEN", sizeof("BPT_OPEN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BPT_OPEN_name, &const_BPT_OPEN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BPT_OPEN_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 52

	zval const_BPT_CLOSE_value;
	ZVAL_LONG(&const_BPT_CLOSE_value, U_BPT_CLOSE);
	zend_string *const_BPT_CLOSE_name = zend_string_init_interned("BPT_CLOSE", sizeof("BPT_CLOSE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BPT_CLOSE_name, &const_BPT_CLOSE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BPT_CLOSE_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 52

	zval const_BPT_COUNT_value;
	ZVAL_LONG(&const_BPT_COUNT_value, U_BPT_COUNT);
	zend_string *const_BPT_COUNT_name = zend_string_init_interned("BPT_COUNT", sizeof("BPT_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_BPT_COUNT_name, &const_BPT_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_BPT_COUNT_name);
#endif

	zval const_EA_NEUTRAL_value;
	ZVAL_LONG(&const_EA_NEUTRAL_value, U_EA_NEUTRAL);
	zend_string *const_EA_NEUTRAL_name = zend_string_init_interned("EA_NEUTRAL", sizeof("EA_NEUTRAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EA_NEUTRAL_name, &const_EA_NEUTRAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EA_NEUTRAL_name);

	zval const_EA_AMBIGUOUS_value;
	ZVAL_LONG(&const_EA_AMBIGUOUS_value, U_EA_AMBIGUOUS);
	zend_string *const_EA_AMBIGUOUS_name = zend_string_init_interned("EA_AMBIGUOUS", sizeof("EA_AMBIGUOUS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EA_AMBIGUOUS_name, &const_EA_AMBIGUOUS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EA_AMBIGUOUS_name);

	zval const_EA_HALFWIDTH_value;
	ZVAL_LONG(&const_EA_HALFWIDTH_value, U_EA_HALFWIDTH);
	zend_string *const_EA_HALFWIDTH_name = zend_string_init_interned("EA_HALFWIDTH", sizeof("EA_HALFWIDTH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EA_HALFWIDTH_name, &const_EA_HALFWIDTH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EA_HALFWIDTH_name);

	zval const_EA_FULLWIDTH_value;
	ZVAL_LONG(&const_EA_FULLWIDTH_value, U_EA_FULLWIDTH);
	zend_string *const_EA_FULLWIDTH_name = zend_string_init_interned("EA_FULLWIDTH", sizeof("EA_FULLWIDTH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EA_FULLWIDTH_name, &const_EA_FULLWIDTH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EA_FULLWIDTH_name);

	zval const_EA_NARROW_value;
	ZVAL_LONG(&const_EA_NARROW_value, U_EA_NARROW);
	zend_string *const_EA_NARROW_name = zend_string_init_interned("EA_NARROW", sizeof("EA_NARROW") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EA_NARROW_name, &const_EA_NARROW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EA_NARROW_name);

	zval const_EA_WIDE_value;
	ZVAL_LONG(&const_EA_WIDE_value, U_EA_WIDE);
	zend_string *const_EA_WIDE_name = zend_string_init_interned("EA_WIDE", sizeof("EA_WIDE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EA_WIDE_name, &const_EA_WIDE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EA_WIDE_name);

	zval const_EA_COUNT_value;
	ZVAL_LONG(&const_EA_COUNT_value, U_EA_COUNT);
	zend_string *const_EA_COUNT_name = zend_string_init_interned("EA_COUNT", sizeof("EA_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EA_COUNT_name, &const_EA_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EA_COUNT_name);

	zval const_UNICODE_CHAR_NAME_value;
	ZVAL_LONG(&const_UNICODE_CHAR_NAME_value, U_UNICODE_CHAR_NAME);
	zend_string *const_UNICODE_CHAR_NAME_name = zend_string_init_interned("UNICODE_CHAR_NAME", sizeof("UNICODE_CHAR_NAME") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_UNICODE_CHAR_NAME_name, &const_UNICODE_CHAR_NAME_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_UNICODE_CHAR_NAME_name);

	zval const_UNICODE_10_CHAR_NAME_value;
	ZVAL_LONG(&const_UNICODE_10_CHAR_NAME_value, U_UNICODE_10_CHAR_NAME);
	zend_string *const_UNICODE_10_CHAR_NAME_name = zend_string_init_interned("UNICODE_10_CHAR_NAME", sizeof("UNICODE_10_CHAR_NAME") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_UNICODE_10_CHAR_NAME_name, &const_UNICODE_10_CHAR_NAME_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_UNICODE_10_CHAR_NAME_name);

	zval const_EXTENDED_CHAR_NAME_value;
	ZVAL_LONG(&const_EXTENDED_CHAR_NAME_value, U_EXTENDED_CHAR_NAME);
	zend_string *const_EXTENDED_CHAR_NAME_name = zend_string_init_interned("EXTENDED_CHAR_NAME", sizeof("EXTENDED_CHAR_NAME") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EXTENDED_CHAR_NAME_name, &const_EXTENDED_CHAR_NAME_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EXTENDED_CHAR_NAME_name);

	zval const_CHAR_NAME_ALIAS_value;
	ZVAL_LONG(&const_CHAR_NAME_ALIAS_value, U_CHAR_NAME_ALIAS);
	zend_string *const_CHAR_NAME_ALIAS_name = zend_string_init_interned("CHAR_NAME_ALIAS", sizeof("CHAR_NAME_ALIAS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_NAME_ALIAS_name, &const_CHAR_NAME_ALIAS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_NAME_ALIAS_name);

	zval const_CHAR_NAME_CHOICE_COUNT_value;
	ZVAL_LONG(&const_CHAR_NAME_CHOICE_COUNT_value, U_CHAR_NAME_CHOICE_COUNT);
	zend_string *const_CHAR_NAME_CHOICE_COUNT_name = zend_string_init_interned("CHAR_NAME_CHOICE_COUNT", sizeof("CHAR_NAME_CHOICE_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CHAR_NAME_CHOICE_COUNT_name, &const_CHAR_NAME_CHOICE_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CHAR_NAME_CHOICE_COUNT_name);

	zval const_SHORT_PROPERTY_NAME_value;
	ZVAL_LONG(&const_SHORT_PROPERTY_NAME_value, U_SHORT_PROPERTY_NAME);
	zend_string *const_SHORT_PROPERTY_NAME_name = zend_string_init_interned("SHORT_PROPERTY_NAME", sizeof("SHORT_PROPERTY_NAME") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SHORT_PROPERTY_NAME_name, &const_SHORT_PROPERTY_NAME_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SHORT_PROPERTY_NAME_name);

	zval const_LONG_PROPERTY_NAME_value;
	ZVAL_LONG(&const_LONG_PROPERTY_NAME_value, U_LONG_PROPERTY_NAME);
	zend_string *const_LONG_PROPERTY_NAME_name = zend_string_init_interned("LONG_PROPERTY_NAME", sizeof("LONG_PROPERTY_NAME") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LONG_PROPERTY_NAME_name, &const_LONG_PROPERTY_NAME_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LONG_PROPERTY_NAME_name);

	zval const_PROPERTY_NAME_CHOICE_COUNT_value;
	ZVAL_LONG(&const_PROPERTY_NAME_CHOICE_COUNT_value, U_PROPERTY_NAME_CHOICE_COUNT);
	zend_string *const_PROPERTY_NAME_CHOICE_COUNT_name = zend_string_init_interned("PROPERTY_NAME_CHOICE_COUNT", sizeof("PROPERTY_NAME_CHOICE_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PROPERTY_NAME_CHOICE_COUNT_name, &const_PROPERTY_NAME_CHOICE_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PROPERTY_NAME_CHOICE_COUNT_name);

	zval const_DT_NONE_value;
	ZVAL_LONG(&const_DT_NONE_value, U_DT_NONE);
	zend_string *const_DT_NONE_name = zend_string_init_interned("DT_NONE", sizeof("DT_NONE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_NONE_name, &const_DT_NONE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_NONE_name);

	zval const_DT_CANONICAL_value;
	ZVAL_LONG(&const_DT_CANONICAL_value, U_DT_CANONICAL);
	zend_string *const_DT_CANONICAL_name = zend_string_init_interned("DT_CANONICAL", sizeof("DT_CANONICAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_CANONICAL_name, &const_DT_CANONICAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_CANONICAL_name);

	zval const_DT_COMPAT_value;
	ZVAL_LONG(&const_DT_COMPAT_value, U_DT_COMPAT);
	zend_string *const_DT_COMPAT_name = zend_string_init_interned("DT_COMPAT", sizeof("DT_COMPAT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_COMPAT_name, &const_DT_COMPAT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_COMPAT_name);

	zval const_DT_CIRCLE_value;
	ZVAL_LONG(&const_DT_CIRCLE_value, U_DT_CIRCLE);
	zend_string *const_DT_CIRCLE_name = zend_string_init_interned("DT_CIRCLE", sizeof("DT_CIRCLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_CIRCLE_name, &const_DT_CIRCLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_CIRCLE_name);

	zval const_DT_FINAL_value;
	ZVAL_LONG(&const_DT_FINAL_value, U_DT_FINAL);
	zend_string *const_DT_FINAL_name = zend_string_init_interned("DT_FINAL", sizeof("DT_FINAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_FINAL_name, &const_DT_FINAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_FINAL_name);

	zval const_DT_FONT_value;
	ZVAL_LONG(&const_DT_FONT_value, U_DT_FONT);
	zend_string *const_DT_FONT_name = zend_string_init_interned("DT_FONT", sizeof("DT_FONT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_FONT_name, &const_DT_FONT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_FONT_name);

	zval const_DT_FRACTION_value;
	ZVAL_LONG(&const_DT_FRACTION_value, U_DT_FRACTION);
	zend_string *const_DT_FRACTION_name = zend_string_init_interned("DT_FRACTION", sizeof("DT_FRACTION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_FRACTION_name, &const_DT_FRACTION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_FRACTION_name);

	zval const_DT_INITIAL_value;
	ZVAL_LONG(&const_DT_INITIAL_value, U_DT_INITIAL);
	zend_string *const_DT_INITIAL_name = zend_string_init_interned("DT_INITIAL", sizeof("DT_INITIAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_INITIAL_name, &const_DT_INITIAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_INITIAL_name);

	zval const_DT_ISOLATED_value;
	ZVAL_LONG(&const_DT_ISOLATED_value, U_DT_ISOLATED);
	zend_string *const_DT_ISOLATED_name = zend_string_init_interned("DT_ISOLATED", sizeof("DT_ISOLATED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_ISOLATED_name, &const_DT_ISOLATED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_ISOLATED_name);

	zval const_DT_MEDIAL_value;
	ZVAL_LONG(&const_DT_MEDIAL_value, U_DT_MEDIAL);
	zend_string *const_DT_MEDIAL_name = zend_string_init_interned("DT_MEDIAL", sizeof("DT_MEDIAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_MEDIAL_name, &const_DT_MEDIAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_MEDIAL_name);

	zval const_DT_NARROW_value;
	ZVAL_LONG(&const_DT_NARROW_value, U_DT_NARROW);
	zend_string *const_DT_NARROW_name = zend_string_init_interned("DT_NARROW", sizeof("DT_NARROW") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_NARROW_name, &const_DT_NARROW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_NARROW_name);

	zval const_DT_NOBREAK_value;
	ZVAL_LONG(&const_DT_NOBREAK_value, U_DT_NOBREAK);
	zend_string *const_DT_NOBREAK_name = zend_string_init_interned("DT_NOBREAK", sizeof("DT_NOBREAK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_NOBREAK_name, &const_DT_NOBREAK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_NOBREAK_name);

	zval const_DT_SMALL_value;
	ZVAL_LONG(&const_DT_SMALL_value, U_DT_SMALL);
	zend_string *const_DT_SMALL_name = zend_string_init_interned("DT_SMALL", sizeof("DT_SMALL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_SMALL_name, &const_DT_SMALL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_SMALL_name);

	zval const_DT_SQUARE_value;
	ZVAL_LONG(&const_DT_SQUARE_value, U_DT_SQUARE);
	zend_string *const_DT_SQUARE_name = zend_string_init_interned("DT_SQUARE", sizeof("DT_SQUARE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_SQUARE_name, &const_DT_SQUARE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_SQUARE_name);

	zval const_DT_SUB_value;
	ZVAL_LONG(&const_DT_SUB_value, U_DT_SUB);
	zend_string *const_DT_SUB_name = zend_string_init_interned("DT_SUB", sizeof("DT_SUB") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_SUB_name, &const_DT_SUB_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_SUB_name);

	zval const_DT_SUPER_value;
	ZVAL_LONG(&const_DT_SUPER_value, U_DT_SUPER);
	zend_string *const_DT_SUPER_name = zend_string_init_interned("DT_SUPER", sizeof("DT_SUPER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_SUPER_name, &const_DT_SUPER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_SUPER_name);

	zval const_DT_VERTICAL_value;
	ZVAL_LONG(&const_DT_VERTICAL_value, U_DT_VERTICAL);
	zend_string *const_DT_VERTICAL_name = zend_string_init_interned("DT_VERTICAL", sizeof("DT_VERTICAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_VERTICAL_name, &const_DT_VERTICAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_VERTICAL_name);

	zval const_DT_WIDE_value;
	ZVAL_LONG(&const_DT_WIDE_value, U_DT_WIDE);
	zend_string *const_DT_WIDE_name = zend_string_init_interned("DT_WIDE", sizeof("DT_WIDE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_WIDE_name, &const_DT_WIDE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_WIDE_name);

	zval const_DT_COUNT_value;
	ZVAL_LONG(&const_DT_COUNT_value, U_DT_COUNT);
	zend_string *const_DT_COUNT_name = zend_string_init_interned("DT_COUNT", sizeof("DT_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DT_COUNT_name, &const_DT_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DT_COUNT_name);

	zval const_JT_NON_JOINING_value;
	ZVAL_LONG(&const_JT_NON_JOINING_value, U_JT_NON_JOINING);
	zend_string *const_JT_NON_JOINING_name = zend_string_init_interned("JT_NON_JOINING", sizeof("JT_NON_JOINING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JT_NON_JOINING_name, &const_JT_NON_JOINING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JT_NON_JOINING_name);

	zval const_JT_JOIN_CAUSING_value;
	ZVAL_LONG(&const_JT_JOIN_CAUSING_value, U_JT_JOIN_CAUSING);
	zend_string *const_JT_JOIN_CAUSING_name = zend_string_init_interned("JT_JOIN_CAUSING", sizeof("JT_JOIN_CAUSING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JT_JOIN_CAUSING_name, &const_JT_JOIN_CAUSING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JT_JOIN_CAUSING_name);

	zval const_JT_DUAL_JOINING_value;
	ZVAL_LONG(&const_JT_DUAL_JOINING_value, U_JT_DUAL_JOINING);
	zend_string *const_JT_DUAL_JOINING_name = zend_string_init_interned("JT_DUAL_JOINING", sizeof("JT_DUAL_JOINING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JT_DUAL_JOINING_name, &const_JT_DUAL_JOINING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JT_DUAL_JOINING_name);

	zval const_JT_LEFT_JOINING_value;
	ZVAL_LONG(&const_JT_LEFT_JOINING_value, U_JT_LEFT_JOINING);
	zend_string *const_JT_LEFT_JOINING_name = zend_string_init_interned("JT_LEFT_JOINING", sizeof("JT_LEFT_JOINING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JT_LEFT_JOINING_name, &const_JT_LEFT_JOINING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JT_LEFT_JOINING_name);

	zval const_JT_RIGHT_JOINING_value;
	ZVAL_LONG(&const_JT_RIGHT_JOINING_value, U_JT_RIGHT_JOINING);
	zend_string *const_JT_RIGHT_JOINING_name = zend_string_init_interned("JT_RIGHT_JOINING", sizeof("JT_RIGHT_JOINING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JT_RIGHT_JOINING_name, &const_JT_RIGHT_JOINING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JT_RIGHT_JOINING_name);

	zval const_JT_TRANSPARENT_value;
	ZVAL_LONG(&const_JT_TRANSPARENT_value, U_JT_TRANSPARENT);
	zend_string *const_JT_TRANSPARENT_name = zend_string_init_interned("JT_TRANSPARENT", sizeof("JT_TRANSPARENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JT_TRANSPARENT_name, &const_JT_TRANSPARENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JT_TRANSPARENT_name);

	zval const_JT_COUNT_value;
	ZVAL_LONG(&const_JT_COUNT_value, U_JT_COUNT);
	zend_string *const_JT_COUNT_name = zend_string_init_interned("JT_COUNT", sizeof("JT_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JT_COUNT_name, &const_JT_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JT_COUNT_name);

	zval const_JG_NO_JOINING_GROUP_value;
	ZVAL_LONG(&const_JG_NO_JOINING_GROUP_value, U_JG_NO_JOINING_GROUP);
	zend_string *const_JG_NO_JOINING_GROUP_name = zend_string_init_interned("JG_NO_JOINING_GROUP", sizeof("JG_NO_JOINING_GROUP") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_NO_JOINING_GROUP_name, &const_JG_NO_JOINING_GROUP_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_NO_JOINING_GROUP_name);

	zval const_JG_AIN_value;
	ZVAL_LONG(&const_JG_AIN_value, U_JG_AIN);
	zend_string *const_JG_AIN_name = zend_string_init_interned("JG_AIN", sizeof("JG_AIN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_AIN_name, &const_JG_AIN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_AIN_name);

	zval const_JG_ALAPH_value;
	ZVAL_LONG(&const_JG_ALAPH_value, U_JG_ALAPH);
	zend_string *const_JG_ALAPH_name = zend_string_init_interned("JG_ALAPH", sizeof("JG_ALAPH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_ALAPH_name, &const_JG_ALAPH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_ALAPH_name);

	zval const_JG_ALEF_value;
	ZVAL_LONG(&const_JG_ALEF_value, U_JG_ALEF);
	zend_string *const_JG_ALEF_name = zend_string_init_interned("JG_ALEF", sizeof("JG_ALEF") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_ALEF_name, &const_JG_ALEF_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_ALEF_name);

	zval const_JG_BEH_value;
	ZVAL_LONG(&const_JG_BEH_value, U_JG_BEH);
	zend_string *const_JG_BEH_name = zend_string_init_interned("JG_BEH", sizeof("JG_BEH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_BEH_name, &const_JG_BEH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_BEH_name);

	zval const_JG_BETH_value;
	ZVAL_LONG(&const_JG_BETH_value, U_JG_BETH);
	zend_string *const_JG_BETH_name = zend_string_init_interned("JG_BETH", sizeof("JG_BETH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_BETH_name, &const_JG_BETH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_BETH_name);

	zval const_JG_DAL_value;
	ZVAL_LONG(&const_JG_DAL_value, U_JG_DAL);
	zend_string *const_JG_DAL_name = zend_string_init_interned("JG_DAL", sizeof("JG_DAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_DAL_name, &const_JG_DAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_DAL_name);

	zval const_JG_DALATH_RISH_value;
	ZVAL_LONG(&const_JG_DALATH_RISH_value, U_JG_DALATH_RISH);
	zend_string *const_JG_DALATH_RISH_name = zend_string_init_interned("JG_DALATH_RISH", sizeof("JG_DALATH_RISH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_DALATH_RISH_name, &const_JG_DALATH_RISH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_DALATH_RISH_name);

	zval const_JG_E_value;
	ZVAL_LONG(&const_JG_E_value, U_JG_E);
	zend_string *const_JG_E_name = zend_string_init_interned("JG_E", sizeof("JG_E") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_E_name, &const_JG_E_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_E_name);

	zval const_JG_FEH_value;
	ZVAL_LONG(&const_JG_FEH_value, U_JG_FEH);
	zend_string *const_JG_FEH_name = zend_string_init_interned("JG_FEH", sizeof("JG_FEH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_FEH_name, &const_JG_FEH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_FEH_name);

	zval const_JG_FINAL_SEMKATH_value;
	ZVAL_LONG(&const_JG_FINAL_SEMKATH_value, U_JG_FINAL_SEMKATH);
	zend_string *const_JG_FINAL_SEMKATH_name = zend_string_init_interned("JG_FINAL_SEMKATH", sizeof("JG_FINAL_SEMKATH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_FINAL_SEMKATH_name, &const_JG_FINAL_SEMKATH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_FINAL_SEMKATH_name);

	zval const_JG_GAF_value;
	ZVAL_LONG(&const_JG_GAF_value, U_JG_GAF);
	zend_string *const_JG_GAF_name = zend_string_init_interned("JG_GAF", sizeof("JG_GAF") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_GAF_name, &const_JG_GAF_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_GAF_name);

	zval const_JG_GAMAL_value;
	ZVAL_LONG(&const_JG_GAMAL_value, U_JG_GAMAL);
	zend_string *const_JG_GAMAL_name = zend_string_init_interned("JG_GAMAL", sizeof("JG_GAMAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_GAMAL_name, &const_JG_GAMAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_GAMAL_name);

	zval const_JG_HAH_value;
	ZVAL_LONG(&const_JG_HAH_value, U_JG_HAH);
	zend_string *const_JG_HAH_name = zend_string_init_interned("JG_HAH", sizeof("JG_HAH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_HAH_name, &const_JG_HAH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_HAH_name);

	zval const_JG_TEH_MARBUTA_GOAL_value;
	ZVAL_LONG(&const_JG_TEH_MARBUTA_GOAL_value, U_JG_TEH_MARBUTA_GOAL);
	zend_string *const_JG_TEH_MARBUTA_GOAL_name = zend_string_init_interned("JG_TEH_MARBUTA_GOAL", sizeof("JG_TEH_MARBUTA_GOAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_TEH_MARBUTA_GOAL_name, &const_JG_TEH_MARBUTA_GOAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_TEH_MARBUTA_GOAL_name);

	zval const_JG_HAMZA_ON_HEH_GOAL_value;
	ZVAL_LONG(&const_JG_HAMZA_ON_HEH_GOAL_value, U_JG_HAMZA_ON_HEH_GOAL);
	zend_string *const_JG_HAMZA_ON_HEH_GOAL_name = zend_string_init_interned("JG_HAMZA_ON_HEH_GOAL", sizeof("JG_HAMZA_ON_HEH_GOAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_HAMZA_ON_HEH_GOAL_name, &const_JG_HAMZA_ON_HEH_GOAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_HAMZA_ON_HEH_GOAL_name);

	zval const_JG_HE_value;
	ZVAL_LONG(&const_JG_HE_value, U_JG_HE);
	zend_string *const_JG_HE_name = zend_string_init_interned("JG_HE", sizeof("JG_HE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_HE_name, &const_JG_HE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_HE_name);

	zval const_JG_HEH_value;
	ZVAL_LONG(&const_JG_HEH_value, U_JG_HEH);
	zend_string *const_JG_HEH_name = zend_string_init_interned("JG_HEH", sizeof("JG_HEH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_HEH_name, &const_JG_HEH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_HEH_name);

	zval const_JG_HEH_GOAL_value;
	ZVAL_LONG(&const_JG_HEH_GOAL_value, U_JG_HEH_GOAL);
	zend_string *const_JG_HEH_GOAL_name = zend_string_init_interned("JG_HEH_GOAL", sizeof("JG_HEH_GOAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_HEH_GOAL_name, &const_JG_HEH_GOAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_HEH_GOAL_name);

	zval const_JG_HETH_value;
	ZVAL_LONG(&const_JG_HETH_value, U_JG_HETH);
	zend_string *const_JG_HETH_name = zend_string_init_interned("JG_HETH", sizeof("JG_HETH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_HETH_name, &const_JG_HETH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_HETH_name);

	zval const_JG_KAF_value;
	ZVAL_LONG(&const_JG_KAF_value, U_JG_KAF);
	zend_string *const_JG_KAF_name = zend_string_init_interned("JG_KAF", sizeof("JG_KAF") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_KAF_name, &const_JG_KAF_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_KAF_name);

	zval const_JG_KAPH_value;
	ZVAL_LONG(&const_JG_KAPH_value, U_JG_KAPH);
	zend_string *const_JG_KAPH_name = zend_string_init_interned("JG_KAPH", sizeof("JG_KAPH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_KAPH_name, &const_JG_KAPH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_KAPH_name);

	zval const_JG_KNOTTED_HEH_value;
	ZVAL_LONG(&const_JG_KNOTTED_HEH_value, U_JG_KNOTTED_HEH);
	zend_string *const_JG_KNOTTED_HEH_name = zend_string_init_interned("JG_KNOTTED_HEH", sizeof("JG_KNOTTED_HEH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_KNOTTED_HEH_name, &const_JG_KNOTTED_HEH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_KNOTTED_HEH_name);

	zval const_JG_LAM_value;
	ZVAL_LONG(&const_JG_LAM_value, U_JG_LAM);
	zend_string *const_JG_LAM_name = zend_string_init_interned("JG_LAM", sizeof("JG_LAM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_LAM_name, &const_JG_LAM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_LAM_name);

	zval const_JG_LAMADH_value;
	ZVAL_LONG(&const_JG_LAMADH_value, U_JG_LAMADH);
	zend_string *const_JG_LAMADH_name = zend_string_init_interned("JG_LAMADH", sizeof("JG_LAMADH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_LAMADH_name, &const_JG_LAMADH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_LAMADH_name);

	zval const_JG_MEEM_value;
	ZVAL_LONG(&const_JG_MEEM_value, U_JG_MEEM);
	zend_string *const_JG_MEEM_name = zend_string_init_interned("JG_MEEM", sizeof("JG_MEEM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MEEM_name, &const_JG_MEEM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MEEM_name);

	zval const_JG_MIM_value;
	ZVAL_LONG(&const_JG_MIM_value, U_JG_MIM);
	zend_string *const_JG_MIM_name = zend_string_init_interned("JG_MIM", sizeof("JG_MIM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MIM_name, &const_JG_MIM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MIM_name);

	zval const_JG_NOON_value;
	ZVAL_LONG(&const_JG_NOON_value, U_JG_NOON);
	zend_string *const_JG_NOON_name = zend_string_init_interned("JG_NOON", sizeof("JG_NOON") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_NOON_name, &const_JG_NOON_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_NOON_name);

	zval const_JG_NUN_value;
	ZVAL_LONG(&const_JG_NUN_value, U_JG_NUN);
	zend_string *const_JG_NUN_name = zend_string_init_interned("JG_NUN", sizeof("JG_NUN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_NUN_name, &const_JG_NUN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_NUN_name);

	zval const_JG_PE_value;
	ZVAL_LONG(&const_JG_PE_value, U_JG_PE);
	zend_string *const_JG_PE_name = zend_string_init_interned("JG_PE", sizeof("JG_PE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_PE_name, &const_JG_PE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_PE_name);

	zval const_JG_QAF_value;
	ZVAL_LONG(&const_JG_QAF_value, U_JG_QAF);
	zend_string *const_JG_QAF_name = zend_string_init_interned("JG_QAF", sizeof("JG_QAF") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_QAF_name, &const_JG_QAF_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_QAF_name);

	zval const_JG_QAPH_value;
	ZVAL_LONG(&const_JG_QAPH_value, U_JG_QAPH);
	zend_string *const_JG_QAPH_name = zend_string_init_interned("JG_QAPH", sizeof("JG_QAPH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_QAPH_name, &const_JG_QAPH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_QAPH_name);

	zval const_JG_REH_value;
	ZVAL_LONG(&const_JG_REH_value, U_JG_REH);
	zend_string *const_JG_REH_name = zend_string_init_interned("JG_REH", sizeof("JG_REH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_REH_name, &const_JG_REH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_REH_name);

	zval const_JG_REVERSED_PE_value;
	ZVAL_LONG(&const_JG_REVERSED_PE_value, U_JG_REVERSED_PE);
	zend_string *const_JG_REVERSED_PE_name = zend_string_init_interned("JG_REVERSED_PE", sizeof("JG_REVERSED_PE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_REVERSED_PE_name, &const_JG_REVERSED_PE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_REVERSED_PE_name);

	zval const_JG_SAD_value;
	ZVAL_LONG(&const_JG_SAD_value, U_JG_SAD);
	zend_string *const_JG_SAD_name = zend_string_init_interned("JG_SAD", sizeof("JG_SAD") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_SAD_name, &const_JG_SAD_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_SAD_name);

	zval const_JG_SADHE_value;
	ZVAL_LONG(&const_JG_SADHE_value, U_JG_SADHE);
	zend_string *const_JG_SADHE_name = zend_string_init_interned("JG_SADHE", sizeof("JG_SADHE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_SADHE_name, &const_JG_SADHE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_SADHE_name);

	zval const_JG_SEEN_value;
	ZVAL_LONG(&const_JG_SEEN_value, U_JG_SEEN);
	zend_string *const_JG_SEEN_name = zend_string_init_interned("JG_SEEN", sizeof("JG_SEEN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_SEEN_name, &const_JG_SEEN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_SEEN_name);

	zval const_JG_SEMKATH_value;
	ZVAL_LONG(&const_JG_SEMKATH_value, U_JG_SEMKATH);
	zend_string *const_JG_SEMKATH_name = zend_string_init_interned("JG_SEMKATH", sizeof("JG_SEMKATH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_SEMKATH_name, &const_JG_SEMKATH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_SEMKATH_name);

	zval const_JG_SHIN_value;
	ZVAL_LONG(&const_JG_SHIN_value, U_JG_SHIN);
	zend_string *const_JG_SHIN_name = zend_string_init_interned("JG_SHIN", sizeof("JG_SHIN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_SHIN_name, &const_JG_SHIN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_SHIN_name);

	zval const_JG_SWASH_KAF_value;
	ZVAL_LONG(&const_JG_SWASH_KAF_value, U_JG_SWASH_KAF);
	zend_string *const_JG_SWASH_KAF_name = zend_string_init_interned("JG_SWASH_KAF", sizeof("JG_SWASH_KAF") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_SWASH_KAF_name, &const_JG_SWASH_KAF_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_SWASH_KAF_name);

	zval const_JG_SYRIAC_WAW_value;
	ZVAL_LONG(&const_JG_SYRIAC_WAW_value, U_JG_SYRIAC_WAW);
	zend_string *const_JG_SYRIAC_WAW_name = zend_string_init_interned("JG_SYRIAC_WAW", sizeof("JG_SYRIAC_WAW") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_SYRIAC_WAW_name, &const_JG_SYRIAC_WAW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_SYRIAC_WAW_name);

	zval const_JG_TAH_value;
	ZVAL_LONG(&const_JG_TAH_value, U_JG_TAH);
	zend_string *const_JG_TAH_name = zend_string_init_interned("JG_TAH", sizeof("JG_TAH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_TAH_name, &const_JG_TAH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_TAH_name);

	zval const_JG_TAW_value;
	ZVAL_LONG(&const_JG_TAW_value, U_JG_TAW);
	zend_string *const_JG_TAW_name = zend_string_init_interned("JG_TAW", sizeof("JG_TAW") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_TAW_name, &const_JG_TAW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_TAW_name);

	zval const_JG_TEH_MARBUTA_value;
	ZVAL_LONG(&const_JG_TEH_MARBUTA_value, U_JG_TEH_MARBUTA);
	zend_string *const_JG_TEH_MARBUTA_name = zend_string_init_interned("JG_TEH_MARBUTA", sizeof("JG_TEH_MARBUTA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_TEH_MARBUTA_name, &const_JG_TEH_MARBUTA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_TEH_MARBUTA_name);

	zval const_JG_TETH_value;
	ZVAL_LONG(&const_JG_TETH_value, U_JG_TETH);
	zend_string *const_JG_TETH_name = zend_string_init_interned("JG_TETH", sizeof("JG_TETH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_TETH_name, &const_JG_TETH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_TETH_name);

	zval const_JG_WAW_value;
	ZVAL_LONG(&const_JG_WAW_value, U_JG_WAW);
	zend_string *const_JG_WAW_name = zend_string_init_interned("JG_WAW", sizeof("JG_WAW") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_WAW_name, &const_JG_WAW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_WAW_name);

	zval const_JG_YEH_value;
	ZVAL_LONG(&const_JG_YEH_value, U_JG_YEH);
	zend_string *const_JG_YEH_name = zend_string_init_interned("JG_YEH", sizeof("JG_YEH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_YEH_name, &const_JG_YEH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_YEH_name);

	zval const_JG_YEH_BARREE_value;
	ZVAL_LONG(&const_JG_YEH_BARREE_value, U_JG_YEH_BARREE);
	zend_string *const_JG_YEH_BARREE_name = zend_string_init_interned("JG_YEH_BARREE", sizeof("JG_YEH_BARREE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_YEH_BARREE_name, &const_JG_YEH_BARREE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_YEH_BARREE_name);

	zval const_JG_YEH_WITH_TAIL_value;
	ZVAL_LONG(&const_JG_YEH_WITH_TAIL_value, U_JG_YEH_WITH_TAIL);
	zend_string *const_JG_YEH_WITH_TAIL_name = zend_string_init_interned("JG_YEH_WITH_TAIL", sizeof("JG_YEH_WITH_TAIL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_YEH_WITH_TAIL_name, &const_JG_YEH_WITH_TAIL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_YEH_WITH_TAIL_name);

	zval const_JG_YUDH_value;
	ZVAL_LONG(&const_JG_YUDH_value, U_JG_YUDH);
	zend_string *const_JG_YUDH_name = zend_string_init_interned("JG_YUDH", sizeof("JG_YUDH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_YUDH_name, &const_JG_YUDH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_YUDH_name);

	zval const_JG_YUDH_HE_value;
	ZVAL_LONG(&const_JG_YUDH_HE_value, U_JG_YUDH_HE);
	zend_string *const_JG_YUDH_HE_name = zend_string_init_interned("JG_YUDH_HE", sizeof("JG_YUDH_HE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_YUDH_HE_name, &const_JG_YUDH_HE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_YUDH_HE_name);

	zval const_JG_ZAIN_value;
	ZVAL_LONG(&const_JG_ZAIN_value, U_JG_ZAIN);
	zend_string *const_JG_ZAIN_name = zend_string_init_interned("JG_ZAIN", sizeof("JG_ZAIN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_ZAIN_name, &const_JG_ZAIN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_ZAIN_name);

	zval const_JG_FE_value;
	ZVAL_LONG(&const_JG_FE_value, U_JG_FE);
	zend_string *const_JG_FE_name = zend_string_init_interned("JG_FE", sizeof("JG_FE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_FE_name, &const_JG_FE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_FE_name);

	zval const_JG_KHAPH_value;
	ZVAL_LONG(&const_JG_KHAPH_value, U_JG_KHAPH);
	zend_string *const_JG_KHAPH_name = zend_string_init_interned("JG_KHAPH", sizeof("JG_KHAPH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_KHAPH_name, &const_JG_KHAPH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_KHAPH_name);

	zval const_JG_ZHAIN_value;
	ZVAL_LONG(&const_JG_ZHAIN_value, U_JG_ZHAIN);
	zend_string *const_JG_ZHAIN_name = zend_string_init_interned("JG_ZHAIN", sizeof("JG_ZHAIN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_ZHAIN_name, &const_JG_ZHAIN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_ZHAIN_name);

	zval const_JG_BURUSHASKI_YEH_BARREE_value;
	ZVAL_LONG(&const_JG_BURUSHASKI_YEH_BARREE_value, U_JG_BURUSHASKI_YEH_BARREE);
	zend_string *const_JG_BURUSHASKI_YEH_BARREE_name = zend_string_init_interned("JG_BURUSHASKI_YEH_BARREE", sizeof("JG_BURUSHASKI_YEH_BARREE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_BURUSHASKI_YEH_BARREE_name, &const_JG_BURUSHASKI_YEH_BARREE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_BURUSHASKI_YEH_BARREE_name);

	zval const_JG_FARSI_YEH_value;
	ZVAL_LONG(&const_JG_FARSI_YEH_value, U_JG_FARSI_YEH);
	zend_string *const_JG_FARSI_YEH_name = zend_string_init_interned("JG_FARSI_YEH", sizeof("JG_FARSI_YEH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_FARSI_YEH_name, &const_JG_FARSI_YEH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_FARSI_YEH_name);

	zval const_JG_NYA_value;
	ZVAL_LONG(&const_JG_NYA_value, U_JG_NYA);
	zend_string *const_JG_NYA_name = zend_string_init_interned("JG_NYA", sizeof("JG_NYA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_NYA_name, &const_JG_NYA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_NYA_name);

	zval const_JG_ROHINGYA_YEH_value;
	ZVAL_LONG(&const_JG_ROHINGYA_YEH_value, U_JG_ROHINGYA_YEH);
	zend_string *const_JG_ROHINGYA_YEH_name = zend_string_init_interned("JG_ROHINGYA_YEH", sizeof("JG_ROHINGYA_YEH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_ROHINGYA_YEH_name, &const_JG_ROHINGYA_YEH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_ROHINGYA_YEH_name);
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_ALEPH_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_ALEPH_value, U_JG_MANICHAEAN_ALEPH);
	zend_string *const_JG_MANICHAEAN_ALEPH_name = zend_string_init_interned("JG_MANICHAEAN_ALEPH", sizeof("JG_MANICHAEAN_ALEPH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_ALEPH_name, &const_JG_MANICHAEAN_ALEPH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_ALEPH_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_AYIN_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_AYIN_value, U_JG_MANICHAEAN_AYIN);
	zend_string *const_JG_MANICHAEAN_AYIN_name = zend_string_init_interned("JG_MANICHAEAN_AYIN", sizeof("JG_MANICHAEAN_AYIN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_AYIN_name, &const_JG_MANICHAEAN_AYIN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_AYIN_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_BETH_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_BETH_value, U_JG_MANICHAEAN_BETH);
	zend_string *const_JG_MANICHAEAN_BETH_name = zend_string_init_interned("JG_MANICHAEAN_BETH", sizeof("JG_MANICHAEAN_BETH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_BETH_name, &const_JG_MANICHAEAN_BETH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_BETH_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_DALETH_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_DALETH_value, U_JG_MANICHAEAN_DALETH);
	zend_string *const_JG_MANICHAEAN_DALETH_name = zend_string_init_interned("JG_MANICHAEAN_DALETH", sizeof("JG_MANICHAEAN_DALETH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_DALETH_name, &const_JG_MANICHAEAN_DALETH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_DALETH_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_DHAMEDH_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_DHAMEDH_value, U_JG_MANICHAEAN_DHAMEDH);
	zend_string *const_JG_MANICHAEAN_DHAMEDH_name = zend_string_init_interned("JG_MANICHAEAN_DHAMEDH", sizeof("JG_MANICHAEAN_DHAMEDH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_DHAMEDH_name, &const_JG_MANICHAEAN_DHAMEDH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_DHAMEDH_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_FIVE_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_FIVE_value, U_JG_MANICHAEAN_FIVE);
	zend_string *const_JG_MANICHAEAN_FIVE_name = zend_string_init_interned("JG_MANICHAEAN_FIVE", sizeof("JG_MANICHAEAN_FIVE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_FIVE_name, &const_JG_MANICHAEAN_FIVE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_FIVE_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_GIMEL_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_GIMEL_value, U_JG_MANICHAEAN_GIMEL);
	zend_string *const_JG_MANICHAEAN_GIMEL_name = zend_string_init_interned("JG_MANICHAEAN_GIMEL", sizeof("JG_MANICHAEAN_GIMEL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_GIMEL_name, &const_JG_MANICHAEAN_GIMEL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_GIMEL_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_HETH_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_HETH_value, U_JG_MANICHAEAN_HETH);
	zend_string *const_JG_MANICHAEAN_HETH_name = zend_string_init_interned("JG_MANICHAEAN_HETH", sizeof("JG_MANICHAEAN_HETH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_HETH_name, &const_JG_MANICHAEAN_HETH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_HETH_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_HUNDRED_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_HUNDRED_value, U_JG_MANICHAEAN_HUNDRED);
	zend_string *const_JG_MANICHAEAN_HUNDRED_name = zend_string_init_interned("JG_MANICHAEAN_HUNDRED", sizeof("JG_MANICHAEAN_HUNDRED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_HUNDRED_name, &const_JG_MANICHAEAN_HUNDRED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_HUNDRED_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_KAPH_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_KAPH_value, U_JG_MANICHAEAN_KAPH);
	zend_string *const_JG_MANICHAEAN_KAPH_name = zend_string_init_interned("JG_MANICHAEAN_KAPH", sizeof("JG_MANICHAEAN_KAPH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_KAPH_name, &const_JG_MANICHAEAN_KAPH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_KAPH_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_LAMEDH_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_LAMEDH_value, U_JG_MANICHAEAN_LAMEDH);
	zend_string *const_JG_MANICHAEAN_LAMEDH_name = zend_string_init_interned("JG_MANICHAEAN_LAMEDH", sizeof("JG_MANICHAEAN_LAMEDH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_LAMEDH_name, &const_JG_MANICHAEAN_LAMEDH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_LAMEDH_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_MEM_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_MEM_value, U_JG_MANICHAEAN_MEM);
	zend_string *const_JG_MANICHAEAN_MEM_name = zend_string_init_interned("JG_MANICHAEAN_MEM", sizeof("JG_MANICHAEAN_MEM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_MEM_name, &const_JG_MANICHAEAN_MEM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_MEM_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_NUN_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_NUN_value, U_JG_MANICHAEAN_NUN);
	zend_string *const_JG_MANICHAEAN_NUN_name = zend_string_init_interned("JG_MANICHAEAN_NUN", sizeof("JG_MANICHAEAN_NUN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_NUN_name, &const_JG_MANICHAEAN_NUN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_NUN_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_ONE_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_ONE_value, U_JG_MANICHAEAN_ONE);
	zend_string *const_JG_MANICHAEAN_ONE_name = zend_string_init_interned("JG_MANICHAEAN_ONE", sizeof("JG_MANICHAEAN_ONE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_ONE_name, &const_JG_MANICHAEAN_ONE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_ONE_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_PE_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_PE_value, U_JG_MANICHAEAN_PE);
	zend_string *const_JG_MANICHAEAN_PE_name = zend_string_init_interned("JG_MANICHAEAN_PE", sizeof("JG_MANICHAEAN_PE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_PE_name, &const_JG_MANICHAEAN_PE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_PE_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_QOPH_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_QOPH_value, U_JG_MANICHAEAN_QOPH);
	zend_string *const_JG_MANICHAEAN_QOPH_name = zend_string_init_interned("JG_MANICHAEAN_QOPH", sizeof("JG_MANICHAEAN_QOPH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_QOPH_name, &const_JG_MANICHAEAN_QOPH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_QOPH_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_RESH_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_RESH_value, U_JG_MANICHAEAN_RESH);
	zend_string *const_JG_MANICHAEAN_RESH_name = zend_string_init_interned("JG_MANICHAEAN_RESH", sizeof("JG_MANICHAEAN_RESH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_RESH_name, &const_JG_MANICHAEAN_RESH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_RESH_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_SADHE_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_SADHE_value, U_JG_MANICHAEAN_SADHE);
	zend_string *const_JG_MANICHAEAN_SADHE_name = zend_string_init_interned("JG_MANICHAEAN_SADHE", sizeof("JG_MANICHAEAN_SADHE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_SADHE_name, &const_JG_MANICHAEAN_SADHE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_SADHE_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_SAMEKH_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_SAMEKH_value, U_JG_MANICHAEAN_SAMEKH);
	zend_string *const_JG_MANICHAEAN_SAMEKH_name = zend_string_init_interned("JG_MANICHAEAN_SAMEKH", sizeof("JG_MANICHAEAN_SAMEKH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_SAMEKH_name, &const_JG_MANICHAEAN_SAMEKH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_SAMEKH_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_TAW_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_TAW_value, U_JG_MANICHAEAN_TAW);
	zend_string *const_JG_MANICHAEAN_TAW_name = zend_string_init_interned("JG_MANICHAEAN_TAW", sizeof("JG_MANICHAEAN_TAW") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_TAW_name, &const_JG_MANICHAEAN_TAW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_TAW_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_TEN_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_TEN_value, U_JG_MANICHAEAN_TEN);
	zend_string *const_JG_MANICHAEAN_TEN_name = zend_string_init_interned("JG_MANICHAEAN_TEN", sizeof("JG_MANICHAEAN_TEN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_TEN_name, &const_JG_MANICHAEAN_TEN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_TEN_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_TETH_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_TETH_value, U_JG_MANICHAEAN_TETH);
	zend_string *const_JG_MANICHAEAN_TETH_name = zend_string_init_interned("JG_MANICHAEAN_TETH", sizeof("JG_MANICHAEAN_TETH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_TETH_name, &const_JG_MANICHAEAN_TETH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_TETH_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_THAMEDH_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_THAMEDH_value, U_JG_MANICHAEAN_THAMEDH);
	zend_string *const_JG_MANICHAEAN_THAMEDH_name = zend_string_init_interned("JG_MANICHAEAN_THAMEDH", sizeof("JG_MANICHAEAN_THAMEDH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_THAMEDH_name, &const_JG_MANICHAEAN_THAMEDH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_THAMEDH_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_TWENTY_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_TWENTY_value, U_JG_MANICHAEAN_TWENTY);
	zend_string *const_JG_MANICHAEAN_TWENTY_name = zend_string_init_interned("JG_MANICHAEAN_TWENTY", sizeof("JG_MANICHAEAN_TWENTY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_TWENTY_name, &const_JG_MANICHAEAN_TWENTY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_TWENTY_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_WAW_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_WAW_value, U_JG_MANICHAEAN_WAW);
	zend_string *const_JG_MANICHAEAN_WAW_name = zend_string_init_interned("JG_MANICHAEAN_WAW", sizeof("JG_MANICHAEAN_WAW") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_WAW_name, &const_JG_MANICHAEAN_WAW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_WAW_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_YODH_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_YODH_value, U_JG_MANICHAEAN_YODH);
	zend_string *const_JG_MANICHAEAN_YODH_name = zend_string_init_interned("JG_MANICHAEAN_YODH", sizeof("JG_MANICHAEAN_YODH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_YODH_name, &const_JG_MANICHAEAN_YODH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_YODH_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_MANICHAEAN_ZAYIN_value;
	ZVAL_LONG(&const_JG_MANICHAEAN_ZAYIN_value, U_JG_MANICHAEAN_ZAYIN);
	zend_string *const_JG_MANICHAEAN_ZAYIN_name = zend_string_init_interned("JG_MANICHAEAN_ZAYIN", sizeof("JG_MANICHAEAN_ZAYIN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_MANICHAEAN_ZAYIN_name, &const_JG_MANICHAEAN_ZAYIN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_MANICHAEAN_ZAYIN_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 54

	zval const_JG_STRAIGHT_WAW_value;
	ZVAL_LONG(&const_JG_STRAIGHT_WAW_value, U_JG_STRAIGHT_WAW);
	zend_string *const_JG_STRAIGHT_WAW_name = zend_string_init_interned("JG_STRAIGHT_WAW", sizeof("JG_STRAIGHT_WAW") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_STRAIGHT_WAW_name, &const_JG_STRAIGHT_WAW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_STRAIGHT_WAW_name);
#endif

	zval const_JG_COUNT_value;
	ZVAL_LONG(&const_JG_COUNT_value, U_JG_COUNT);
	zend_string *const_JG_COUNT_name = zend_string_init_interned("JG_COUNT", sizeof("JG_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_JG_COUNT_name, &const_JG_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_JG_COUNT_name);

	zval const_GCB_OTHER_value;
	ZVAL_LONG(&const_GCB_OTHER_value, U_GCB_OTHER);
	zend_string *const_GCB_OTHER_name = zend_string_init_interned("GCB_OTHER", sizeof("GCB_OTHER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GCB_OTHER_name, &const_GCB_OTHER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_GCB_OTHER_name);

	zval const_GCB_CONTROL_value;
	ZVAL_LONG(&const_GCB_CONTROL_value, U_GCB_CONTROL);
	zend_string *const_GCB_CONTROL_name = zend_string_init_interned("GCB_CONTROL", sizeof("GCB_CONTROL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GCB_CONTROL_name, &const_GCB_CONTROL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_GCB_CONTROL_name);

	zval const_GCB_CR_value;
	ZVAL_LONG(&const_GCB_CR_value, U_GCB_CR);
	zend_string *const_GCB_CR_name = zend_string_init_interned("GCB_CR", sizeof("GCB_CR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GCB_CR_name, &const_GCB_CR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_GCB_CR_name);

	zval const_GCB_EXTEND_value;
	ZVAL_LONG(&const_GCB_EXTEND_value, U_GCB_EXTEND);
	zend_string *const_GCB_EXTEND_name = zend_string_init_interned("GCB_EXTEND", sizeof("GCB_EXTEND") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GCB_EXTEND_name, &const_GCB_EXTEND_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_GCB_EXTEND_name);

	zval const_GCB_L_value;
	ZVAL_LONG(&const_GCB_L_value, U_GCB_L);
	zend_string *const_GCB_L_name = zend_string_init_interned("GCB_L", sizeof("GCB_L") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GCB_L_name, &const_GCB_L_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_GCB_L_name);

	zval const_GCB_LF_value;
	ZVAL_LONG(&const_GCB_LF_value, U_GCB_LF);
	zend_string *const_GCB_LF_name = zend_string_init_interned("GCB_LF", sizeof("GCB_LF") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GCB_LF_name, &const_GCB_LF_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_GCB_LF_name);

	zval const_GCB_LV_value;
	ZVAL_LONG(&const_GCB_LV_value, U_GCB_LV);
	zend_string *const_GCB_LV_name = zend_string_init_interned("GCB_LV", sizeof("GCB_LV") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GCB_LV_name, &const_GCB_LV_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_GCB_LV_name);

	zval const_GCB_LVT_value;
	ZVAL_LONG(&const_GCB_LVT_value, U_GCB_LVT);
	zend_string *const_GCB_LVT_name = zend_string_init_interned("GCB_LVT", sizeof("GCB_LVT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GCB_LVT_name, &const_GCB_LVT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_GCB_LVT_name);

	zval const_GCB_T_value;
	ZVAL_LONG(&const_GCB_T_value, U_GCB_T);
	zend_string *const_GCB_T_name = zend_string_init_interned("GCB_T", sizeof("GCB_T") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GCB_T_name, &const_GCB_T_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_GCB_T_name);

	zval const_GCB_V_value;
	ZVAL_LONG(&const_GCB_V_value, U_GCB_V);
	zend_string *const_GCB_V_name = zend_string_init_interned("GCB_V", sizeof("GCB_V") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GCB_V_name, &const_GCB_V_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_GCB_V_name);

	zval const_GCB_SPACING_MARK_value;
	ZVAL_LONG(&const_GCB_SPACING_MARK_value, U_GCB_SPACING_MARK);
	zend_string *const_GCB_SPACING_MARK_name = zend_string_init_interned("GCB_SPACING_MARK", sizeof("GCB_SPACING_MARK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GCB_SPACING_MARK_name, &const_GCB_SPACING_MARK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_GCB_SPACING_MARK_name);

	zval const_GCB_PREPEND_value;
	ZVAL_LONG(&const_GCB_PREPEND_value, U_GCB_PREPEND);
	zend_string *const_GCB_PREPEND_name = zend_string_init_interned("GCB_PREPEND", sizeof("GCB_PREPEND") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GCB_PREPEND_name, &const_GCB_PREPEND_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_GCB_PREPEND_name);

	zval const_GCB_REGIONAL_INDICATOR_value;
	ZVAL_LONG(&const_GCB_REGIONAL_INDICATOR_value, U_GCB_REGIONAL_INDICATOR);
	zend_string *const_GCB_REGIONAL_INDICATOR_name = zend_string_init_interned("GCB_REGIONAL_INDICATOR", sizeof("GCB_REGIONAL_INDICATOR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GCB_REGIONAL_INDICATOR_name, &const_GCB_REGIONAL_INDICATOR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_GCB_REGIONAL_INDICATOR_name);

	zval const_GCB_COUNT_value;
	ZVAL_LONG(&const_GCB_COUNT_value, U_GCB_COUNT);
	zend_string *const_GCB_COUNT_name = zend_string_init_interned("GCB_COUNT", sizeof("GCB_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_GCB_COUNT_name, &const_GCB_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_GCB_COUNT_name);

	zval const_WB_OTHER_value;
	ZVAL_LONG(&const_WB_OTHER_value, U_WB_OTHER);
	zend_string *const_WB_OTHER_name = zend_string_init_interned("WB_OTHER", sizeof("WB_OTHER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_OTHER_name, &const_WB_OTHER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_OTHER_name);

	zval const_WB_ALETTER_value;
	ZVAL_LONG(&const_WB_ALETTER_value, U_WB_ALETTER);
	zend_string *const_WB_ALETTER_name = zend_string_init_interned("WB_ALETTER", sizeof("WB_ALETTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_ALETTER_name, &const_WB_ALETTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_ALETTER_name);

	zval const_WB_FORMAT_value;
	ZVAL_LONG(&const_WB_FORMAT_value, U_WB_FORMAT);
	zend_string *const_WB_FORMAT_name = zend_string_init_interned("WB_FORMAT", sizeof("WB_FORMAT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_FORMAT_name, &const_WB_FORMAT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_FORMAT_name);

	zval const_WB_KATAKANA_value;
	ZVAL_LONG(&const_WB_KATAKANA_value, U_WB_KATAKANA);
	zend_string *const_WB_KATAKANA_name = zend_string_init_interned("WB_KATAKANA", sizeof("WB_KATAKANA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_KATAKANA_name, &const_WB_KATAKANA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_KATAKANA_name);

	zval const_WB_MIDLETTER_value;
	ZVAL_LONG(&const_WB_MIDLETTER_value, U_WB_MIDLETTER);
	zend_string *const_WB_MIDLETTER_name = zend_string_init_interned("WB_MIDLETTER", sizeof("WB_MIDLETTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_MIDLETTER_name, &const_WB_MIDLETTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_MIDLETTER_name);

	zval const_WB_MIDNUM_value;
	ZVAL_LONG(&const_WB_MIDNUM_value, U_WB_MIDNUM);
	zend_string *const_WB_MIDNUM_name = zend_string_init_interned("WB_MIDNUM", sizeof("WB_MIDNUM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_MIDNUM_name, &const_WB_MIDNUM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_MIDNUM_name);

	zval const_WB_NUMERIC_value;
	ZVAL_LONG(&const_WB_NUMERIC_value, U_WB_NUMERIC);
	zend_string *const_WB_NUMERIC_name = zend_string_init_interned("WB_NUMERIC", sizeof("WB_NUMERIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_NUMERIC_name, &const_WB_NUMERIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_NUMERIC_name);

	zval const_WB_EXTENDNUMLET_value;
	ZVAL_LONG(&const_WB_EXTENDNUMLET_value, U_WB_EXTENDNUMLET);
	zend_string *const_WB_EXTENDNUMLET_name = zend_string_init_interned("WB_EXTENDNUMLET", sizeof("WB_EXTENDNUMLET") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_EXTENDNUMLET_name, &const_WB_EXTENDNUMLET_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_EXTENDNUMLET_name);

	zval const_WB_CR_value;
	ZVAL_LONG(&const_WB_CR_value, U_WB_CR);
	zend_string *const_WB_CR_name = zend_string_init_interned("WB_CR", sizeof("WB_CR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_CR_name, &const_WB_CR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_CR_name);

	zval const_WB_EXTEND_value;
	ZVAL_LONG(&const_WB_EXTEND_value, U_WB_EXTEND);
	zend_string *const_WB_EXTEND_name = zend_string_init_interned("WB_EXTEND", sizeof("WB_EXTEND") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_EXTEND_name, &const_WB_EXTEND_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_EXTEND_name);

	zval const_WB_LF_value;
	ZVAL_LONG(&const_WB_LF_value, U_WB_LF);
	zend_string *const_WB_LF_name = zend_string_init_interned("WB_LF", sizeof("WB_LF") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_LF_name, &const_WB_LF_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_LF_name);

	zval const_WB_MIDNUMLET_value;
	ZVAL_LONG(&const_WB_MIDNUMLET_value, U_WB_MIDNUMLET);
	zend_string *const_WB_MIDNUMLET_name = zend_string_init_interned("WB_MIDNUMLET", sizeof("WB_MIDNUMLET") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_MIDNUMLET_name, &const_WB_MIDNUMLET_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_MIDNUMLET_name);

	zval const_WB_NEWLINE_value;
	ZVAL_LONG(&const_WB_NEWLINE_value, U_WB_NEWLINE);
	zend_string *const_WB_NEWLINE_name = zend_string_init_interned("WB_NEWLINE", sizeof("WB_NEWLINE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_NEWLINE_name, &const_WB_NEWLINE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_NEWLINE_name);

	zval const_WB_REGIONAL_INDICATOR_value;
	ZVAL_LONG(&const_WB_REGIONAL_INDICATOR_value, U_WB_REGIONAL_INDICATOR);
	zend_string *const_WB_REGIONAL_INDICATOR_name = zend_string_init_interned("WB_REGIONAL_INDICATOR", sizeof("WB_REGIONAL_INDICATOR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_REGIONAL_INDICATOR_name, &const_WB_REGIONAL_INDICATOR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_REGIONAL_INDICATOR_name);
#if U_ICU_VERSION_MAJOR_NUM >= 52

	zval const_WB_HEBREW_LETTER_value;
	ZVAL_LONG(&const_WB_HEBREW_LETTER_value, U_WB_HEBREW_LETTER);
	zend_string *const_WB_HEBREW_LETTER_name = zend_string_init_interned("WB_HEBREW_LETTER", sizeof("WB_HEBREW_LETTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_HEBREW_LETTER_name, &const_WB_HEBREW_LETTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_HEBREW_LETTER_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 52

	zval const_WB_SINGLE_QUOTE_value;
	ZVAL_LONG(&const_WB_SINGLE_QUOTE_value, U_WB_SINGLE_QUOTE);
	zend_string *const_WB_SINGLE_QUOTE_name = zend_string_init_interned("WB_SINGLE_QUOTE", sizeof("WB_SINGLE_QUOTE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_SINGLE_QUOTE_name, &const_WB_SINGLE_QUOTE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_SINGLE_QUOTE_name);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 52

	zval const_WB_DOUBLE_QUOTE_value;
	ZVAL_LONG(&const_WB_DOUBLE_QUOTE_value, U_WB_DOUBLE_QUOTE);
	zend_string *const_WB_DOUBLE_QUOTE_name = zend_string_init_interned("WB_DOUBLE_QUOTE", sizeof("WB_DOUBLE_QUOTE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_DOUBLE_QUOTE_name, &const_WB_DOUBLE_QUOTE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_DOUBLE_QUOTE_name);
#endif

	zval const_WB_COUNT_value;
	ZVAL_LONG(&const_WB_COUNT_value, U_WB_COUNT);
	zend_string *const_WB_COUNT_name = zend_string_init_interned("WB_COUNT", sizeof("WB_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WB_COUNT_name, &const_WB_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WB_COUNT_name);

	zval const_SB_OTHER_value;
	ZVAL_LONG(&const_SB_OTHER_value, U_SB_OTHER);
	zend_string *const_SB_OTHER_name = zend_string_init_interned("SB_OTHER", sizeof("SB_OTHER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SB_OTHER_name, &const_SB_OTHER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SB_OTHER_name);

	zval const_SB_ATERM_value;
	ZVAL_LONG(&const_SB_ATERM_value, U_SB_ATERM);
	zend_string *const_SB_ATERM_name = zend_string_init_interned("SB_ATERM", sizeof("SB_ATERM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SB_ATERM_name, &const_SB_ATERM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SB_ATERM_name);

	zval const_SB_CLOSE_value;
	ZVAL_LONG(&const_SB_CLOSE_value, U_SB_CLOSE);
	zend_string *const_SB_CLOSE_name = zend_string_init_interned("SB_CLOSE", sizeof("SB_CLOSE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SB_CLOSE_name, &const_SB_CLOSE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SB_CLOSE_name);

	zval const_SB_FORMAT_value;
	ZVAL_LONG(&const_SB_FORMAT_value, U_SB_FORMAT);
	zend_string *const_SB_FORMAT_name = zend_string_init_interned("SB_FORMAT", sizeof("SB_FORMAT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SB_FORMAT_name, &const_SB_FORMAT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SB_FORMAT_name);

	zval const_SB_LOWER_value;
	ZVAL_LONG(&const_SB_LOWER_value, U_SB_LOWER);
	zend_string *const_SB_LOWER_name = zend_string_init_interned("SB_LOWER", sizeof("SB_LOWER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SB_LOWER_name, &const_SB_LOWER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SB_LOWER_name);

	zval const_SB_NUMERIC_value;
	ZVAL_LONG(&const_SB_NUMERIC_value, U_SB_NUMERIC);
	zend_string *const_SB_NUMERIC_name = zend_string_init_interned("SB_NUMERIC", sizeof("SB_NUMERIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SB_NUMERIC_name, &const_SB_NUMERIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SB_NUMERIC_name);

	zval const_SB_OLETTER_value;
	ZVAL_LONG(&const_SB_OLETTER_value, U_SB_OLETTER);
	zend_string *const_SB_OLETTER_name = zend_string_init_interned("SB_OLETTER", sizeof("SB_OLETTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SB_OLETTER_name, &const_SB_OLETTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SB_OLETTER_name);

	zval const_SB_SEP_value;
	ZVAL_LONG(&const_SB_SEP_value, U_SB_SEP);
	zend_string *const_SB_SEP_name = zend_string_init_interned("SB_SEP", sizeof("SB_SEP") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SB_SEP_name, &const_SB_SEP_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SB_SEP_name);

	zval const_SB_SP_value;
	ZVAL_LONG(&const_SB_SP_value, U_SB_SP);
	zend_string *const_SB_SP_name = zend_string_init_interned("SB_SP", sizeof("SB_SP") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SB_SP_name, &const_SB_SP_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SB_SP_name);

	zval const_SB_STERM_value;
	ZVAL_LONG(&const_SB_STERM_value, U_SB_STERM);
	zend_string *const_SB_STERM_name = zend_string_init_interned("SB_STERM", sizeof("SB_STERM") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SB_STERM_name, &const_SB_STERM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SB_STERM_name);

	zval const_SB_UPPER_value;
	ZVAL_LONG(&const_SB_UPPER_value, U_SB_UPPER);
	zend_string *const_SB_UPPER_name = zend_string_init_interned("SB_UPPER", sizeof("SB_UPPER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SB_UPPER_name, &const_SB_UPPER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SB_UPPER_name);

	zval const_SB_CR_value;
	ZVAL_LONG(&const_SB_CR_value, U_SB_CR);
	zend_string *const_SB_CR_name = zend_string_init_interned("SB_CR", sizeof("SB_CR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SB_CR_name, &const_SB_CR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SB_CR_name);

	zval const_SB_EXTEND_value;
	ZVAL_LONG(&const_SB_EXTEND_value, U_SB_EXTEND);
	zend_string *const_SB_EXTEND_name = zend_string_init_interned("SB_EXTEND", sizeof("SB_EXTEND") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SB_EXTEND_name, &const_SB_EXTEND_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SB_EXTEND_name);

	zval const_SB_LF_value;
	ZVAL_LONG(&const_SB_LF_value, U_SB_LF);
	zend_string *const_SB_LF_name = zend_string_init_interned("SB_LF", sizeof("SB_LF") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SB_LF_name, &const_SB_LF_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SB_LF_name);

	zval const_SB_SCONTINUE_value;
	ZVAL_LONG(&const_SB_SCONTINUE_value, U_SB_SCONTINUE);
	zend_string *const_SB_SCONTINUE_name = zend_string_init_interned("SB_SCONTINUE", sizeof("SB_SCONTINUE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SB_SCONTINUE_name, &const_SB_SCONTINUE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SB_SCONTINUE_name);

	zval const_SB_COUNT_value;
	ZVAL_LONG(&const_SB_COUNT_value, U_SB_COUNT);
	zend_string *const_SB_COUNT_name = zend_string_init_interned("SB_COUNT", sizeof("SB_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SB_COUNT_name, &const_SB_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SB_COUNT_name);

	zval const_LB_UNKNOWN_value;
	ZVAL_LONG(&const_LB_UNKNOWN_value, U_LB_UNKNOWN);
	zend_string *const_LB_UNKNOWN_name = zend_string_init_interned("LB_UNKNOWN", sizeof("LB_UNKNOWN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_UNKNOWN_name, &const_LB_UNKNOWN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_UNKNOWN_name);

	zval const_LB_AMBIGUOUS_value;
	ZVAL_LONG(&const_LB_AMBIGUOUS_value, U_LB_AMBIGUOUS);
	zend_string *const_LB_AMBIGUOUS_name = zend_string_init_interned("LB_AMBIGUOUS", sizeof("LB_AMBIGUOUS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_AMBIGUOUS_name, &const_LB_AMBIGUOUS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_AMBIGUOUS_name);

	zval const_LB_ALPHABETIC_value;
	ZVAL_LONG(&const_LB_ALPHABETIC_value, U_LB_ALPHABETIC);
	zend_string *const_LB_ALPHABETIC_name = zend_string_init_interned("LB_ALPHABETIC", sizeof("LB_ALPHABETIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_ALPHABETIC_name, &const_LB_ALPHABETIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_ALPHABETIC_name);

	zval const_LB_BREAK_BOTH_value;
	ZVAL_LONG(&const_LB_BREAK_BOTH_value, U_LB_BREAK_BOTH);
	zend_string *const_LB_BREAK_BOTH_name = zend_string_init_interned("LB_BREAK_BOTH", sizeof("LB_BREAK_BOTH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_BREAK_BOTH_name, &const_LB_BREAK_BOTH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_BREAK_BOTH_name);

	zval const_LB_BREAK_AFTER_value;
	ZVAL_LONG(&const_LB_BREAK_AFTER_value, U_LB_BREAK_AFTER);
	zend_string *const_LB_BREAK_AFTER_name = zend_string_init_interned("LB_BREAK_AFTER", sizeof("LB_BREAK_AFTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_BREAK_AFTER_name, &const_LB_BREAK_AFTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_BREAK_AFTER_name);

	zval const_LB_BREAK_BEFORE_value;
	ZVAL_LONG(&const_LB_BREAK_BEFORE_value, U_LB_BREAK_BEFORE);
	zend_string *const_LB_BREAK_BEFORE_name = zend_string_init_interned("LB_BREAK_BEFORE", sizeof("LB_BREAK_BEFORE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_BREAK_BEFORE_name, &const_LB_BREAK_BEFORE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_BREAK_BEFORE_name);

	zval const_LB_MANDATORY_BREAK_value;
	ZVAL_LONG(&const_LB_MANDATORY_BREAK_value, U_LB_MANDATORY_BREAK);
	zend_string *const_LB_MANDATORY_BREAK_name = zend_string_init_interned("LB_MANDATORY_BREAK", sizeof("LB_MANDATORY_BREAK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_MANDATORY_BREAK_name, &const_LB_MANDATORY_BREAK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_MANDATORY_BREAK_name);

	zval const_LB_CONTINGENT_BREAK_value;
	ZVAL_LONG(&const_LB_CONTINGENT_BREAK_value, U_LB_CONTINGENT_BREAK);
	zend_string *const_LB_CONTINGENT_BREAK_name = zend_string_init_interned("LB_CONTINGENT_BREAK", sizeof("LB_CONTINGENT_BREAK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_CONTINGENT_BREAK_name, &const_LB_CONTINGENT_BREAK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_CONTINGENT_BREAK_name);

	zval const_LB_CLOSE_PUNCTUATION_value;
	ZVAL_LONG(&const_LB_CLOSE_PUNCTUATION_value, U_LB_CLOSE_PUNCTUATION);
	zend_string *const_LB_CLOSE_PUNCTUATION_name = zend_string_init_interned("LB_CLOSE_PUNCTUATION", sizeof("LB_CLOSE_PUNCTUATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_CLOSE_PUNCTUATION_name, &const_LB_CLOSE_PUNCTUATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_CLOSE_PUNCTUATION_name);

	zval const_LB_COMBINING_MARK_value;
	ZVAL_LONG(&const_LB_COMBINING_MARK_value, U_LB_COMBINING_MARK);
	zend_string *const_LB_COMBINING_MARK_name = zend_string_init_interned("LB_COMBINING_MARK", sizeof("LB_COMBINING_MARK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_COMBINING_MARK_name, &const_LB_COMBINING_MARK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_COMBINING_MARK_name);

	zval const_LB_CARRIAGE_RETURN_value;
	ZVAL_LONG(&const_LB_CARRIAGE_RETURN_value, U_LB_CARRIAGE_RETURN);
	zend_string *const_LB_CARRIAGE_RETURN_name = zend_string_init_interned("LB_CARRIAGE_RETURN", sizeof("LB_CARRIAGE_RETURN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_CARRIAGE_RETURN_name, &const_LB_CARRIAGE_RETURN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_CARRIAGE_RETURN_name);

	zval const_LB_EXCLAMATION_value;
	ZVAL_LONG(&const_LB_EXCLAMATION_value, U_LB_EXCLAMATION);
	zend_string *const_LB_EXCLAMATION_name = zend_string_init_interned("LB_EXCLAMATION", sizeof("LB_EXCLAMATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_EXCLAMATION_name, &const_LB_EXCLAMATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_EXCLAMATION_name);

	zval const_LB_GLUE_value;
	ZVAL_LONG(&const_LB_GLUE_value, U_LB_GLUE);
	zend_string *const_LB_GLUE_name = zend_string_init_interned("LB_GLUE", sizeof("LB_GLUE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_GLUE_name, &const_LB_GLUE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_GLUE_name);

	zval const_LB_HYPHEN_value;
	ZVAL_LONG(&const_LB_HYPHEN_value, U_LB_HYPHEN);
	zend_string *const_LB_HYPHEN_name = zend_string_init_interned("LB_HYPHEN", sizeof("LB_HYPHEN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_HYPHEN_name, &const_LB_HYPHEN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_HYPHEN_name);

	zval const_LB_IDEOGRAPHIC_value;
	ZVAL_LONG(&const_LB_IDEOGRAPHIC_value, U_LB_IDEOGRAPHIC);
	zend_string *const_LB_IDEOGRAPHIC_name = zend_string_init_interned("LB_IDEOGRAPHIC", sizeof("LB_IDEOGRAPHIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_IDEOGRAPHIC_name, &const_LB_IDEOGRAPHIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_IDEOGRAPHIC_name);

	zval const_LB_INSEPARABLE_value;
	ZVAL_LONG(&const_LB_INSEPARABLE_value, U_LB_INSEPARABLE);
	zend_string *const_LB_INSEPARABLE_name = zend_string_init_interned("LB_INSEPARABLE", sizeof("LB_INSEPARABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_INSEPARABLE_name, &const_LB_INSEPARABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_INSEPARABLE_name);

	zval const_LB_INSEPERABLE_value;
	ZVAL_LONG(&const_LB_INSEPERABLE_value, U_LB_INSEPERABLE);
	zend_string *const_LB_INSEPERABLE_name = zend_string_init_interned("LB_INSEPERABLE", sizeof("LB_INSEPERABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_INSEPERABLE_name, &const_LB_INSEPERABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_INSEPERABLE_name);

	zval const_LB_INFIX_NUMERIC_value;
	ZVAL_LONG(&const_LB_INFIX_NUMERIC_value, U_LB_INFIX_NUMERIC);
	zend_string *const_LB_INFIX_NUMERIC_name = zend_string_init_interned("LB_INFIX_NUMERIC", sizeof("LB_INFIX_NUMERIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_INFIX_NUMERIC_name, &const_LB_INFIX_NUMERIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_INFIX_NUMERIC_name);

	zval const_LB_LINE_FEED_value;
	ZVAL_LONG(&const_LB_LINE_FEED_value, U_LB_LINE_FEED);
	zend_string *const_LB_LINE_FEED_name = zend_string_init_interned("LB_LINE_FEED", sizeof("LB_LINE_FEED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_LINE_FEED_name, &const_LB_LINE_FEED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_LINE_FEED_name);

	zval const_LB_NONSTARTER_value;
	ZVAL_LONG(&const_LB_NONSTARTER_value, U_LB_NONSTARTER);
	zend_string *const_LB_NONSTARTER_name = zend_string_init_interned("LB_NONSTARTER", sizeof("LB_NONSTARTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_NONSTARTER_name, &const_LB_NONSTARTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_NONSTARTER_name);

	zval const_LB_NUMERIC_value;
	ZVAL_LONG(&const_LB_NUMERIC_value, U_LB_NUMERIC);
	zend_string *const_LB_NUMERIC_name = zend_string_init_interned("LB_NUMERIC", sizeof("LB_NUMERIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_NUMERIC_name, &const_LB_NUMERIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_NUMERIC_name);

	zval const_LB_OPEN_PUNCTUATION_value;
	ZVAL_LONG(&const_LB_OPEN_PUNCTUATION_value, U_LB_OPEN_PUNCTUATION);
	zend_string *const_LB_OPEN_PUNCTUATION_name = zend_string_init_interned("LB_OPEN_PUNCTUATION", sizeof("LB_OPEN_PUNCTUATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_OPEN_PUNCTUATION_name, &const_LB_OPEN_PUNCTUATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_OPEN_PUNCTUATION_name);

	zval const_LB_POSTFIX_NUMERIC_value;
	ZVAL_LONG(&const_LB_POSTFIX_NUMERIC_value, U_LB_POSTFIX_NUMERIC);
	zend_string *const_LB_POSTFIX_NUMERIC_name = zend_string_init_interned("LB_POSTFIX_NUMERIC", sizeof("LB_POSTFIX_NUMERIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_POSTFIX_NUMERIC_name, &const_LB_POSTFIX_NUMERIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_POSTFIX_NUMERIC_name);

	zval const_LB_PREFIX_NUMERIC_value;
	ZVAL_LONG(&const_LB_PREFIX_NUMERIC_value, U_LB_PREFIX_NUMERIC);
	zend_string *const_LB_PREFIX_NUMERIC_name = zend_string_init_interned("LB_PREFIX_NUMERIC", sizeof("LB_PREFIX_NUMERIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_PREFIX_NUMERIC_name, &const_LB_PREFIX_NUMERIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_PREFIX_NUMERIC_name);

	zval const_LB_QUOTATION_value;
	ZVAL_LONG(&const_LB_QUOTATION_value, U_LB_QUOTATION);
	zend_string *const_LB_QUOTATION_name = zend_string_init_interned("LB_QUOTATION", sizeof("LB_QUOTATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_QUOTATION_name, &const_LB_QUOTATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_QUOTATION_name);

	zval const_LB_COMPLEX_CONTEXT_value;
	ZVAL_LONG(&const_LB_COMPLEX_CONTEXT_value, U_LB_COMPLEX_CONTEXT);
	zend_string *const_LB_COMPLEX_CONTEXT_name = zend_string_init_interned("LB_COMPLEX_CONTEXT", sizeof("LB_COMPLEX_CONTEXT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_COMPLEX_CONTEXT_name, &const_LB_COMPLEX_CONTEXT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_COMPLEX_CONTEXT_name);

	zval const_LB_SURROGATE_value;
	ZVAL_LONG(&const_LB_SURROGATE_value, U_LB_SURROGATE);
	zend_string *const_LB_SURROGATE_name = zend_string_init_interned("LB_SURROGATE", sizeof("LB_SURROGATE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_SURROGATE_name, &const_LB_SURROGATE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_SURROGATE_name);

	zval const_LB_SPACE_value;
	ZVAL_LONG(&const_LB_SPACE_value, U_LB_SPACE);
	zend_string *const_LB_SPACE_name = zend_string_init_interned("LB_SPACE", sizeof("LB_SPACE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_SPACE_name, &const_LB_SPACE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_SPACE_name);

	zval const_LB_BREAK_SYMBOLS_value;
	ZVAL_LONG(&const_LB_BREAK_SYMBOLS_value, U_LB_BREAK_SYMBOLS);
	zend_string *const_LB_BREAK_SYMBOLS_name = zend_string_init_interned("LB_BREAK_SYMBOLS", sizeof("LB_BREAK_SYMBOLS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_BREAK_SYMBOLS_name, &const_LB_BREAK_SYMBOLS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_BREAK_SYMBOLS_name);

	zval const_LB_ZWSPACE_value;
	ZVAL_LONG(&const_LB_ZWSPACE_value, U_LB_ZWSPACE);
	zend_string *const_LB_ZWSPACE_name = zend_string_init_interned("LB_ZWSPACE", sizeof("LB_ZWSPACE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_ZWSPACE_name, &const_LB_ZWSPACE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_ZWSPACE_name);

	zval const_LB_NEXT_LINE_value;
	ZVAL_LONG(&const_LB_NEXT_LINE_value, U_LB_NEXT_LINE);
	zend_string *const_LB_NEXT_LINE_name = zend_string_init_interned("LB_NEXT_LINE", sizeof("LB_NEXT_LINE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_NEXT_LINE_name, &const_LB_NEXT_LINE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_NEXT_LINE_name);

	zval const_LB_WORD_JOINER_value;
	ZVAL_LONG(&const_LB_WORD_JOINER_value, U_LB_WORD_JOINER);
	zend_string *const_LB_WORD_JOINER_name = zend_string_init_interned("LB_WORD_JOINER", sizeof("LB_WORD_JOINER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_WORD_JOINER_name, &const_LB_WORD_JOINER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_WORD_JOINER_name);

	zval const_LB_H2_value;
	ZVAL_LONG(&const_LB_H2_value, U_LB_H2);
	zend_string *const_LB_H2_name = zend_string_init_interned("LB_H2", sizeof("LB_H2") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_H2_name, &const_LB_H2_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_H2_name);

	zval const_LB_H3_value;
	ZVAL_LONG(&const_LB_H3_value, U_LB_H3);
	zend_string *const_LB_H3_name = zend_string_init_interned("LB_H3", sizeof("LB_H3") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_H3_name, &const_LB_H3_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_H3_name);

	zval const_LB_JL_value;
	ZVAL_LONG(&const_LB_JL_value, U_LB_JL);
	zend_string *const_LB_JL_name = zend_string_init_interned("LB_JL", sizeof("LB_JL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_JL_name, &const_LB_JL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_JL_name);

	zval const_LB_JT_value;
	ZVAL_LONG(&const_LB_JT_value, U_LB_JT);
	zend_string *const_LB_JT_name = zend_string_init_interned("LB_JT", sizeof("LB_JT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_JT_name, &const_LB_JT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_JT_name);

	zval const_LB_JV_value;
	ZVAL_LONG(&const_LB_JV_value, U_LB_JV);
	zend_string *const_LB_JV_name = zend_string_init_interned("LB_JV", sizeof("LB_JV") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_JV_name, &const_LB_JV_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_JV_name);

	zval const_LB_CLOSE_PARENTHESIS_value;
	ZVAL_LONG(&const_LB_CLOSE_PARENTHESIS_value, U_LB_CLOSE_PARENTHESIS);
	zend_string *const_LB_CLOSE_PARENTHESIS_name = zend_string_init_interned("LB_CLOSE_PARENTHESIS", sizeof("LB_CLOSE_PARENTHESIS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_CLOSE_PARENTHESIS_name, &const_LB_CLOSE_PARENTHESIS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_CLOSE_PARENTHESIS_name);

	zval const_LB_CONDITIONAL_JAPANESE_STARTER_value;
	ZVAL_LONG(&const_LB_CONDITIONAL_JAPANESE_STARTER_value, U_LB_CONDITIONAL_JAPANESE_STARTER);
	zend_string *const_LB_CONDITIONAL_JAPANESE_STARTER_name = zend_string_init_interned("LB_CONDITIONAL_JAPANESE_STARTER", sizeof("LB_CONDITIONAL_JAPANESE_STARTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_CONDITIONAL_JAPANESE_STARTER_name, &const_LB_CONDITIONAL_JAPANESE_STARTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_CONDITIONAL_JAPANESE_STARTER_name);

	zval const_LB_HEBREW_LETTER_value;
	ZVAL_LONG(&const_LB_HEBREW_LETTER_value, U_LB_HEBREW_LETTER);
	zend_string *const_LB_HEBREW_LETTER_name = zend_string_init_interned("LB_HEBREW_LETTER", sizeof("LB_HEBREW_LETTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_HEBREW_LETTER_name, &const_LB_HEBREW_LETTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_HEBREW_LETTER_name);

	zval const_LB_REGIONAL_INDICATOR_value;
	ZVAL_LONG(&const_LB_REGIONAL_INDICATOR_value, U_LB_REGIONAL_INDICATOR);
	zend_string *const_LB_REGIONAL_INDICATOR_name = zend_string_init_interned("LB_REGIONAL_INDICATOR", sizeof("LB_REGIONAL_INDICATOR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_REGIONAL_INDICATOR_name, &const_LB_REGIONAL_INDICATOR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_REGIONAL_INDICATOR_name);

	zval const_LB_COUNT_value;
	ZVAL_LONG(&const_LB_COUNT_value, U_LB_COUNT);
	zend_string *const_LB_COUNT_name = zend_string_init_interned("LB_COUNT", sizeof("LB_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_LB_COUNT_name, &const_LB_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_LB_COUNT_name);

	zval const_NT_NONE_value;
	ZVAL_LONG(&const_NT_NONE_value, U_NT_NONE);
	zend_string *const_NT_NONE_name = zend_string_init_interned("NT_NONE", sizeof("NT_NONE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_NT_NONE_name, &const_NT_NONE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_NT_NONE_name);

	zval const_NT_DECIMAL_value;
	ZVAL_LONG(&const_NT_DECIMAL_value, U_NT_DECIMAL);
	zend_string *const_NT_DECIMAL_name = zend_string_init_interned("NT_DECIMAL", sizeof("NT_DECIMAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_NT_DECIMAL_name, &const_NT_DECIMAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_NT_DECIMAL_name);

	zval const_NT_DIGIT_value;
	ZVAL_LONG(&const_NT_DIGIT_value, U_NT_DIGIT);
	zend_string *const_NT_DIGIT_name = zend_string_init_interned("NT_DIGIT", sizeof("NT_DIGIT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_NT_DIGIT_name, &const_NT_DIGIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_NT_DIGIT_name);

	zval const_NT_NUMERIC_value;
	ZVAL_LONG(&const_NT_NUMERIC_value, U_NT_NUMERIC);
	zend_string *const_NT_NUMERIC_name = zend_string_init_interned("NT_NUMERIC", sizeof("NT_NUMERIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_NT_NUMERIC_name, &const_NT_NUMERIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_NT_NUMERIC_name);

	zval const_NT_COUNT_value;
	ZVAL_LONG(&const_NT_COUNT_value, U_NT_COUNT);
	zend_string *const_NT_COUNT_name = zend_string_init_interned("NT_COUNT", sizeof("NT_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_NT_COUNT_name, &const_NT_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_NT_COUNT_name);

	zval const_HST_NOT_APPLICABLE_value;
	ZVAL_LONG(&const_HST_NOT_APPLICABLE_value, U_HST_NOT_APPLICABLE);
	zend_string *const_HST_NOT_APPLICABLE_name = zend_string_init_interned("HST_NOT_APPLICABLE", sizeof("HST_NOT_APPLICABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_HST_NOT_APPLICABLE_name, &const_HST_NOT_APPLICABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_HST_NOT_APPLICABLE_name);

	zval const_HST_LEADING_JAMO_value;
	ZVAL_LONG(&const_HST_LEADING_JAMO_value, U_HST_LEADING_JAMO);
	zend_string *const_HST_LEADING_JAMO_name = zend_string_init_interned("HST_LEADING_JAMO", sizeof("HST_LEADING_JAMO") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_HST_LEADING_JAMO_name, &const_HST_LEADING_JAMO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_HST_LEADING_JAMO_name);

	zval const_HST_VOWEL_JAMO_value;
	ZVAL_LONG(&const_HST_VOWEL_JAMO_value, U_HST_VOWEL_JAMO);
	zend_string *const_HST_VOWEL_JAMO_name = zend_string_init_interned("HST_VOWEL_JAMO", sizeof("HST_VOWEL_JAMO") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_HST_VOWEL_JAMO_name, &const_HST_VOWEL_JAMO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_HST_VOWEL_JAMO_name);

	zval const_HST_TRAILING_JAMO_value;
	ZVAL_LONG(&const_HST_TRAILING_JAMO_value, U_HST_TRAILING_JAMO);
	zend_string *const_HST_TRAILING_JAMO_name = zend_string_init_interned("HST_TRAILING_JAMO", sizeof("HST_TRAILING_JAMO") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_HST_TRAILING_JAMO_name, &const_HST_TRAILING_JAMO_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_HST_TRAILING_JAMO_name);

	zval const_HST_LV_SYLLABLE_value;
	ZVAL_LONG(&const_HST_LV_SYLLABLE_value, U_HST_LV_SYLLABLE);
	zend_string *const_HST_LV_SYLLABLE_name = zend_string_init_interned("HST_LV_SYLLABLE", sizeof("HST_LV_SYLLABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_HST_LV_SYLLABLE_name, &const_HST_LV_SYLLABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_HST_LV_SYLLABLE_name);

	zval const_HST_LVT_SYLLABLE_value;
	ZVAL_LONG(&const_HST_LVT_SYLLABLE_value, U_HST_LVT_SYLLABLE);
	zend_string *const_HST_LVT_SYLLABLE_name = zend_string_init_interned("HST_LVT_SYLLABLE", sizeof("HST_LVT_SYLLABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_HST_LVT_SYLLABLE_name, &const_HST_LVT_SYLLABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_HST_LVT_SYLLABLE_name);

	zval const_HST_COUNT_value;
	ZVAL_LONG(&const_HST_COUNT_value, U_HST_COUNT);
	zend_string *const_HST_COUNT_name = zend_string_init_interned("HST_COUNT", sizeof("HST_COUNT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_HST_COUNT_name, &const_HST_COUNT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_HST_COUNT_name);

	zval const_FOLD_CASE_DEFAULT_value;
	ZVAL_LONG(&const_FOLD_CASE_DEFAULT_value, U_FOLD_CASE_DEFAULT);
	zend_string *const_FOLD_CASE_DEFAULT_name = zend_string_init_interned("FOLD_CASE_DEFAULT", sizeof("FOLD_CASE_DEFAULT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FOLD_CASE_DEFAULT_name, &const_FOLD_CASE_DEFAULT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FOLD_CASE_DEFAULT_name);

	zval const_FOLD_CASE_EXCLUDE_SPECIAL_I_value;
	ZVAL_LONG(&const_FOLD_CASE_EXCLUDE_SPECIAL_I_value, U_FOLD_CASE_EXCLUDE_SPECIAL_I);
	zend_string *const_FOLD_CASE_EXCLUDE_SPECIAL_I_name = zend_string_init_interned("FOLD_CASE_EXCLUDE_SPECIAL_I", sizeof("FOLD_CASE_EXCLUDE_SPECIAL_I") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FOLD_CASE_EXCLUDE_SPECIAL_I_name, &const_FOLD_CASE_EXCLUDE_SPECIAL_I_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FOLD_CASE_EXCLUDE_SPECIAL_I_name);

	return class_entry;
}
