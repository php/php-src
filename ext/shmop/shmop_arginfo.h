/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: e7f250077b6721539caee96afe4ed392396018f9 */

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
	ZEND_FE(shmop_open, arginfo_shmop_open)
	ZEND_FE(shmop_read, arginfo_shmop_read)
	ZEND_RAW_FENTRY("shmop_close", zif_shmop_close, arginfo_shmop_close, ZEND_ACC_DEPRECATED, NULL, NULL)
	ZEND_FE(shmop_size, arginfo_shmop_size)
	ZEND_FE(shmop_write, arginfo_shmop_write)
	ZEND_FE(shmop_delete, arginfo_shmop_delete)
	ZEND_FE_END
};

static void register_shmop_symbols(int module_number)
{

	zend_attribute *attribute_Deprecated_func_shmop_close_0 = zend_add_function_attribute(zend_hash_str_find_ptr(CG(function_table), "shmop_close", sizeof("shmop_close") - 1), ZSTR_KNOWN(ZEND_STR_DEPRECATED_CAPITALIZED), 2);
	zval attribute_Deprecated_func_shmop_close_0_arg0;
	zend_string *attribute_Deprecated_func_shmop_close_0_arg0_str = zend_string_init("8.0", strlen("8.0"), 1);
	ZVAL_STR(&attribute_Deprecated_func_shmop_close_0_arg0, attribute_Deprecated_func_shmop_close_0_arg0_str);
	ZVAL_COPY_VALUE(&attribute_Deprecated_func_shmop_close_0->args[0].value, &attribute_Deprecated_func_shmop_close_0_arg0);
	attribute_Deprecated_func_shmop_close_0->args[0].name = ZSTR_KNOWN(ZEND_STR_SINCE);
	zval attribute_Deprecated_func_shmop_close_0_arg1;
	zend_string *attribute_Deprecated_func_shmop_close_0_arg1_str = zend_string_init("as Shmop objects are freed automatically", strlen("as Shmop objects are freed automatically"), 1);
	ZVAL_STR(&attribute_Deprecated_func_shmop_close_0_arg1, attribute_Deprecated_func_shmop_close_0_arg1_str);
	ZVAL_COPY_VALUE(&attribute_Deprecated_func_shmop_close_0->args[1].value, &attribute_Deprecated_func_shmop_close_0_arg1);
	attribute_Deprecated_func_shmop_close_0->args[1].name = ZSTR_KNOWN(ZEND_STR_MESSAGE);
}

static zend_class_entry *register_class_Shmop(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Shmop", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}
