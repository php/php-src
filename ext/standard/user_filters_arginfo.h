/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c7ae14efaeb0e8f5fdd6ddf92574a35bd5c860a1 */

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_php_user_filter_filter, 0, 4, IS_LONG, 0)
	ZEND_ARG_INFO(0, in)
	ZEND_ARG_INFO(0, out)
	ZEND_ARG_INFO(1, consumed)
	ZEND_ARG_TYPE_INFO(0, closing, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_php_user_filter_onCreate, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_php_user_filter_onClose, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(php_user_filter, filter);
ZEND_METHOD(php_user_filter, onCreate);
ZEND_METHOD(php_user_filter, onClose);


static const zend_function_entry class_php_user_filter_methods[] = {
	ZEND_ME(php_user_filter, filter, arginfo_class_php_user_filter_filter, ZEND_ACC_PUBLIC)
	ZEND_ME(php_user_filter, onCreate, arginfo_class_php_user_filter_onCreate, ZEND_ACC_PUBLIC)
	ZEND_ME(php_user_filter, onClose, arginfo_class_php_user_filter_onClose, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_php_user_filter(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "php_user_filter", class_php_user_filter_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	zval property_filtername_default_value;
	ZVAL_EMPTY_STRING(&property_filtername_default_value);
	zend_string *property_filtername_name = zend_string_init("filtername", sizeof("filtername") - 1, 1);
	zend_declare_typed_property(class_entry, property_filtername_name, &property_filtername_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_filtername_name);

	zval property_params_default_value;
	ZVAL_EMPTY_STRING(&property_params_default_value);
	zend_string *property_params_name = zend_string_init("params", sizeof("params") - 1, 1);
	zend_declare_typed_property(class_entry, property_params_name, &property_params_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ANY));
	zend_string_release(property_params_name);

	zval property_stream_default_value;
	ZVAL_NULL(&property_stream_default_value);
	zend_string *property_stream_name = zend_string_init("stream", sizeof("stream") - 1, 1);
	zend_declare_property_ex(class_entry, property_stream_name, &property_stream_default_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(property_stream_name);

	return class_entry;
}
