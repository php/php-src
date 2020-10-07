/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 4cdab686b39e7c4bb03bd5517a637ff25a38b04a */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CURLFile___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mime_type, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, posted_filename, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CURLFile_getFilename, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CURLFile_getMimeType arginfo_class_CURLFile_getFilename

#define arginfo_class_CURLFile_getPostFilename arginfo_class_CURLFile_getFilename

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CURLFile_setMimeType, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, mime_type, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CURLFile_setPostFilename, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, posted_filename, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(CURLFile, __construct);
ZEND_METHOD(CURLFile, getFilename);
ZEND_METHOD(CURLFile, getMimeType);
ZEND_METHOD(CURLFile, getPostFilename);
ZEND_METHOD(CURLFile, setMimeType);
ZEND_METHOD(CURLFile, setPostFilename);


static const zend_function_entry class_CURLFile_methods[] = {
	ZEND_ME(CURLFile, __construct, arginfo_class_CURLFile___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(CURLFile, getFilename, arginfo_class_CURLFile_getFilename, ZEND_ACC_PUBLIC)
	ZEND_ME(CURLFile, getMimeType, arginfo_class_CURLFile_getMimeType, ZEND_ACC_PUBLIC)
	ZEND_ME(CURLFile, getPostFilename, arginfo_class_CURLFile_getPostFilename, ZEND_ACC_PUBLIC)
	ZEND_ME(CURLFile, setMimeType, arginfo_class_CURLFile_setMimeType, ZEND_ACC_PUBLIC)
	ZEND_ME(CURLFile, setPostFilename, arginfo_class_CURLFile_setPostFilename, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

zend_class_entry *register_class_CURLFile()
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "CURLFile", class_CURLFile_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	zend_declare_property_string(class_entry, "name", sizeof("name") - 1, "", ZEND_ACC_PUBLIC);

	zend_declare_property_string(class_entry, "mime", sizeof("mime") - 1, "", ZEND_ACC_PUBLIC);

	zend_declare_property_string(class_entry, "postname", sizeof("postname") - 1, "", ZEND_ACC_PUBLIC);

	return class_entry;
}

