/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 995783f78e8fc6071f5b2ca2690c16a1eafe584d */

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

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Random_Engine_Mt19937___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, seed, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "MT_RAND_MT19937")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_Engine_Mt19937_generate, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_Engine_Mt19937___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_Engine_Mt19937___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Random_Engine_Mt19937___debugInfo arginfo_class_Random_Engine_Mt19937___serialize

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Random_Engine_PcgOneseq128XslRr64___construct, 0, 0, 0)
	ZEND_ARG_TYPE_MASK(0, seed, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_MASK(0, sequence, MAY_BE_STRING|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_Random_Engine_PcgOneseq128XslRr64_generate arginfo_class_Random_Engine_Mt19937_generate

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_Engine_PcgOneseq128XslRr64_jump, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, advance, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Random_Engine_PcgOneseq128XslRr64___serialize arginfo_class_Random_Engine_Mt19937___serialize

#define arginfo_class_Random_Engine_PcgOneseq128XslRr64___unserialize arginfo_class_Random_Engine_Mt19937___unserialize

#define arginfo_class_Random_Engine_PcgOneseq128XslRr64___debugInfo arginfo_class_Random_Engine_Mt19937___serialize

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Random_Engine_Xoshiro256StarStar___construct, 0, 0, 0)
	ZEND_ARG_TYPE_MASK(0, seed, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Random_Engine_Xoshiro256StarStar_generate arginfo_class_Random_Engine_Mt19937_generate

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_Engine_Xoshiro256StarStar_jump, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Random_Engine_Xoshiro256StarStar_jumpLong arginfo_class_Random_Engine_Xoshiro256StarStar_jump

#define arginfo_class_Random_Engine_Xoshiro256StarStar___serialize arginfo_class_Random_Engine_Mt19937___serialize

#define arginfo_class_Random_Engine_Xoshiro256StarStar___unserialize arginfo_class_Random_Engine_Mt19937___unserialize

#define arginfo_class_Random_Engine_Xoshiro256StarStar___debugInfo arginfo_class_Random_Engine_Mt19937___serialize

#define arginfo_class_Random_Engine_Secure_generate arginfo_class_Random_Engine_Mt19937_generate

#define arginfo_class_Random_Engine_generate arginfo_class_Random_Engine_Mt19937_generate

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Random_Randomizer___construct, 0, 0, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, engine, Random\\Engine, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Random_Randomizer_getInt arginfo_rand

#define arginfo_class_Random_Randomizer_getBytes arginfo_random_bytes

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_Randomizer_shuffleArray, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_Randomizer_shuffleBytes, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, bytes, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Random_Randomizer_pickArrayKeys, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Random_Randomizer___serialize arginfo_class_Random_Engine_Mt19937___serialize

#define arginfo_class_Random_Randomizer___unserialize arginfo_class_Random_Engine_Mt19937___unserialize


ZEND_FUNCTION(lcg_value);
ZEND_FUNCTION(mt_srand);
ZEND_FUNCTION(rand);
ZEND_FUNCTION(mt_rand);
ZEND_FUNCTION(mt_getrandmax);
ZEND_FUNCTION(random_bytes);
ZEND_FUNCTION(random_int);
ZEND_METHOD(Random_Engine_Mt19937, __construct);
ZEND_METHOD(Random_Engine_Mt19937, generate);
ZEND_METHOD(Random_Engine_Mt19937, __serialize);
ZEND_METHOD(Random_Engine_Mt19937, __unserialize);
ZEND_METHOD(Random_Engine_Mt19937, __debugInfo);
ZEND_METHOD(Random_Engine_PcgOneseq128XslRr64, __construct);
ZEND_METHOD(Random_Engine_PcgOneseq128XslRr64, jump);
ZEND_METHOD(Random_Engine_Xoshiro256StarStar, __construct);
ZEND_METHOD(Random_Engine_Xoshiro256StarStar, jump);
ZEND_METHOD(Random_Engine_Xoshiro256StarStar, jumpLong);
ZEND_METHOD(Random_Randomizer, __construct);
ZEND_METHOD(Random_Randomizer, getInt);
ZEND_METHOD(Random_Randomizer, getBytes);
ZEND_METHOD(Random_Randomizer, shuffleArray);
ZEND_METHOD(Random_Randomizer, shuffleBytes);
ZEND_METHOD(Random_Randomizer, pickArrayKeys);
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


static const zend_function_entry class_Random_Engine_Mt19937_methods[] = {
	ZEND_ME(Random_Engine_Mt19937, __construct, arginfo_class_Random_Engine_Mt19937___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Engine_Mt19937, generate, arginfo_class_Random_Engine_Mt19937_generate, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Engine_Mt19937, __serialize, arginfo_class_Random_Engine_Mt19937___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Engine_Mt19937, __unserialize, arginfo_class_Random_Engine_Mt19937___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Engine_Mt19937, __debugInfo, arginfo_class_Random_Engine_Mt19937___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Random_Engine_PcgOneseq128XslRr64_methods[] = {
	ZEND_ME(Random_Engine_PcgOneseq128XslRr64, __construct, arginfo_class_Random_Engine_PcgOneseq128XslRr64___construct, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_Engine_Mt19937, generate, generate, arginfo_class_Random_Engine_PcgOneseq128XslRr64_generate, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Engine_PcgOneseq128XslRr64, jump, arginfo_class_Random_Engine_PcgOneseq128XslRr64_jump, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_Engine_Mt19937, __serialize, __serialize, arginfo_class_Random_Engine_PcgOneseq128XslRr64___serialize, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_Engine_Mt19937, __unserialize, __unserialize, arginfo_class_Random_Engine_PcgOneseq128XslRr64___unserialize, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_Engine_Mt19937, __debugInfo, __debugInfo, arginfo_class_Random_Engine_PcgOneseq128XslRr64___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Random_Engine_Xoshiro256StarStar_methods[] = {
	ZEND_ME(Random_Engine_Xoshiro256StarStar, __construct, arginfo_class_Random_Engine_Xoshiro256StarStar___construct, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_Engine_Mt19937, generate, generate, arginfo_class_Random_Engine_Xoshiro256StarStar_generate, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Engine_Xoshiro256StarStar, jump, arginfo_class_Random_Engine_Xoshiro256StarStar_jump, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Engine_Xoshiro256StarStar, jumpLong, arginfo_class_Random_Engine_Xoshiro256StarStar_jumpLong, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_Engine_Mt19937, __serialize, __serialize, arginfo_class_Random_Engine_Xoshiro256StarStar___serialize, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_Engine_Mt19937, __unserialize, __unserialize, arginfo_class_Random_Engine_Xoshiro256StarStar___unserialize, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(Random_Engine_Mt19937, __debugInfo, __debugInfo, arginfo_class_Random_Engine_Xoshiro256StarStar___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Random_Engine_Secure_methods[] = {
	ZEND_MALIAS(Random_Engine_Mt19937, generate, generate, arginfo_class_Random_Engine_Secure_generate, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Random_Engine_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(Random_Engine, generate, arginfo_class_Random_Engine_generate, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_Random_CryptoSafeEngine_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_Random_Randomizer_methods[] = {
	ZEND_ME(Random_Randomizer, __construct, arginfo_class_Random_Randomizer___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Randomizer, getInt, arginfo_class_Random_Randomizer_getInt, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Randomizer, getBytes, arginfo_class_Random_Randomizer_getBytes, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Randomizer, shuffleArray, arginfo_class_Random_Randomizer_shuffleArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Randomizer, shuffleBytes, arginfo_class_Random_Randomizer_shuffleBytes, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Randomizer, pickArrayKeys, arginfo_class_Random_Randomizer_pickArrayKeys, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Randomizer, __serialize, arginfo_class_Random_Randomizer___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Random_Randomizer, __unserialize, arginfo_class_Random_Randomizer___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Random_Engine_Mt19937(zend_class_entry *class_entry_Random_Engine)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random\\Engine", "Mt19937", class_Random_Engine_Mt19937_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	zend_class_implements(class_entry, 1, class_entry_Random_Engine);

	return class_entry;
}

static zend_class_entry *register_class_Random_Engine_PcgOneseq128XslRr64(zend_class_entry *class_entry_Random_Engine)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random\\Engine", "PcgOneseq128XslRr64", class_Random_Engine_PcgOneseq128XslRr64_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	zend_class_implements(class_entry, 1, class_entry_Random_Engine);

	return class_entry;
}

static zend_class_entry *register_class_Random_Engine_Xoshiro256StarStar(zend_class_entry *class_entry_Random_Engine)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random\\Engine", "Xoshiro256StarStar", class_Random_Engine_Xoshiro256StarStar_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	zend_class_implements(class_entry, 1, class_entry_Random_Engine);

	return class_entry;
}

static zend_class_entry *register_class_Random_Engine_Secure(zend_class_entry *class_entry_Random_CryptoSafeEngine)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random\\Engine", "Secure", class_Random_Engine_Secure_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;
	zend_class_implements(class_entry, 1, class_entry_Random_CryptoSafeEngine);

	return class_entry;
}

static zend_class_entry *register_class_Random_Engine(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random", "Engine", class_Random_Engine_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_Random_CryptoSafeEngine(zend_class_entry *class_entry_Random_Engine)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random", "CryptoSafeEngine", class_Random_CryptoSafeEngine_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 1, class_entry_Random_Engine);

	return class_entry;
}

static zend_class_entry *register_class_Random_Randomizer(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Random", "Randomizer", class_Random_Randomizer_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	zend_string *property_engine_class_Random_Engine = zend_string_init("Random\\Engine", sizeof("Random\\Engine")-1, 1);
	zval property_engine_default_value;
	ZVAL_UNDEF(&property_engine_default_value);
	zend_string *property_engine_name = zend_string_init("engine", sizeof("engine") - 1, 1);
	zend_declare_typed_property(class_entry, property_engine_name, &property_engine_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_engine_class_Random_Engine, 0, 0));
	zend_string_release(property_engine_name);

	return class_entry;
}
