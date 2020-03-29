/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Normalizer_normalize, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, input, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, form, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Normalizer_isNormalized arginfo_class_Normalizer_normalize

#if U_ICU_VERSION_MAJOR_NUM >= 56
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Normalizer_getRawDecomposition, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, input, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, form, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif
