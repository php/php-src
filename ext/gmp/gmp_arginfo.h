/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d52f82c7084a8122fe07c91eb6d4ab6030daa27d */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_gmp_init, 0, 1, GMP, 0)
	ZEND_ARG_TYPE_MASK(0, num, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, base, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_gmp_import, 0, 1, GMP, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, word_size, IS_LONG, 0, "1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "GMP_MSW_FIRST | GMP_NATIVE_ENDIAN")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_export, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, word_size, IS_LONG, 0, "1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "GMP_MSW_FIRST | GMP_NATIVE_ENDIAN")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_intval, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_strval, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, base, IS_LONG, 0, "10")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_gmp_add, 0, 2, GMP, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num1, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_OBJ_TYPE_MASK(0, num2, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

#define arginfo_gmp_sub arginfo_gmp_add

#define arginfo_gmp_mul arginfo_gmp_add

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_div_qr, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num1, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_OBJ_TYPE_MASK(0, num2, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, rounding_mode, IS_LONG, 0, "GMP_ROUND_ZERO")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_gmp_div_q, 0, 2, GMP, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num1, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_OBJ_TYPE_MASK(0, num2, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, rounding_mode, IS_LONG, 0, "GMP_ROUND_ZERO")
ZEND_END_ARG_INFO()

#define arginfo_gmp_div_r arginfo_gmp_div_q

#define arginfo_gmp_div arginfo_gmp_div_q

#define arginfo_gmp_mod arginfo_gmp_add

#define arginfo_gmp_divexact arginfo_gmp_add

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_gmp_neg, 0, 1, GMP, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

#define arginfo_gmp_abs arginfo_gmp_neg

#define arginfo_gmp_fact arginfo_gmp_neg

#define arginfo_gmp_sqrt arginfo_gmp_neg

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_sqrtrem, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_gmp_root, 0, 2, GMP, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, nth, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_rootrem, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, nth, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_gmp_pow, 0, 2, GMP, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, exponent, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_gmp_powm, 0, 3, GMP, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_OBJ_TYPE_MASK(0, exponent, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_OBJ_TYPE_MASK(0, modulus, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_perfect_square, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

#define arginfo_gmp_perfect_power arginfo_gmp_perfect_square

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_prob_prime, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, repetitions, IS_LONG, 0, "10")
ZEND_END_ARG_INFO()

#define arginfo_gmp_gcd arginfo_gmp_add

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_gcdext, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num1, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_OBJ_TYPE_MASK(0, num2, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

#define arginfo_gmp_lcm arginfo_gmp_add

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_gmp_invert, 0, 2, GMP, MAY_BE_FALSE)
	ZEND_ARG_OBJ_TYPE_MASK(0, num1, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_OBJ_TYPE_MASK(0, num2, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_jacobi, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num1, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_OBJ_TYPE_MASK(0, num2, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

#define arginfo_gmp_legendre arginfo_gmp_jacobi

#define arginfo_gmp_kronecker arginfo_gmp_jacobi

#define arginfo_gmp_cmp arginfo_gmp_jacobi

#define arginfo_gmp_sign arginfo_gmp_intval

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_random_seed, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, seed, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_gmp_random_bits, 0, 1, GMP, 0)
	ZEND_ARG_TYPE_INFO(0, bits, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_gmp_random_range, 0, 2, GMP, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, min, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_OBJ_TYPE_MASK(0, max, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

#define arginfo_gmp_and arginfo_gmp_add

#define arginfo_gmp_or arginfo_gmp_add

#define arginfo_gmp_com arginfo_gmp_neg

#define arginfo_gmp_xor arginfo_gmp_add

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_setbit, 0, 2, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, num, GMP, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_clrbit, 0, 2, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, num, GMP, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_testbit, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_scan0, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num1, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_gmp_scan1 arginfo_gmp_scan0

#define arginfo_gmp_popcount arginfo_gmp_intval

#define arginfo_gmp_hamdist arginfo_gmp_jacobi

#define arginfo_gmp_nextprime arginfo_gmp_neg

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_gmp_binomial, 0, 2, GMP, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, n, GMP, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, k, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_GMP___construct, 0, 0, 0)
	ZEND_ARG_TYPE_MASK(0, num, MAY_BE_LONG|MAY_BE_STRING, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, base, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_GMP___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_GMP___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(gmp_init);
ZEND_FUNCTION(gmp_import);
ZEND_FUNCTION(gmp_export);
ZEND_FUNCTION(gmp_intval);
ZEND_FUNCTION(gmp_strval);
ZEND_FUNCTION(gmp_add);
ZEND_FUNCTION(gmp_sub);
ZEND_FUNCTION(gmp_mul);
ZEND_FUNCTION(gmp_div_qr);
ZEND_FUNCTION(gmp_div_q);
ZEND_FUNCTION(gmp_div_r);
ZEND_FUNCTION(gmp_mod);
ZEND_FUNCTION(gmp_divexact);
ZEND_FUNCTION(gmp_neg);
ZEND_FUNCTION(gmp_abs);
ZEND_FUNCTION(gmp_fact);
ZEND_FUNCTION(gmp_sqrt);
ZEND_FUNCTION(gmp_sqrtrem);
ZEND_FUNCTION(gmp_root);
ZEND_FUNCTION(gmp_rootrem);
ZEND_FUNCTION(gmp_pow);
ZEND_FUNCTION(gmp_powm);
ZEND_FUNCTION(gmp_perfect_square);
ZEND_FUNCTION(gmp_perfect_power);
ZEND_FUNCTION(gmp_prob_prime);
ZEND_FUNCTION(gmp_gcd);
ZEND_FUNCTION(gmp_gcdext);
ZEND_FUNCTION(gmp_lcm);
ZEND_FUNCTION(gmp_invert);
ZEND_FUNCTION(gmp_jacobi);
ZEND_FUNCTION(gmp_legendre);
ZEND_FUNCTION(gmp_kronecker);
ZEND_FUNCTION(gmp_cmp);
ZEND_FUNCTION(gmp_sign);
ZEND_FUNCTION(gmp_random_seed);
ZEND_FUNCTION(gmp_random_bits);
ZEND_FUNCTION(gmp_random_range);
ZEND_FUNCTION(gmp_and);
ZEND_FUNCTION(gmp_or);
ZEND_FUNCTION(gmp_com);
ZEND_FUNCTION(gmp_xor);
ZEND_FUNCTION(gmp_setbit);
ZEND_FUNCTION(gmp_clrbit);
ZEND_FUNCTION(gmp_testbit);
ZEND_FUNCTION(gmp_scan0);
ZEND_FUNCTION(gmp_scan1);
ZEND_FUNCTION(gmp_popcount);
ZEND_FUNCTION(gmp_hamdist);
ZEND_FUNCTION(gmp_nextprime);
ZEND_FUNCTION(gmp_binomial);
ZEND_METHOD(GMP, __construct);
ZEND_METHOD(GMP, __serialize);
ZEND_METHOD(GMP, __unserialize);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY("gmp_init", zif_gmp_init, arginfo_gmp_init, 0, NULL)
	ZEND_RAW_FENTRY("gmp_import", zif_gmp_import, arginfo_gmp_import, 0, NULL)
	ZEND_RAW_FENTRY("gmp_export", zif_gmp_export, arginfo_gmp_export, 0, NULL)
	ZEND_RAW_FENTRY("gmp_intval", zif_gmp_intval, arginfo_gmp_intval, 0, NULL)
	ZEND_RAW_FENTRY("gmp_strval", zif_gmp_strval, arginfo_gmp_strval, 0, NULL)
	ZEND_RAW_FENTRY("gmp_add", zif_gmp_add, arginfo_gmp_add, 0, NULL)
	ZEND_RAW_FENTRY("gmp_sub", zif_gmp_sub, arginfo_gmp_sub, 0, NULL)
	ZEND_RAW_FENTRY("gmp_mul", zif_gmp_mul, arginfo_gmp_mul, 0, NULL)
	ZEND_RAW_FENTRY("gmp_div_qr", zif_gmp_div_qr, arginfo_gmp_div_qr, 0, NULL)
	ZEND_RAW_FENTRY("gmp_div_q", zif_gmp_div_q, arginfo_gmp_div_q, 0, NULL)
	ZEND_RAW_FENTRY("gmp_div_r", zif_gmp_div_r, arginfo_gmp_div_r, 0, NULL)
	ZEND_RAW_FENTRY("gmp_div", zif_gmp_div_q, arginfo_gmp_div, 0, NULL)
	ZEND_RAW_FENTRY("gmp_mod", zif_gmp_mod, arginfo_gmp_mod, 0, NULL)
	ZEND_RAW_FENTRY("gmp_divexact", zif_gmp_divexact, arginfo_gmp_divexact, 0, NULL)
	ZEND_RAW_FENTRY("gmp_neg", zif_gmp_neg, arginfo_gmp_neg, 0, NULL)
	ZEND_RAW_FENTRY("gmp_abs", zif_gmp_abs, arginfo_gmp_abs, 0, NULL)
	ZEND_RAW_FENTRY("gmp_fact", zif_gmp_fact, arginfo_gmp_fact, 0, NULL)
	ZEND_RAW_FENTRY("gmp_sqrt", zif_gmp_sqrt, arginfo_gmp_sqrt, 0, NULL)
	ZEND_RAW_FENTRY("gmp_sqrtrem", zif_gmp_sqrtrem, arginfo_gmp_sqrtrem, 0, NULL)
	ZEND_RAW_FENTRY("gmp_root", zif_gmp_root, arginfo_gmp_root, 0, NULL)
	ZEND_RAW_FENTRY("gmp_rootrem", zif_gmp_rootrem, arginfo_gmp_rootrem, 0, NULL)
	ZEND_RAW_FENTRY("gmp_pow", zif_gmp_pow, arginfo_gmp_pow, 0, NULL)
	ZEND_RAW_FENTRY("gmp_powm", zif_gmp_powm, arginfo_gmp_powm, 0, NULL)
	ZEND_RAW_FENTRY("gmp_perfect_square", zif_gmp_perfect_square, arginfo_gmp_perfect_square, 0, NULL)
	ZEND_RAW_FENTRY("gmp_perfect_power", zif_gmp_perfect_power, arginfo_gmp_perfect_power, 0, NULL)
	ZEND_RAW_FENTRY("gmp_prob_prime", zif_gmp_prob_prime, arginfo_gmp_prob_prime, 0, NULL)
	ZEND_RAW_FENTRY("gmp_gcd", zif_gmp_gcd, arginfo_gmp_gcd, 0, NULL)
	ZEND_RAW_FENTRY("gmp_gcdext", zif_gmp_gcdext, arginfo_gmp_gcdext, 0, NULL)
	ZEND_RAW_FENTRY("gmp_lcm", zif_gmp_lcm, arginfo_gmp_lcm, 0, NULL)
	ZEND_RAW_FENTRY("gmp_invert", zif_gmp_invert, arginfo_gmp_invert, 0, NULL)
	ZEND_RAW_FENTRY("gmp_jacobi", zif_gmp_jacobi, arginfo_gmp_jacobi, 0, NULL)
	ZEND_RAW_FENTRY("gmp_legendre", zif_gmp_legendre, arginfo_gmp_legendre, 0, NULL)
	ZEND_RAW_FENTRY("gmp_kronecker", zif_gmp_kronecker, arginfo_gmp_kronecker, 0, NULL)
	ZEND_RAW_FENTRY("gmp_cmp", zif_gmp_cmp, arginfo_gmp_cmp, 0, NULL)
	ZEND_RAW_FENTRY("gmp_sign", zif_gmp_sign, arginfo_gmp_sign, 0, NULL)
	ZEND_RAW_FENTRY("gmp_random_seed", zif_gmp_random_seed, arginfo_gmp_random_seed, 0, NULL)
	ZEND_RAW_FENTRY("gmp_random_bits", zif_gmp_random_bits, arginfo_gmp_random_bits, 0, NULL)
	ZEND_RAW_FENTRY("gmp_random_range", zif_gmp_random_range, arginfo_gmp_random_range, 0, NULL)
	ZEND_RAW_FENTRY("gmp_and", zif_gmp_and, arginfo_gmp_and, 0, NULL)
	ZEND_RAW_FENTRY("gmp_or", zif_gmp_or, arginfo_gmp_or, 0, NULL)
	ZEND_RAW_FENTRY("gmp_com", zif_gmp_com, arginfo_gmp_com, 0, NULL)
	ZEND_RAW_FENTRY("gmp_xor", zif_gmp_xor, arginfo_gmp_xor, 0, NULL)
	ZEND_RAW_FENTRY("gmp_setbit", zif_gmp_setbit, arginfo_gmp_setbit, 0, NULL)
	ZEND_RAW_FENTRY("gmp_clrbit", zif_gmp_clrbit, arginfo_gmp_clrbit, 0, NULL)
	ZEND_RAW_FENTRY("gmp_testbit", zif_gmp_testbit, arginfo_gmp_testbit, 0, NULL)
	ZEND_RAW_FENTRY("gmp_scan0", zif_gmp_scan0, arginfo_gmp_scan0, 0, NULL)
	ZEND_RAW_FENTRY("gmp_scan1", zif_gmp_scan1, arginfo_gmp_scan1, 0, NULL)
	ZEND_RAW_FENTRY("gmp_popcount", zif_gmp_popcount, arginfo_gmp_popcount, 0, NULL)
	ZEND_RAW_FENTRY("gmp_hamdist", zif_gmp_hamdist, arginfo_gmp_hamdist, 0, NULL)
	ZEND_RAW_FENTRY("gmp_nextprime", zif_gmp_nextprime, arginfo_gmp_nextprime, 0, NULL)
	ZEND_RAW_FENTRY("gmp_binomial", zif_gmp_binomial, arginfo_gmp_binomial, 0, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_GMP_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_GMP___construct, arginfo_class_GMP___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("__serialize", zim_GMP___serialize, arginfo_class_GMP___serialize, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("__unserialize", zim_GMP___unserialize, arginfo_class_GMP___unserialize, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static void register_gmp_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("GMP_ROUND_ZERO", GMP_ROUND_ZERO, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_ROUND_PLUSINF", GMP_ROUND_PLUSINF, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_ROUND_MINUSINF", GMP_ROUND_MINUSINF, CONST_PERSISTENT);
#if defined(mpir_version)
	REGISTER_STRING_CONSTANT("GMP_MPIR_VERSION", GMP_MPIR_VERSION_STRING, CONST_PERSISTENT);
#endif
	REGISTER_STRING_CONSTANT("GMP_VERSION", GMP_VERSION_STRING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_MSW_FIRST", GMP_MSW_FIRST, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_LSW_FIRST", GMP_LSW_FIRST, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_LITTLE_ENDIAN", GMP_LITTLE_ENDIAN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_BIG_ENDIAN", GMP_BIG_ENDIAN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GMP_NATIVE_ENDIAN", GMP_NATIVE_ENDIAN, CONST_PERSISTENT);
}

static zend_class_entry *register_class_GMP(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "GMP", class_GMP_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}
