/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 6781103bb88a5cb7c4e776301f0c8689582a29ab */

#if PHP_OPENSSL_API_VERSION >= 0x30200
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_password_hash, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, algo, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()
#endif

#if PHP_OPENSSL_API_VERSION >= 0x30200
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_openssl_password_verify, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, algo, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, hash, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if PHP_OPENSSL_API_VERSION >= 0x30200
ZEND_FUNCTION(openssl_password_hash);
#endif
#if PHP_OPENSSL_API_VERSION >= 0x30200
ZEND_FUNCTION(openssl_password_verify);
#endif

static const zend_function_entry ext_functions[] = {
#if PHP_OPENSSL_API_VERSION >= 0x30200
	ZEND_FE(openssl_password_hash, arginfo_openssl_password_hash)
#endif
#if PHP_OPENSSL_API_VERSION >= 0x30200
	ZEND_FE(openssl_password_verify, arginfo_openssl_password_verify)
#endif
	ZEND_FE_END
};

static void register_openssl_pwhash_symbols(int module_number)
{
#if PHP_OPENSSL_API_VERSION >= 0x30200
	REGISTER_STRING_CONSTANT("PASSWORD_ARGON2I", "argon2i", CONST_PERSISTENT);
#endif
#if PHP_OPENSSL_API_VERSION >= 0x30200
	REGISTER_STRING_CONSTANT("PASSWORD_ARGON2ID", "argon2id", CONST_PERSISTENT);
#endif
#if PHP_OPENSSL_API_VERSION >= 0x30200
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2_DEFAULT_MEMORY_COST", PHP_OPENSSL_PWHASH_MEMLIMIT, CONST_PERSISTENT);
#endif
#if PHP_OPENSSL_API_VERSION >= 0x30200
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2_DEFAULT_TIME_COST", PHP_OPENSSL_PWHASH_OPSLIMIT, CONST_PERSISTENT);
#endif
#if PHP_OPENSSL_API_VERSION >= 0x30200
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2_DEFAULT_THREADS", PHP_OPENSSL_PWHASH_THREADS, CONST_PERSISTENT);
#endif
#if PHP_OPENSSL_API_VERSION >= 0x30200
	REGISTER_STRING_CONSTANT("PASSWORD_ARGON2_PROVIDER", "openssl", CONST_PERSISTENT);
#endif

#if PHP_OPENSSL_API_VERSION >= 0x30200

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_password_hash", sizeof("openssl_password_hash") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
#if PHP_OPENSSL_API_VERSION >= 0x30200

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "openssl_password_verify", sizeof("openssl_password_verify") - 1), 1, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
#endif
}
