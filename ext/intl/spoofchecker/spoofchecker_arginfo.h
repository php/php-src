/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Spoofchecker___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Spoofchecker_isSuspicious, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, text, IS_STRING, 0)
	ZEND_ARG_INFO(1, error)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Spoofchecker_areConfusable, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, s1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, s2, IS_STRING, 0)
	ZEND_ARG_INFO(1, error)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Spoofchecker_setAllowedLocales, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, locale_list, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Spoofchecker_setChecks, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, checks, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if U_ICU_VERSION_MAJOR_NUM >= 58
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Spoofchecker_setRestrictionLevel, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, level, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif
