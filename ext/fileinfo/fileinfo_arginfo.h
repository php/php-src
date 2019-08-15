/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_finfo_open, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_finfo_close, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, finfo)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_finfo_set_flags, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, finfo)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_finfo_file, 0, 2, IS_STRING, 0)
	ZEND_ARG_INFO(0, finfo)
	ZEND_ARG_TYPE_INFO(0, file_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_finfo_buffer, 0, 1, IS_STRING, 0)
	ZEND_ARG_INFO(0, finfo)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mime_content_type, 0, 1, IS_STRING, 0)
	ZEND_ARG_INFO(0, file)
ZEND_END_ARG_INFO()
