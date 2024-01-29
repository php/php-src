/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 7816536650d8513ef6998233096b0bf6a29d7af4 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ResourceBundle___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, bundle, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fallback, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_ResourceBundle_create, 0, 2, ResourceBundle, 1)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, bundle, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fallback, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ResourceBundle_get, 0, 1, IS_MIXED, 0)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fallback, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ResourceBundle_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_ResourceBundle_getLocales, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, bundle, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ResourceBundle_getErrorCode arginfo_class_ResourceBundle_count

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ResourceBundle_getErrorMessage, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_ResourceBundle_getIterator, 0, 0, Iterator, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(ResourceBundle, __construct);
ZEND_FUNCTION(resourcebundle_create);
ZEND_FUNCTION(resourcebundle_get);
ZEND_FUNCTION(resourcebundle_count);
ZEND_FUNCTION(resourcebundle_locales);
ZEND_FUNCTION(resourcebundle_get_error_code);
ZEND_FUNCTION(resourcebundle_get_error_message);
ZEND_METHOD(ResourceBundle, getIterator);

static const zend_function_entry class_ResourceBundle_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_ResourceBundle___construct, arginfo_class_ResourceBundle___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("create", zif_resourcebundle_create, arginfo_class_ResourceBundle_create, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("get", zif_resourcebundle_get, arginfo_class_ResourceBundle_get, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("count", zif_resourcebundle_count, arginfo_class_ResourceBundle_count, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getLocales", zif_resourcebundle_locales, arginfo_class_ResourceBundle_getLocales, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL)
	ZEND_RAW_FENTRY("getErrorCode", zif_resourcebundle_get_error_code, arginfo_class_ResourceBundle_getErrorCode, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getErrorMessage", zif_resourcebundle_get_error_message, arginfo_class_ResourceBundle_getErrorMessage, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getIterator", zim_ResourceBundle_getIterator, arginfo_class_ResourceBundle_getIterator, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static zend_class_entry *register_class_ResourceBundle(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Countable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ResourceBundle", class_ResourceBundle_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
	zend_class_implements(class_entry, 2, class_entry_IteratorAggregate, class_entry_Countable);

	return class_entry;
}
