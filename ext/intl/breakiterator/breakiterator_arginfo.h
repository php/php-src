/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 6a121ed9817667820f05677a772781d6b788796b */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlBreakIterator_createCharacterInstance, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlBreakIterator_createCodePointInstance, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlBreakIterator_createLineInstance arginfo_class_IntlBreakIterator_createCharacterInstance

#define arginfo_class_IntlBreakIterator_createSentenceInstance arginfo_class_IntlBreakIterator_createCharacterInstance

#define arginfo_class_IntlBreakIterator_createTitleInstance arginfo_class_IntlBreakIterator_createCharacterInstance

#define arginfo_class_IntlBreakIterator_createWordInstance arginfo_class_IntlBreakIterator_createCharacterInstance

#define arginfo_class_IntlBreakIterator___construct arginfo_class_IntlBreakIterator_createCodePointInstance

#define arginfo_class_IntlBreakIterator_current arginfo_class_IntlBreakIterator_createCodePointInstance

#define arginfo_class_IntlBreakIterator_first arginfo_class_IntlBreakIterator_createCodePointInstance

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlBreakIterator_following, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlBreakIterator_getErrorCode arginfo_class_IntlBreakIterator_createCodePointInstance

#define arginfo_class_IntlBreakIterator_getErrorMessage arginfo_class_IntlBreakIterator_createCodePointInstance

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlBreakIterator_getLocale, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlBreakIterator_getPartsIterator, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_STRING, 0, "IntlPartsIterator::KEY_SEQUENTIAL")
ZEND_END_ARG_INFO()

#define arginfo_class_IntlBreakIterator_getText arginfo_class_IntlBreakIterator_createCodePointInstance

#define arginfo_class_IntlBreakIterator_isBoundary arginfo_class_IntlBreakIterator_following

#define arginfo_class_IntlBreakIterator_last arginfo_class_IntlBreakIterator_createCodePointInstance

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlBreakIterator_next, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_IntlBreakIterator_preceding arginfo_class_IntlBreakIterator_following

#define arginfo_class_IntlBreakIterator_previous arginfo_class_IntlBreakIterator_createCodePointInstance

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlBreakIterator_setText, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, text, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_IntlBreakIterator_getIterator, 0, 0, Iterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlRuleBasedBreakIterator___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, rules, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, compiled, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_class_IntlRuleBasedBreakIterator_getBinaryRules arginfo_class_IntlBreakIterator_createCodePointInstance

#define arginfo_class_IntlRuleBasedBreakIterator_getRules arginfo_class_IntlBreakIterator_createCodePointInstance

#define arginfo_class_IntlRuleBasedBreakIterator_getRuleStatus arginfo_class_IntlBreakIterator_createCodePointInstance

#define arginfo_class_IntlRuleBasedBreakIterator_getRuleStatusVec arginfo_class_IntlBreakIterator_createCodePointInstance

#define arginfo_class_IntlPartsIterator_getBreakIterator arginfo_class_IntlBreakIterator_createCodePointInstance

#define arginfo_class_IntlCodePointBreakIterator_getLastCodePoint arginfo_class_IntlBreakIterator_createCodePointInstance


ZEND_METHOD(IntlBreakIterator, createCharacterInstance);
ZEND_METHOD(IntlBreakIterator, createCodePointInstance);
ZEND_METHOD(IntlBreakIterator, createLineInstance);
ZEND_METHOD(IntlBreakIterator, createSentenceInstance);
ZEND_METHOD(IntlBreakIterator, createTitleInstance);
ZEND_METHOD(IntlBreakIterator, createWordInstance);
ZEND_METHOD(IntlBreakIterator, __construct);
ZEND_METHOD(IntlBreakIterator, current);
ZEND_METHOD(IntlBreakIterator, first);
ZEND_METHOD(IntlBreakIterator, following);
ZEND_METHOD(IntlBreakIterator, getErrorCode);
ZEND_METHOD(IntlBreakIterator, getErrorMessage);
ZEND_METHOD(IntlBreakIterator, getLocale);
ZEND_METHOD(IntlBreakIterator, getPartsIterator);
ZEND_METHOD(IntlBreakIterator, getText);
ZEND_METHOD(IntlBreakIterator, isBoundary);
ZEND_METHOD(IntlBreakIterator, last);
ZEND_METHOD(IntlBreakIterator, next);
ZEND_METHOD(IntlBreakIterator, preceding);
ZEND_METHOD(IntlBreakIterator, previous);
ZEND_METHOD(IntlBreakIterator, setText);
ZEND_METHOD(IntlBreakIterator, getIterator);
ZEND_METHOD(IntlRuleBasedBreakIterator, __construct);
ZEND_METHOD(IntlRuleBasedBreakIterator, getBinaryRules);
ZEND_METHOD(IntlRuleBasedBreakIterator, getRules);
ZEND_METHOD(IntlRuleBasedBreakIterator, getRuleStatus);
ZEND_METHOD(IntlRuleBasedBreakIterator, getRuleStatusVec);
ZEND_METHOD(IntlPartsIterator, getBreakIterator);
ZEND_METHOD(IntlCodePointBreakIterator, getLastCodePoint);


static const zend_function_entry class_IntlBreakIterator_methods[] = {
	ZEND_ME(IntlBreakIterator, createCharacterInstance, arginfo_class_IntlBreakIterator_createCharacterInstance, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlBreakIterator, createCodePointInstance, arginfo_class_IntlBreakIterator_createCodePointInstance, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlBreakIterator, createLineInstance, arginfo_class_IntlBreakIterator_createLineInstance, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlBreakIterator, createSentenceInstance, arginfo_class_IntlBreakIterator_createSentenceInstance, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlBreakIterator, createTitleInstance, arginfo_class_IntlBreakIterator_createTitleInstance, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlBreakIterator, createWordInstance, arginfo_class_IntlBreakIterator_createWordInstance, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlBreakIterator, __construct, arginfo_class_IntlBreakIterator___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(IntlBreakIterator, current, arginfo_class_IntlBreakIterator_current, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlBreakIterator, first, arginfo_class_IntlBreakIterator_first, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlBreakIterator, following, arginfo_class_IntlBreakIterator_following, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlBreakIterator, getErrorCode, arginfo_class_IntlBreakIterator_getErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlBreakIterator, getErrorMessage, arginfo_class_IntlBreakIterator_getErrorMessage, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlBreakIterator, getLocale, arginfo_class_IntlBreakIterator_getLocale, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlBreakIterator, getPartsIterator, arginfo_class_IntlBreakIterator_getPartsIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlBreakIterator, getText, arginfo_class_IntlBreakIterator_getText, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlBreakIterator, isBoundary, arginfo_class_IntlBreakIterator_isBoundary, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlBreakIterator, last, arginfo_class_IntlBreakIterator_last, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlBreakIterator, next, arginfo_class_IntlBreakIterator_next, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlBreakIterator, preceding, arginfo_class_IntlBreakIterator_preceding, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlBreakIterator, previous, arginfo_class_IntlBreakIterator_previous, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlBreakIterator, setText, arginfo_class_IntlBreakIterator_setText, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlBreakIterator, getIterator, arginfo_class_IntlBreakIterator_getIterator, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_IntlRuleBasedBreakIterator_methods[] = {
	ZEND_ME(IntlRuleBasedBreakIterator, __construct, arginfo_class_IntlRuleBasedBreakIterator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlRuleBasedBreakIterator, getBinaryRules, arginfo_class_IntlRuleBasedBreakIterator_getBinaryRules, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlRuleBasedBreakIterator, getRules, arginfo_class_IntlRuleBasedBreakIterator_getRules, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlRuleBasedBreakIterator, getRuleStatus, arginfo_class_IntlRuleBasedBreakIterator_getRuleStatus, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlRuleBasedBreakIterator, getRuleStatusVec, arginfo_class_IntlRuleBasedBreakIterator_getRuleStatusVec, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_IntlPartsIterator_methods[] = {
	ZEND_ME(IntlPartsIterator, getBreakIterator, arginfo_class_IntlPartsIterator_getBreakIterator, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_IntlCodePointBreakIterator_methods[] = {
	ZEND_ME(IntlCodePointBreakIterator, getLastCodePoint, arginfo_class_IntlCodePointBreakIterator_getLastCodePoint, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
