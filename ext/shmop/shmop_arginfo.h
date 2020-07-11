/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: e451ccfbe66fc2b6fc0dae6e7e5710ededaf7b0c */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_shmop_open, 0, 4, Shmop, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_shmop_read, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, shmid, Shmop, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shmop_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, shmid, Shmop, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shmop_size, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, shmid, Shmop, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_shmop_write, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, shmid, Shmop, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shmop_delete, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, shmid, Shmop, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(shmop_open);
ZEND_FUNCTION(shmop_read);
ZEND_FUNCTION(shmop_close);
ZEND_FUNCTION(shmop_size);
ZEND_FUNCTION(shmop_write);
ZEND_FUNCTION(shmop_delete);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(shmop_open, arginfo_shmop_open)
	ZEND_FE(shmop_read, arginfo_shmop_read)
	ZEND_DEP_FE(shmop_close, arginfo_shmop_close)
	ZEND_FE(shmop_size, arginfo_shmop_size)
	ZEND_FE(shmop_write, arginfo_shmop_write)
	ZEND_FE(shmop_delete, arginfo_shmop_delete)
	ZEND_FE_END
};


static const zend_function_entry class_Shmop_methods[] = {
	ZEND_FE_END
};
