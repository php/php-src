/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f4531ebc6817042a2729c3dd1502631656c29f05 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_hash, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, algo, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, binary, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_hash_file, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, algo, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, binary, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_hash_hmac, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, algo, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, binary, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_hash_hmac_file, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, algo, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, binary, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_hash_init, 0, 1, HashContext, 0)
	ZEND_ARG_TYPE_INFO(0, algo, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, key, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_hash_update, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, context, HashContext, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_hash_update_stream, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, context, HashContext, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_hash_update_file, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, context, HashContext, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, stream_context, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_hash_final, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, context, HashContext, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, binary, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_hash_copy, 0, 1, HashContext, 0)
	ZEND_ARG_OBJ_INFO(0, context, HashContext, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_hash_algos, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_hash_hmac_algos arginfo_hash_algos

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_hash_pbkdf2, 0, 4, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, algo, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, salt, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, iterations, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, binary, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_hash_equals, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, known_string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, user_string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_hash_hkdf, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, algo, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, info, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, salt, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#if defined(PHP_MHASH_BC)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mhash_get_block_size, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, algo, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_MHASH_BC)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mhash_get_hash_name, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, algo, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_MHASH_BC)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mhash_keygen_s2k, 0, 4, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, algo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, salt, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_MHASH_BC)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mhash_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_MHASH_BC)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mhash, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, algo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, key, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_HashContext___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_HashContext___serialize arginfo_hash_algos

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_HashContext___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(hash);
ZEND_FUNCTION(hash_file);
ZEND_FUNCTION(hash_hmac);
ZEND_FUNCTION(hash_hmac_file);
ZEND_FUNCTION(hash_init);
ZEND_FUNCTION(hash_update);
ZEND_FUNCTION(hash_update_stream);
ZEND_FUNCTION(hash_update_file);
ZEND_FUNCTION(hash_final);
ZEND_FUNCTION(hash_copy);
ZEND_FUNCTION(hash_algos);
ZEND_FUNCTION(hash_hmac_algos);
ZEND_FUNCTION(hash_pbkdf2);
ZEND_FUNCTION(hash_equals);
ZEND_FUNCTION(hash_hkdf);
#if defined(PHP_MHASH_BC)
ZEND_FUNCTION(mhash_get_block_size);
#endif
#if defined(PHP_MHASH_BC)
ZEND_FUNCTION(mhash_get_hash_name);
#endif
#if defined(PHP_MHASH_BC)
ZEND_FUNCTION(mhash_keygen_s2k);
#endif
#if defined(PHP_MHASH_BC)
ZEND_FUNCTION(mhash_count);
#endif
#if defined(PHP_MHASH_BC)
ZEND_FUNCTION(mhash);
#endif
ZEND_METHOD(HashContext, __construct);
ZEND_METHOD(HashContext, __serialize);
ZEND_METHOD(HashContext, __unserialize);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY("hash", zif_hash, arginfo_hash, 0, NULL)
	ZEND_RAW_FENTRY("hash_file", zif_hash_file, arginfo_hash_file, 0, NULL)
	ZEND_RAW_FENTRY("hash_hmac", zif_hash_hmac, arginfo_hash_hmac, 0, NULL)
	ZEND_RAW_FENTRY("hash_hmac_file", zif_hash_hmac_file, arginfo_hash_hmac_file, 0, NULL)
	ZEND_RAW_FENTRY("hash_init", zif_hash_init, arginfo_hash_init, 0, NULL)
	ZEND_RAW_FENTRY("hash_update", zif_hash_update, arginfo_hash_update, 0, NULL)
	ZEND_RAW_FENTRY("hash_update_stream", zif_hash_update_stream, arginfo_hash_update_stream, 0, NULL)
	ZEND_RAW_FENTRY("hash_update_file", zif_hash_update_file, arginfo_hash_update_file, 0, NULL)
	ZEND_RAW_FENTRY("hash_final", zif_hash_final, arginfo_hash_final, 0, NULL)
	ZEND_RAW_FENTRY("hash_copy", zif_hash_copy, arginfo_hash_copy, 0, NULL)
	ZEND_RAW_FENTRY("hash_algos", zif_hash_algos, arginfo_hash_algos, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("hash_hmac_algos", zif_hash_hmac_algos, arginfo_hash_hmac_algos, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("hash_pbkdf2", zif_hash_pbkdf2, arginfo_hash_pbkdf2, 0, NULL)
	ZEND_RAW_FENTRY("hash_equals", zif_hash_equals, arginfo_hash_equals, 0, NULL)
	ZEND_RAW_FENTRY("hash_hkdf", zif_hash_hkdf, arginfo_hash_hkdf, 0, NULL)
#if defined(PHP_MHASH_BC)
	ZEND_RAW_FENTRY("mhash_get_block_size", zif_mhash_get_block_size, arginfo_mhash_get_block_size, ZEND_ACC_DEPRECATED, NULL)
#endif
#if defined(PHP_MHASH_BC)
	ZEND_RAW_FENTRY("mhash_get_hash_name", zif_mhash_get_hash_name, arginfo_mhash_get_hash_name, ZEND_ACC_DEPRECATED, NULL)
#endif
#if defined(PHP_MHASH_BC)
	ZEND_RAW_FENTRY("mhash_keygen_s2k", zif_mhash_keygen_s2k, arginfo_mhash_keygen_s2k, ZEND_ACC_DEPRECATED, NULL)
#endif
#if defined(PHP_MHASH_BC)
	ZEND_RAW_FENTRY("mhash_count", zif_mhash_count, arginfo_mhash_count, ZEND_ACC_DEPRECATED, NULL)
#endif
#if defined(PHP_MHASH_BC)
	ZEND_RAW_FENTRY("mhash", zif_mhash, arginfo_mhash, ZEND_ACC_DEPRECATED, NULL)
#endif
	ZEND_FE_END
};

static const zend_function_entry class_HashContext_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_HashContext___construct, arginfo_class_HashContext___construct, ZEND_ACC_PRIVATE, NULL)
	ZEND_RAW_FENTRY("__serialize", zim_HashContext___serialize, arginfo_class_HashContext___serialize, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("__unserialize", zim_HashContext___unserialize, arginfo_class_HashContext___unserialize, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static void register_hash_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("HASH_HMAC", PHP_HASH_HMAC, CONST_PERSISTENT);


	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "hash_hmac", sizeof("hash_hmac") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "hash_hmac_file", sizeof("hash_hmac_file") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "hash_init", sizeof("hash_init") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "hash_pbkdf2", sizeof("hash_pbkdf2") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "hash_equals", sizeof("hash_equals") - 1), 0, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "hash_equals", sizeof("hash_equals") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "hash_hkdf", sizeof("hash_hkdf") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
}

static zend_class_entry *register_class_HashContext(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "HashContext", class_HashContext_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}
