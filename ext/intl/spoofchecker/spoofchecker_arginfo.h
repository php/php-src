/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 10c330c8e06dbe21833f9c556b005fde2ea0ab42 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Spoofchecker___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Spoofchecker_isSuspicious, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, errorCode, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Spoofchecker_areConfusable, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, string1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string2, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, errorCode, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Spoofchecker_setAllowedLocales, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, locales, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Spoofchecker_setChecks, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, checks, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if U_ICU_VERSION_MAJOR_NUM >= 58
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Spoofchecker_setRestrictionLevel, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, level, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif


ZEND_METHOD(Spoofchecker, __construct);
ZEND_METHOD(Spoofchecker, isSuspicious);
ZEND_METHOD(Spoofchecker, areConfusable);
ZEND_METHOD(Spoofchecker, setAllowedLocales);
ZEND_METHOD(Spoofchecker, setChecks);
#if U_ICU_VERSION_MAJOR_NUM >= 58
ZEND_METHOD(Spoofchecker, setRestrictionLevel);
#endif


static const zend_function_entry class_Spoofchecker_methods[] = {
	ZEND_ME(Spoofchecker, __construct, arginfo_class_Spoofchecker___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Spoofchecker, isSuspicious, arginfo_class_Spoofchecker_isSuspicious, ZEND_ACC_PUBLIC)
	ZEND_ME(Spoofchecker, areConfusable, arginfo_class_Spoofchecker_areConfusable, ZEND_ACC_PUBLIC)
	ZEND_ME(Spoofchecker, setAllowedLocales, arginfo_class_Spoofchecker_setAllowedLocales, ZEND_ACC_PUBLIC)
	ZEND_ME(Spoofchecker, setChecks, arginfo_class_Spoofchecker_setChecks, ZEND_ACC_PUBLIC)
#if U_ICU_VERSION_MAJOR_NUM >= 58
	ZEND_ME(Spoofchecker, setRestrictionLevel, arginfo_class_Spoofchecker_setRestrictionLevel, ZEND_ACC_PUBLIC)
#endif
	ZEND_FE_END
};
