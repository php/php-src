/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 72e03ec3a21486d6426fc0658f0c133247da141f */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_NumberGenerator_generate, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Random_NumberGenerator_XorShift128Plus___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, seed, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Random_NumberGenerator_XorShift128Plus_generate arginfo_class_Random_NumberGenerator_generate

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_NumberGenerator_XorShift128Plus___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_NumberGenerator_XorShift128Plus___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Random_NumberGenerator_MT19937___construct arginfo_class_Random_NumberGenerator_XorShift128Plus___construct

#define arginfo_class_Random_NumberGenerator_MT19937_generate arginfo_class_Random_NumberGenerator_generate

#define arginfo_class_Random_NumberGenerator_MT19937___serialize arginfo_class_Random_NumberGenerator_XorShift128Plus___serialize

#define arginfo_class_Random_NumberGenerator_MT19937___unserialize arginfo_class_Random_NumberGenerator_XorShift128Plus___unserialize

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Random_NumberGenerator_Secure___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Random_NumberGenerator_Secure_generate arginfo_class_Random_NumberGenerator_generate

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Random___construct, 0, 0, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, rng, Random\\NumberGenerator, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Random_getNumberGenerator, 0, 0, Random\\NumberGenerator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_getInt, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_getBytes, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_shuffleArray, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_shuffleString, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Random___serialize arginfo_class_Random_NumberGenerator_XorShift128Plus___serialize

#define arginfo_class_Random___unserialize arginfo_class_Random_NumberGenerator_XorShift128Plus___unserialize


ZEND_METHOD(Random_NumberGenerator_XorShift128Plus, __construct);
ZEND_METHOD(Random_NumberGenerator_XorShift128Plus, generate);
ZEND_METHOD(Random_NumberGenerator_XorShift128Plus, __serialize);
ZEND_METHOD(Random_NumberGenerator_XorShift128Plus, __unserialize);
ZEND_METHOD(Random_NumberGenerator_Secure, __construct);
ZEND_METHOD(Random, __construct);
ZEND_METHOD(Random, getNumberGenerator);
ZEND_METHOD(Random, getInt);
ZEND_METHOD(Random, getBytes);
ZEND_METHOD(Random, shuffleArray);
ZEND_METHOD(Random, shuffleString);
ZEND_METHOD(Random, __serialize);
ZEND_METHOD(Random, __unserialize);


static const zend_function_entry class_Random_NumberGenerator_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(Random_NumberGenerator, generate, arginfo_class_Random_NumberGenerator_generate, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_Random_NumberGenerator_XorShift128Plus_methods[] = {
	ZEND_ME(Random_NumberGenerator_XorShift128Plus, __construct, arginfo_class_Random_NumberGenerator_XorShift128Plus___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_NumberGenerator_XorShift128Plus, generate, arginfo_class_Random_NumberGenerator_XorShift128Plus_generate, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_NumberGenerator_XorShift128Plus, __serialize, arginfo_class_Random_NumberGenerator_XorShift128Plus___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_NumberGenerator_XorShift128Plus, __unserialize, arginfo_class_Random_NumberGenerator_XorShift128Plus___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Random_NumberGenerator_MT19937_methods[] = {
	ZEND_MALIAS(Random_NumberGenerator_XorShift128Plus, __construct, __construct, arginfo_class_Random_NumberGenerator_MT19937___construct, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_NumberGenerator_XorShift128Plus, generate, generate, arginfo_class_Random_NumberGenerator_MT19937_generate, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_NumberGenerator_XorShift128Plus, __serialize, __serialize, arginfo_class_Random_NumberGenerator_MT19937___serialize, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_NumberGenerator_XorShift128Plus, __unserialize, __unserialize, arginfo_class_Random_NumberGenerator_MT19937___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Random_NumberGenerator_Secure_methods[] = {
	ZEND_ME(Random_NumberGenerator_Secure, __construct, arginfo_class_Random_NumberGenerator_Secure___construct, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_NumberGenerator_XorShift128Plus, generate, generate, arginfo_class_Random_NumberGenerator_Secure_generate, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Random_methods[] = {
	ZEND_ME(Random, __construct, arginfo_class_Random___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Random, getNumberGenerator, arginfo_class_Random_getNumberGenerator, ZEND_ACC_PUBLIC)
	ZEND_ME(Random, getInt, arginfo_class_Random_getInt, ZEND_ACC_PUBLIC)
	ZEND_ME(Random, getBytes, arginfo_class_Random_getBytes, ZEND_ACC_PUBLIC)
	ZEND_ME(Random, shuffleArray, arginfo_class_Random_shuffleArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Random, shuffleString, arginfo_class_Random_shuffleString, ZEND_ACC_PUBLIC)
	ZEND_ME(Random, __serialize, arginfo_class_Random___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Random, __unserialize, arginfo_class_Random___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Random_NumberGenerator(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random", "NumberGenerator", class_Random_NumberGenerator_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_Random_NumberGenerator_XorShift128Plus(zend_class_entry *class_entry_Random_NumberGenerator_Random_NumberGenerator)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random\\NumberGenerator", "XorShift128Plus", class_Random_NumberGenerator_XorShift128Plus_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry_Random_NumberGenerator_Random_NumberGenerator);

	return class_entry;
}

static zend_class_entry *register_class_Random_NumberGenerator_MT19937(zend_class_entry *class_entry_Random_NumberGenerator_Random_NumberGenerator)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random\\NumberGenerator", "MT19937", class_Random_NumberGenerator_MT19937_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry_Random_NumberGenerator_Random_NumberGenerator);

	return class_entry;
}

static zend_class_entry *register_class_Random_NumberGenerator_Secure(zend_class_entry *class_entry_Random_NumberGenerator_Random_NumberGenerator)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random\\NumberGenerator", "Secure", class_Random_NumberGenerator_Secure_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry_Random_NumberGenerator_Random_NumberGenerator);

	return class_entry;
}

static zend_class_entry *register_class_Random(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Random", class_Random_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	zval property_rng_default_value;
	ZVAL_UNDEF(&property_rng_default_value);
	zend_string *property_rng_name = zend_string_init("rng", sizeof("rng") - 1, 1);
	zend_declare_typed_property(class_entry, property_rng_name, &property_rng_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ANY));
	zend_string_release(property_rng_name);

	return class_entry;
}
