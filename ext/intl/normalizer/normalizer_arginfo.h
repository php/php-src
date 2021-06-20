/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 651065249a99f9ee50126d58223b8f4527e3fc09 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Normalizer_normalize, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, form, IS_LONG, 0, "Normalizer::FORM_C")
ZEND_END_ARG_INFO()

#define arginfo_class_Normalizer_isNormalized arginfo_class_Normalizer_normalize

#if U_ICU_VERSION_MAJOR_NUM >= 56
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Normalizer_getRawDecomposition, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, form, IS_LONG, 0, "Normalizer::FORM_C")
ZEND_END_ARG_INFO()
#endif


ZEND_FUNCTION(normalizer_normalize);
ZEND_FUNCTION(normalizer_is_normalized);
#if U_ICU_VERSION_MAJOR_NUM >= 56
ZEND_FUNCTION(normalizer_get_raw_decomposition);
#endif


static const zend_function_entry class_Normalizer_methods[] = {
	ZEND_ME_MAPPING(normalize, normalizer_normalize, arginfo_class_Normalizer_normalize, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(isNormalized, normalizer_is_normalized, arginfo_class_Normalizer_isNormalized, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
#if U_ICU_VERSION_MAJOR_NUM >= 56
	ZEND_ME_MAPPING(getRawDecomposition, normalizer_get_raw_decomposition, arginfo_class_Normalizer_getRawDecomposition, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
#endif
	ZEND_FE_END
};

static zend_class_entry *register_class_Normalizer(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Normalizer", class_Normalizer_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}
