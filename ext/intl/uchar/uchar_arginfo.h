/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 05ea5d884642e75721f90728fb24b7ad5ce46897 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_hasBinaryProperty, 0, 0, 2)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, property, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_charAge, 0, 0, 1)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlChar_charDigitValue arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_charDirection arginfo_class_IntlChar_charAge

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_charFromName, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "IntlChar::UNICODE_CHAR_NAME")
ZEND_END_ARG_INFO()

#define arginfo_class_IntlChar_charMirror arginfo_class_IntlChar_charAge

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_charName, 0, 0, 1)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "IntlChar::UNICODE_CHAR_NAME")
ZEND_END_ARG_INFO()

#define arginfo_class_IntlChar_charType arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_chr arginfo_class_IntlChar_charAge

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_digit, 0, 0, 1)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, base, IS_LONG, 0, "10")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_enumCharNames, 0, 0, 3)
	ZEND_ARG_TYPE_MASK(0, start, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_MASK(0, end, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "IntlChar::UNICODE_CHAR_NAME")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_enumCharTypes, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_foldCase, 0, 0, 1)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "IntlChar::FOLD_CASE_DEFAULT")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_forDigit, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, digit, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, base, IS_LONG, 0, "10")
ZEND_END_ARG_INFO()

#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_getBidiPairedBracket, 0, 0, 1)
	ZEND_ARG_TYPE_MASK(0, codepoint, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()
#endif

#define arginfo_class_IntlChar_getBlockCode arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_getCombiningClass arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_getFC_NFKC_Closure arginfo_class_IntlChar_charAge

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_getIntPropertyMaxValue, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, property, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlChar_getIntPropertyMinValue arginfo_class_IntlChar_getIntPropertyMaxValue

#define arginfo_class_IntlChar_getIntPropertyValue arginfo_class_IntlChar_hasBinaryProperty

#define arginfo_class_IntlChar_getNumericValue arginfo_class_IntlChar_charAge

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_getPropertyEnum, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, alias, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_getPropertyName, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, property, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "IntlChar::LONG_PROPERTY_NAME")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_getPropertyValueEnum, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, property, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_getPropertyValueName, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, property, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "IntlChar::LONG_PROPERTY_NAME")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_getUnicodeVersion, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlChar_isalnum arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isalpha arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isbase arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isblank arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_iscntrl arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isdefined arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isdigit arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isgraph arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isIDIgnorable arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isIDPart arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isIDStart arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isISOControl arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isJavaIDPart arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isJavaIDStart arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isJavaSpaceChar arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_islower arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isMirrored arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isprint arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_ispunct arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isspace arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_istitle arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isUAlphabetic arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isULowercase arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isupper arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isUUppercase arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isUWhiteSpace arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isWhitespace arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_isxdigit arginfo_class_IntlChar_charAge

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_ord, 0, 0, 1)
	ZEND_ARG_TYPE_MASK(0, character, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlChar_tolower arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_totitle arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_toupper arginfo_class_IntlChar_charAge


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
