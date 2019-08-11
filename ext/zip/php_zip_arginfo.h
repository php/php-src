/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_zip_open, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zip_close, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, zip)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zip_read, 0, 0, 1)
	ZEND_ARG_INFO(0, zip)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zip_entry_open, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, zip_dp)
	ZEND_ARG_INFO(0, zip_entry)
	ZEND_ARG_TYPE_INFO(0, mode, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zip_entry_close, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, zip_ent)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zip_entry_read, 0, 0, 1)
	ZEND_ARG_INFO(0, zip_entry)
	ZEND_ARG_TYPE_INFO(0, len, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zip_entry_name, 0, 0, 1)
	ZEND_ARG_INFO(0, zip_entry)
ZEND_END_ARG_INFO()

#define arginfo_zip_entry_compressedsize arginfo_zip_entry_name

#define arginfo_zip_entry_filesize arginfo_zip_entry_name

#define arginfo_zip_entry_compressionmethod arginfo_zip_entry_name
