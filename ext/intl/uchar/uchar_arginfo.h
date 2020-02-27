/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_hasBinaryProperty, 0, 0, 2)
	ZEND_ARG_INFO(0, codepoint)
	ZEND_ARG_TYPE_INFO(0, property, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_charAge, 0, 0, 1)
	ZEND_ARG_INFO(0, codepoint)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlChar_charDigitValue arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_charDirection arginfo_class_IntlChar_charAge

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_charFromName, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, characterName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, nameChoice, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlChar_charMirror arginfo_class_IntlChar_charAge

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_charName, 0, 0, 1)
	ZEND_ARG_INFO(0, codepoint)
	ZEND_ARG_TYPE_INFO(0, nameChoice, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlChar_charType arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_chr arginfo_class_IntlChar_charAge

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_digit, 0, 0, 1)
	ZEND_ARG_INFO(0, codepoint)
	ZEND_ARG_TYPE_INFO(0, radix, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_enumCharNames, 0, 0, 3)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, limit)
	ZEND_ARG_INFO(0, callback)
	ZEND_ARG_TYPE_INFO(0, nameChoice, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_enumCharTypes, 0, 0, 1)
	ZEND_ARG_INFO(0, callable)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_foldCase, 0, 0, 1)
	ZEND_ARG_INFO(0, codepoint)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_forDigit, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, digit, IS_LONG, 0)
	ZEND_ARG_INFO(0, radix)
ZEND_END_ARG_INFO()

#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_getBidiPairedBracket, 0, 0, 1)
	ZEND_ARG_INFO(0, codepoint)
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
	ZEND_ARG_TYPE_INFO(0, nameChoice, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_getPropertyValueEnum, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, property, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlChar_getPropertyValueName, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, property, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
	ZEND_ARG_INFO(0, nameChoice)
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
	ZEND_ARG_INFO(0, character)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlChar_tolower arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_totitle arginfo_class_IntlChar_charAge

#define arginfo_class_IntlChar_toupper arginfo_class_IntlChar_charAge
