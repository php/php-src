/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 0af5e8985dd4645bf23490b8cec312f8fd1fee2e */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Generator_rewind, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Generator_valid, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Generator_current, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Generator_key arginfo_class_Generator_current

#define arginfo_class_Generator_next arginfo_class_Generator_rewind

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Generator_send, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Generator_throw, 0, 1, IS_MIXED, 0)
	ZEND_ARG_OBJ_INFO(0, exception, Throwable, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Generator_getReturn arginfo_class_Generator_current

ZEND_METHOD(Generator, rewind);
ZEND_METHOD(Generator, valid);
ZEND_METHOD(Generator, current);
ZEND_METHOD(Generator, key);
ZEND_METHOD(Generator, next);
ZEND_METHOD(Generator, send);
ZEND_METHOD(Generator, throw);
ZEND_METHOD(Generator, getReturn);

static const zend_function_entry class_Generator_methods[] = {
	ZEND_RAW_FENTRY("rewind", zim_Generator_rewind, arginfo_class_Generator_rewind, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("valid", zim_Generator_valid, arginfo_class_Generator_valid, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("current", zim_Generator_current, arginfo_class_Generator_current, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("key", zim_Generator_key, arginfo_class_Generator_key, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("next", zim_Generator_next, arginfo_class_Generator_next, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("send", zim_Generator_send, arginfo_class_Generator_send, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("throw", zim_Generator_throw, arginfo_class_Generator_throw, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getReturn", zim_Generator_getReturn, arginfo_class_Generator_getReturn, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_ClosedGeneratorException_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_Generator(zend_class_entry *class_entry_Iterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Generator", class_Generator_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;
	zend_class_implements(class_entry, 1, class_entry_Iterator);

	return class_entry;
}

static zend_class_entry *register_class_ClosedGeneratorException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ClosedGeneratorException", class_ClosedGeneratorException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Exception);

	return class_entry;
}
