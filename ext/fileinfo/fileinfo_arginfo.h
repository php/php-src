/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 648085986695281bd5a8fb536d5ec6c2a9f8c6e8 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_finfo_open, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "FILEINFO_NONE")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, magic_database, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_finfo_close, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, finfo)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_finfo_set_flags, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, finfo)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_finfo_file, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, finfo)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "FILEINFO_NONE")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_finfo_buffer, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, finfo)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "FILEINFO_NONE")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mime_content_type, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

#define arginfo_class_finfo___construct arginfo_finfo_open

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_finfo_file, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "FILEINFO_NONE")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_finfo_buffer, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "FILEINFO_NONE")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_finfo_set_flags, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(finfo_open);
ZEND_FUNCTION(finfo_close);
ZEND_FUNCTION(finfo_set_flags);
ZEND_FUNCTION(finfo_file);
ZEND_FUNCTION(finfo_buffer);
ZEND_FUNCTION(mime_content_type);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(finfo_open, arginfo_finfo_open)
	ZEND_FE(finfo_close, arginfo_finfo_close)
	ZEND_FE(finfo_set_flags, arginfo_finfo_set_flags)
	ZEND_FE(finfo_file, arginfo_finfo_file)
	ZEND_FE(finfo_buffer, arginfo_finfo_buffer)
	ZEND_FE(mime_content_type, arginfo_mime_content_type)
	ZEND_FE_END
};


static const zend_function_entry class_finfo_methods[] = {
	ZEND_ME_MAPPING(__construct, finfo_open, arginfo_class_finfo___construct, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(file, finfo_file, arginfo_class_finfo_file, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(buffer, finfo_buffer, arginfo_class_finfo_buffer, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(set_flags, finfo_set_flags, arginfo_class_finfo_set_flags, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
