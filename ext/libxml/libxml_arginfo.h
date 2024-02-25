/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 08e4e3f10ba89430292831f50c4760a362593282 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_libxml_set_streams_context, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_libxml_use_internal_errors, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, use_errors, _IS_BOOL, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_libxml_get_last_error, 0, 0, LibXMLError, MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_libxml_get_errors, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_libxml_clear_errors, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_libxml_disable_entity_loader, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, disable, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_libxml_set_external_entity_loader, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, resolver_function, IS_CALLABLE, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_libxml_get_external_entity_loader, 0, 0, IS_CALLABLE, 1)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(libxml_set_streams_context);
ZEND_FUNCTION(libxml_use_internal_errors);
ZEND_FUNCTION(libxml_get_last_error);
ZEND_FUNCTION(libxml_get_errors);
ZEND_FUNCTION(libxml_clear_errors);
ZEND_FUNCTION(libxml_disable_entity_loader);
ZEND_FUNCTION(libxml_set_external_entity_loader);
ZEND_FUNCTION(libxml_get_external_entity_loader);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(libxml_set_streams_context, arginfo_libxml_set_streams_context)
	ZEND_FE(libxml_use_internal_errors, arginfo_libxml_use_internal_errors)
	ZEND_FE(libxml_get_last_error, arginfo_libxml_get_last_error)
	ZEND_FE(libxml_get_errors, arginfo_libxml_get_errors)
	ZEND_FE(libxml_clear_errors, arginfo_libxml_clear_errors)
	ZEND_DEP_FE(libxml_disable_entity_loader, arginfo_libxml_disable_entity_loader)
	ZEND_FE(libxml_set_external_entity_loader, arginfo_libxml_set_external_entity_loader)
	ZEND_FE(libxml_get_external_entity_loader, arginfo_libxml_get_external_entity_loader)
	ZEND_FE_END
};

static const zend_function_entry class_LibXMLError_methods[] = {
	ZEND_FE_END
};

static void register_libxml_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("LIBXML_VERSION", LIBXML_VERSION, CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("LIBXML_DOTTED_VERSION", LIBXML_DOTTED_VERSION, CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("LIBXML_LOADED_VERSION", PHP_LIBXML_LOADED_VERSION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_RECOVER", XML_PARSE_RECOVER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOENT", XML_PARSE_NOENT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_DTDLOAD", XML_PARSE_DTDLOAD, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_DTDATTR", XML_PARSE_DTDATTR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_DTDVALID", XML_PARSE_DTDVALID, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOERROR", XML_PARSE_NOERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOWARNING", XML_PARSE_NOWARNING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOBLANKS", XML_PARSE_NOBLANKS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_XINCLUDE", XML_PARSE_XINCLUDE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NSCLEAN", XML_PARSE_NSCLEAN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOCDATA", XML_PARSE_NOCDATA, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NONET", XML_PARSE_NONET, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_PEDANTIC", XML_PARSE_PEDANTIC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_COMPACT", XML_PARSE_COMPACT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOXMLDECL", XML_SAVE_NO_DECL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_PARSEHUGE", XML_PARSE_HUGE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_BIGLINES", XML_PARSE_BIG_LINES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_NOEMPTYTAG", LIBXML_SAVE_NOEMPTYTAG, CONST_PERSISTENT);
#if defined(LIBXML_SCHEMAS_ENABLED)
	REGISTER_LONG_CONSTANT("LIBXML_SCHEMA_CREATE", XML_SCHEMA_VAL_VC_I_CREATE, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("LIBXML_HTML_NOIMPLIED", HTML_PARSE_NOIMPLIED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_HTML_NODEFDTD", HTML_PARSE_NODEFDTD, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_ERR_NONE", XML_ERR_NONE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_ERR_WARNING", XML_ERR_WARNING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_ERR_ERROR", XML_ERR_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LIBXML_ERR_FATAL", XML_ERR_FATAL, CONST_PERSISTENT);
}

static zend_class_entry *register_class_LibXMLError(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "LibXMLError", class_LibXMLError_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	zval property_level_default_value;
	ZVAL_UNDEF(&property_level_default_value);
	zend_string *property_level_name = zend_string_init("level", sizeof("level") - 1, 1);
	zend_declare_typed_property(class_entry, property_level_name, &property_level_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_level_name);

	zval property_code_default_value;
	ZVAL_UNDEF(&property_code_default_value);
	zend_string *property_code_name = zend_string_init("code", sizeof("code") - 1, 1);
	zend_declare_typed_property(class_entry, property_code_name, &property_code_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_code_name);

	zval property_column_default_value;
	ZVAL_UNDEF(&property_column_default_value);
	zend_string *property_column_name = zend_string_init("column", sizeof("column") - 1, 1);
	zend_declare_typed_property(class_entry, property_column_name, &property_column_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_column_name);

	zval property_message_default_value;
	ZVAL_UNDEF(&property_message_default_value);
	zend_string *property_message_name = zend_string_init("message", sizeof("message") - 1, 1);
	zend_declare_typed_property(class_entry, property_message_name, &property_message_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_message_name);

	zval property_file_default_value;
	ZVAL_UNDEF(&property_file_default_value);
	zend_string *property_file_name = zend_string_init("file", sizeof("file") - 1, 1);
	zend_declare_typed_property(class_entry, property_file_name, &property_file_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_file_name);

	zval property_line_default_value;
	ZVAL_UNDEF(&property_line_default_value);
	zend_string *property_line_name = zend_string_init("line", sizeof("line") - 1, 1);
	zend_declare_typed_property(class_entry, property_line_name, &property_line_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_line_name);

	return class_entry;
}
