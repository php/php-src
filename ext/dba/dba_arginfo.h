/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_dba_popen, 0, 0, 2)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, handlername)
	ZEND_ARG_VARIADIC_INFO(0, handler_parameters)
ZEND_END_ARG_INFO()

#define arginfo_dba_open arginfo_dba_popen

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dba_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dba_exists, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_dba_fetch, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, skip)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_dba_key_split, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_dba_firstkey, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

#define arginfo_dba_nextkey arginfo_dba_firstkey

#define arginfo_dba_delete arginfo_dba_exists

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dba_insert, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

#define arginfo_dba_replace arginfo_dba_insert

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dba_optimize, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

#define arginfo_dba_sync arginfo_dba_optimize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dba_handlers, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, full_info, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dba_list, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()
