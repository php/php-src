/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 1a02eaf9da45edb40720620e3beef43fd19dd520 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_dba_popen, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, handler, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, permission, IS_LONG, 0, "0644")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, map_size, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_dba_open arginfo_dba_popen

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dba_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, dba)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dba_exists, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(0, key, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_INFO(0, dba)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_dba_fetch, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, key, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_INFO(0, dba)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, skip, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_dba_key_split, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, key, MAY_BE_STRING|MAY_BE_FALSE|MAY_BE_NULL, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_dba_firstkey, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, dba)
ZEND_END_ARG_INFO()

#define arginfo_dba_nextkey arginfo_dba_firstkey

#define arginfo_dba_delete arginfo_dba_exists

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dba_insert, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(0, key, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_INFO(0, dba)
ZEND_END_ARG_INFO()

#define arginfo_dba_replace arginfo_dba_insert

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dba_optimize, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, dba)
ZEND_END_ARG_INFO()

#define arginfo_dba_sync arginfo_dba_optimize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dba_handlers, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, full_info, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dba_list, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(dba_popen);
ZEND_FUNCTION(dba_open);
ZEND_FUNCTION(dba_close);
ZEND_FUNCTION(dba_exists);
ZEND_FUNCTION(dba_fetch);
ZEND_FUNCTION(dba_key_split);
ZEND_FUNCTION(dba_firstkey);
ZEND_FUNCTION(dba_nextkey);
ZEND_FUNCTION(dba_delete);
ZEND_FUNCTION(dba_insert);
ZEND_FUNCTION(dba_replace);
ZEND_FUNCTION(dba_optimize);
ZEND_FUNCTION(dba_sync);
ZEND_FUNCTION(dba_handlers);
ZEND_FUNCTION(dba_list);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY("dba_popen", zif_dba_popen, arginfo_dba_popen, 0, NULL)
	ZEND_RAW_FENTRY("dba_open", zif_dba_open, arginfo_dba_open, 0, NULL)
	ZEND_RAW_FENTRY("dba_close", zif_dba_close, arginfo_dba_close, 0, NULL)
	ZEND_RAW_FENTRY("dba_exists", zif_dba_exists, arginfo_dba_exists, 0, NULL)
	ZEND_RAW_FENTRY("dba_fetch", zif_dba_fetch, arginfo_dba_fetch, 0, NULL)
	ZEND_RAW_FENTRY("dba_key_split", zif_dba_key_split, arginfo_dba_key_split, 0, NULL)
	ZEND_RAW_FENTRY("dba_firstkey", zif_dba_firstkey, arginfo_dba_firstkey, 0, NULL)
	ZEND_RAW_FENTRY("dba_nextkey", zif_dba_nextkey, arginfo_dba_nextkey, 0, NULL)
	ZEND_RAW_FENTRY("dba_delete", zif_dba_delete, arginfo_dba_delete, 0, NULL)
	ZEND_RAW_FENTRY("dba_insert", zif_dba_insert, arginfo_dba_insert, 0, NULL)
	ZEND_RAW_FENTRY("dba_replace", zif_dba_replace, arginfo_dba_replace, 0, NULL)
	ZEND_RAW_FENTRY("dba_optimize", zif_dba_optimize, arginfo_dba_optimize, 0, NULL)
	ZEND_RAW_FENTRY("dba_sync", zif_dba_sync, arginfo_dba_sync, 0, NULL)
	ZEND_RAW_FENTRY("dba_handlers", zif_dba_handlers, arginfo_dba_handlers, 0, NULL)
	ZEND_RAW_FENTRY("dba_list", zif_dba_list, arginfo_dba_list, 0, NULL)
	ZEND_FE_END
};

static void register_dba_symbols(int module_number)
{
#if defined(DBA_LMDB)
	REGISTER_LONG_CONSTANT("DBA_LMDB_USE_SUB_DIR", 0, CONST_PERSISTENT);
#endif
#if defined(DBA_LMDB)
	REGISTER_LONG_CONSTANT("DBA_LMDB_NO_SUB_DIR", MDB_NOSUBDIR, CONST_PERSISTENT);
#endif
}
