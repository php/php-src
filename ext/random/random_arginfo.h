/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: ffdd4ca586fc2bbdcbc8c03a8d922cac01597050 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lcg_value, 0, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mt_srand, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, seed, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "MT_RAND_MT19937")
ZEND_END_ARG_INFO()

#define arginfo_srand arginfo_mt_srand

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_rand, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_mt_rand arginfo_rand

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mt_getrandmax, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_getrandmax arginfo_mt_getrandmax

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_random_bytes, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_random_int, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Random_NumberGenerator_XorShift128Plus___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, seed, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Random_NumberGenerator_XorShift128Plus_generate arginfo_mt_getrandmax

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_NumberGenerator_XorShift128Plus___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_NumberGenerator_XorShift128Plus___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Random_NumberGenerator_MersenneTwister___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, seed, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "MT_RAND_MT19937")
ZEND_END_ARG_INFO()

#define arginfo_class_Random_NumberGenerator_MersenneTwister_generate arginfo_mt_getrandmax

#define arginfo_class_Random_NumberGenerator_MersenneTwister___serialize arginfo_class_Random_NumberGenerator_XorShift128Plus___serialize

#define arginfo_class_Random_NumberGenerator_MersenneTwister___unserialize arginfo_class_Random_NumberGenerator_XorShift128Plus___unserialize

#define arginfo_class_Random_NumberGenerator_CombinedLCG___construct arginfo_class_Random_NumberGenerator_XorShift128Plus___construct

#define arginfo_class_Random_NumberGenerator_CombinedLCG_generate arginfo_mt_getrandmax

#define arginfo_class_Random_NumberGenerator_CombinedLCG___serialize arginfo_class_Random_NumberGenerator_XorShift128Plus___serialize

#define arginfo_class_Random_NumberGenerator_CombinedLCG___unserialize arginfo_class_Random_NumberGenerator_XorShift128Plus___unserialize

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Random_NumberGenerator_Secure___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Random_NumberGenerator_Secure_generate arginfo_mt_getrandmax

#define arginfo_class_Random_NumberGenerator_generate arginfo_mt_getrandmax

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Random_Randomizer___construct, 0, 0, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, numberGenerator, Random\\NumberGenerator, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Random_Randomizer_getInt arginfo_random_int

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_Randomizer_getBytes, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, legnth, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_Randomizer_shuffleArray, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_Randomizer_shuffleString, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Random_Randomizer___serialize arginfo_class_Random_NumberGenerator_XorShift128Plus___serialize

#define arginfo_class_Random_Randomizer___unserialize arginfo_class_Random_NumberGenerator_XorShift128Plus___unserialize


ZEND_FUNCTION(lcg_value);
ZEND_FUNCTION(mt_srand);
ZEND_FUNCTION(rand);
ZEND_FUNCTION(mt_rand);
ZEND_FUNCTION(mt_getrandmax);
ZEND_FUNCTION(random_bytes);
ZEND_FUNCTION(random_int);
ZEND_METHOD(Random_NumberGenerator_XorShift128Plus, __construct);
ZEND_METHOD(Random_NumberGenerator_XorShift128Plus, generate);
ZEND_METHOD(Random_NumberGenerator_XorShift128Plus, __serialize);
ZEND_METHOD(Random_NumberGenerator_XorShift128Plus, __unserialize);
ZEND_METHOD(Random_NumberGenerator_MersenneTwister, __construct);
ZEND_METHOD(Random_NumberGenerator_Secure, __construct);
ZEND_METHOD(Random_Randomizer, __construct);
ZEND_METHOD(Random_Randomizer, getInt);
ZEND_METHOD(Random_Randomizer, getBytes);
ZEND_METHOD(Random_Randomizer, shuffleArray);
ZEND_METHOD(Random_Randomizer, shuffleString);
ZEND_METHOD(Random_Randomizer, __serialize);
ZEND_METHOD(Random_Randomizer, __unserialize);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(lcg_value, arginfo_lcg_value)
	ZEND_FE(mt_srand, arginfo_mt_srand)
	ZEND_FALIAS(srand, mt_srand, arginfo_srand)
	ZEND_FE(rand, arginfo_rand)
	ZEND_FE(mt_rand, arginfo_mt_rand)
	ZEND_FE(mt_getrandmax, arginfo_mt_getrandmax)
	ZEND_FALIAS(getrandmax, mt_getrandmax, arginfo_getrandmax)
	ZEND_FE(random_bytes, arginfo_random_bytes)
	ZEND_FE(random_int, arginfo_random_int)
	ZEND_FE_END
};


static const zend_function_entry class_Random_NumberGenerator_XorShift128Plus_methods[] = {
	ZEND_ME(Random_NumberGenerator_XorShift128Plus, __construct, arginfo_class_Random_NumberGenerator_XorShift128Plus___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_NumberGenerator_XorShift128Plus, generate, arginfo_class_Random_NumberGenerator_XorShift128Plus_generate, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_NumberGenerator_XorShift128Plus, __serialize, arginfo_class_Random_NumberGenerator_XorShift128Plus___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_NumberGenerator_XorShift128Plus, __unserialize, arginfo_class_Random_NumberGenerator_XorShift128Plus___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Random_NumberGenerator_MersenneTwister_methods[] = {
	ZEND_ME(Random_NumberGenerator_MersenneTwister, __construct, arginfo_class_Random_NumberGenerator_MersenneTwister___construct, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_NumberGenerator_XorShift128Plus, generate, generate, arginfo_class_Random_NumberGenerator_MersenneTwister_generate, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_NumberGenerator_XorShift128Plus, __serialize, __serialize, arginfo_class_Random_NumberGenerator_MersenneTwister___serialize, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_NumberGenerator_XorShift128Plus, __unserialize, __unserialize, arginfo_class_Random_NumberGenerator_MersenneTwister___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Random_NumberGenerator_CombinedLCG_methods[] = {
	ZEND_MALIAS(Random_NumberGenerator_XorShift128Plus, __construct, __construct, arginfo_class_Random_NumberGenerator_CombinedLCG___construct, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_NumberGenerator_XorShift128Plus, generate, generate, arginfo_class_Random_NumberGenerator_CombinedLCG_generate, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_NumberGenerator_XorShift128Plus, __serialize, __serialize, arginfo_class_Random_NumberGenerator_CombinedLCG___serialize, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_NumberGenerator_XorShift128Plus, __unserialize, __unserialize, arginfo_class_Random_NumberGenerator_CombinedLCG___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Random_NumberGenerator_Secure_methods[] = {
	ZEND_ME(Random_NumberGenerator_Secure, __construct, arginfo_class_Random_NumberGenerator_Secure___construct, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_NumberGenerator_XorShift128Plus, generate, generate, arginfo_class_Random_NumberGenerator_Secure_generate, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Random_NumberGenerator_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(Random_NumberGenerator, generate, arginfo_class_Random_NumberGenerator_generate, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_Random_Randomizer_methods[] = {
	ZEND_ME(Random_Randomizer, __construct, arginfo_class_Random_Randomizer___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Randomizer, getInt, arginfo_class_Random_Randomizer_getInt, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Randomizer, getBytes, arginfo_class_Random_Randomizer_getBytes, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Randomizer, shuffleArray, arginfo_class_Random_Randomizer_shuffleArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Randomizer, shuffleString, arginfo_class_Random_Randomizer_shuffleString, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Randomizer, __serialize, arginfo_class_Random_Randomizer___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Randomizer, __unserialize, arginfo_class_Random_Randomizer___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Random_NumberGenerator_XorShift128Plus(zend_class_entry *class_entry_Random_NumberGenerator_Random_NumberGenerator)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random\\NumberGenerator", "XorShift128Plus", class_Random_NumberGenerator_XorShift128Plus_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry_Random_NumberGenerator_Random_NumberGenerator);

	return class_entry;
}

static zend_class_entry *register_class_Random_NumberGenerator_MersenneTwister(zend_class_entry *class_entry_Random_NumberGenerator_Random_NumberGenerator)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random\\NumberGenerator", "MersenneTwister", class_Random_NumberGenerator_MersenneTwister_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry_Random_NumberGenerator_Random_NumberGenerator);

	return class_entry;
}

static zend_class_entry *register_class_Random_NumberGenerator_CombinedLCG(zend_class_entry *class_entry_Random_NumberGenerator_Random_NumberGenerator)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random\\NumberGenerator", "CombinedLCG", class_Random_NumberGenerator_CombinedLCG_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry_Random_NumberGenerator_Random_NumberGenerator);

	return class_entry;
}

static zend_class_entry *register_class_Random_NumberGenerator_Secure(zend_class_entry *class_entry_Random_NumberGenerator_Random_NumberGenerator)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random\\NumberGenerator", "Secure", class_Random_NumberGenerator_Secure_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
	zend_class_implements(class_entry, 1, class_entry_Random_NumberGenerator_Random_NumberGenerator);

	return class_entry;
}

static zend_class_entry *register_class_Random_NumberGenerator(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random", "NumberGenerator", class_Random_NumberGenerator_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_Random_Randomizer(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random", "Randomizer", class_Random_Randomizer_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	zend_string *property_numberGenerator_class_Random_NumberGenerator = zend_string_init("Random\\NumberGenerator", sizeof("Random\\NumberGenerator")-1, 1);
	zval property_numberGenerator_default_value;
	ZVAL_UNDEF(&property_numberGenerator_default_value);
	zend_string *property_numberGenerator_name = zend_string_init("numberGenerator", sizeof("numberGenerator") - 1, 1);
	zend_declare_typed_property(class_entry, property_numberGenerator_name, &property_numberGenerator_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_numberGenerator_class_Random_NumberGenerator, 0, 0));
	zend_string_release(property_numberGenerator_name);

	return class_entry;
}
