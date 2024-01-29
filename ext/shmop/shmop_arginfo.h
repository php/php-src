/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 6055f4edb68a7caed517dbb80f4d5265865dd91d */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_shmop_open, 0, 4, Shmop, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, permissions, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shmop_read, 0, 3, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, shmop, Shmop, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shmop_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, shmop, Shmop, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shmop_size, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, shmop, Shmop, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shmop_write, 0, 3, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, shmop, Shmop, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_shmop_delete, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, shmop, Shmop, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(shmop_open);
ZEND_FUNCTION(shmop_read);
ZEND_FUNCTION(shmop_close);
ZEND_FUNCTION(shmop_size);
ZEND_FUNCTION(shmop_write);
ZEND_FUNCTION(shmop_delete);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY("shmop_open", zif_shmop_open, arginfo_shmop_open, 0, NULL)
	ZEND_RAW_FENTRY("shmop_read", zif_shmop_read, arginfo_shmop_read, 0, NULL)
	ZEND_RAW_FENTRY("shmop_close", zif_shmop_close, arginfo_shmop_close, ZEND_ACC_DEPRECATED, NULL)
	ZEND_RAW_FENTRY("shmop_size", zif_shmop_size, arginfo_shmop_size, 0, NULL)
	ZEND_RAW_FENTRY("shmop_write", zif_shmop_write, arginfo_shmop_write, 0, NULL)
	ZEND_RAW_FENTRY("shmop_delete", zif_shmop_delete, arginfo_shmop_delete, 0, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_Shmop_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_Shmop(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Shmop", class_Shmop_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
