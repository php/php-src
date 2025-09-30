/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f9810e7b2296f1c93ec6b55bd6a3472d955c2717 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_StreamException_getParam, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_StreamException_getWrapperName, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

static ZEND_METHOD(StreamException, getParam);
static ZEND_METHOD(StreamException, getWrapperName);

static const zend_function_entry class_StreamException_methods[] = {
	ZEND_ME(StreamException, getParam, arginfo_class_StreamException_getParam, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamException, getWrapperName, arginfo_class_StreamException_getWrapperName, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_StreamException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "StreamException", class_StreamException_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, 0);

	zval property_wrapperName_default_value;
	ZVAL_UNDEF(&property_wrapperName_default_value);
	zend_string *property_wrapperName_name = zend_string_init("wrapperName", sizeof("wrapperName") - 1, 1);
	zend_declare_typed_property(class_entry, property_wrapperName_name, &property_wrapperName_default_value, ZEND_ACC_PROTECTED, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_wrapperName_name);

	zval property_param_default_value;
	ZVAL_NULL(&property_param_default_value);
	zend_string *property_param_name = zend_string_init("param", sizeof("param") - 1, 1);
	zend_declare_typed_property(class_entry, property_param_name, &property_param_default_value, ZEND_ACC_PROTECTED, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_param_name);

	return class_entry;
}
