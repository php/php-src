/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 0d09bd2f3b0a155cef25ca343319ecf470424d71 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CURLFile___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mime_type, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, posted_filename, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_CURLFile_getFilename, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CURLFile_getMimeType arginfo_class_CURLFile_getFilename

#define arginfo_class_CURLFile_getPostFilename arginfo_class_CURLFile_getFilename

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_CURLFile_setMimeType, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, mime_type, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_CURLFile_setPostFilename, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, posted_filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CURLStringFile___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, postname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mime, IS_STRING, 0, "\"application/octet-stream\"")
ZEND_END_ARG_INFO()


ZEND_METHOD(CURLFile, __construct);
ZEND_METHOD(CURLFile, getFilename);
ZEND_METHOD(CURLFile, getMimeType);
ZEND_METHOD(CURLFile, getPostFilename);
ZEND_METHOD(CURLFile, setMimeType);
ZEND_METHOD(CURLFile, setPostFilename);
ZEND_METHOD(CURLStringFile, __construct);


static const zend_function_entry class_CURLFile_methods[] = {
	ZEND_ME(CURLFile, __construct, arginfo_class_CURLFile___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(CURLFile, getFilename, arginfo_class_CURLFile_getFilename, ZEND_ACC_PUBLIC)
	ZEND_ME(CURLFile, getMimeType, arginfo_class_CURLFile_getMimeType, ZEND_ACC_PUBLIC)
	ZEND_ME(CURLFile, getPostFilename, arginfo_class_CURLFile_getPostFilename, ZEND_ACC_PUBLIC)
	ZEND_ME(CURLFile, setMimeType, arginfo_class_CURLFile_setMimeType, ZEND_ACC_PUBLIC)
	ZEND_ME(CURLFile, setPostFilename, arginfo_class_CURLFile_setPostFilename, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_CURLStringFile_methods[] = {
	ZEND_ME(CURLStringFile, __construct, arginfo_class_CURLStringFile___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_CURLFile(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "CURLFile", class_CURLFile_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	zval property_name_default_value;
	ZVAL_EMPTY_STRING(&property_name_default_value);
	zend_string *property_name_name = zend_string_init("name", sizeof("name") - 1, 1);
	zend_declare_typed_property(class_entry, property_name_name, &property_name_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_name_name);

	zval property_mime_default_value;
	ZVAL_EMPTY_STRING(&property_mime_default_value);
	zend_string *property_mime_name = zend_string_init("mime", sizeof("mime") - 1, 1);
	zend_declare_typed_property(class_entry, property_mime_name, &property_mime_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_mime_name);

	zval property_postname_default_value;
	ZVAL_EMPTY_STRING(&property_postname_default_value);
	zend_string *property_postname_name = zend_string_init("postname", sizeof("postname") - 1, 1);
	zend_declare_typed_property(class_entry, property_postname_name, &property_postname_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_postname_name);

	return class_entry;
}

static zend_class_entry *register_class_CURLStringFile(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "CURLStringFile", class_CURLStringFile_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	zval property_data_default_value;
	ZVAL_UNDEF(&property_data_default_value);
	zend_string *property_data_name = zend_string_init("data", sizeof("data") - 1, 1);
	zend_declare_typed_property(class_entry, property_data_name, &property_data_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_data_name);

	zval property_postname_default_value;
	ZVAL_UNDEF(&property_postname_default_value);
	zend_string *property_postname_name = zend_string_init("postname", sizeof("postname") - 1, 1);
	zend_declare_typed_property(class_entry, property_postname_name, &property_postname_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_postname_name);

	zval property_mime_default_value;
	ZVAL_UNDEF(&property_mime_default_value);
	zend_string *property_mime_name = zend_string_init("mime", sizeof("mime") - 1, 1);
	zend_declare_typed_property(class_entry, property_mime_name, &property_mime_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_mime_name);

	return class_entry;
}
