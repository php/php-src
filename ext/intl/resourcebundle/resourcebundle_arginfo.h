/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 474a5f2d63f94aaf68c92839bb343d74179111a7 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ResourceBundle___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, bundle, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fallback, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

#define arginfo_class_ResourceBundle_create arginfo_class_ResourceBundle___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ResourceBundle_get, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fallback, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ResourceBundle_count, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ResourceBundle_getLocales, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, bundle, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ResourceBundle_getErrorCode arginfo_class_ResourceBundle_count

#define arginfo_class_ResourceBundle_getErrorMessage arginfo_class_ResourceBundle_count

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
	ZEND_ME(ResourceBundle, __construct, arginfo_class_ResourceBundle___construct, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(create, resourcebundle_create, arginfo_class_ResourceBundle_create, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(get, resourcebundle_get, arginfo_class_ResourceBundle_get, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(count, resourcebundle_count, arginfo_class_ResourceBundle_count, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getLocales, resourcebundle_locales, arginfo_class_ResourceBundle_getLocales, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getErrorCode, resourcebundle_get_error_code, arginfo_class_ResourceBundle_getErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getErrorMessage, resourcebundle_get_error_message, arginfo_class_ResourceBundle_getErrorMessage, ZEND_ACC_PUBLIC)
	ZEND_ME(ResourceBundle, getIterator, arginfo_class_ResourceBundle_getIterator, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

zend_class_entry *register_class_ResourceBundle(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Countable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ResourceBundle", class_ResourceBundle_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 2, class_entry_IteratorAggregate, class_entry_Countable);

	return class_entry;
}

