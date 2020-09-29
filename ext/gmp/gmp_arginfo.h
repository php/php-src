/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: a1eb4fd58c0b2155692611386c77035f1ef11c2c */

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


static const zend_function_entry ext_functions[] = {
	ZEND_FE(gmp_init, arginfo_gmp_init)
	ZEND_FE(gmp_import, arginfo_gmp_import)
	ZEND_FE(gmp_export, arginfo_gmp_export)
	ZEND_FE(gmp_intval, arginfo_gmp_intval)
	ZEND_FE(gmp_strval, arginfo_gmp_strval)
	ZEND_FE(gmp_add, arginfo_gmp_add)
	ZEND_FE(gmp_sub, arginfo_gmp_sub)
	ZEND_FE(gmp_mul, arginfo_gmp_mul)
	ZEND_FE(gmp_div_qr, arginfo_gmp_div_qr)
	ZEND_FE(gmp_div_q, arginfo_gmp_div_q)
	ZEND_FE(gmp_div_r, arginfo_gmp_div_r)
	ZEND_FALIAS(gmp_div, gmp_div_q, arginfo_gmp_div)
	ZEND_FE(gmp_mod, arginfo_gmp_mod)
	ZEND_FE(gmp_divexact, arginfo_gmp_divexact)
	ZEND_FE(gmp_neg, arginfo_gmp_neg)
	ZEND_FE(gmp_abs, arginfo_gmp_abs)
	ZEND_FE(gmp_fact, arginfo_gmp_fact)
	ZEND_FE(gmp_sqrt, arginfo_gmp_sqrt)
	ZEND_FE(gmp_sqrtrem, arginfo_gmp_sqrtrem)
	ZEND_FE(gmp_root, arginfo_gmp_root)
	ZEND_FE(gmp_rootrem, arginfo_gmp_rootrem)
	ZEND_FE(gmp_pow, arginfo_gmp_pow)
	ZEND_FE(gmp_powm, arginfo_gmp_powm)
	ZEND_FE(gmp_perfect_square, arginfo_gmp_perfect_square)
	ZEND_FE(gmp_perfect_power, arginfo_gmp_perfect_power)
	ZEND_FE(gmp_prob_prime, arginfo_gmp_prob_prime)
	ZEND_FE(gmp_gcd, arginfo_gmp_gcd)
	ZEND_FE(gmp_gcdext, arginfo_gmp_gcdext)
	ZEND_FE(gmp_lcm, arginfo_gmp_lcm)
	ZEND_FE(gmp_invert, arginfo_gmp_invert)
	ZEND_FE(gmp_jacobi, arginfo_gmp_jacobi)
	ZEND_FE(gmp_legendre, arginfo_gmp_legendre)
	ZEND_FE(gmp_kronecker, arginfo_gmp_kronecker)
	ZEND_FE(gmp_cmp, arginfo_gmp_cmp)
	ZEND_FE(gmp_sign, arginfo_gmp_sign)
	ZEND_FE(gmp_random_seed, arginfo_gmp_random_seed)
	ZEND_FE(gmp_random_bits, arginfo_gmp_random_bits)
	ZEND_FE(gmp_random_range, arginfo_gmp_random_range)
	ZEND_FE(gmp_and, arginfo_gmp_and)
	ZEND_FE(gmp_or, arginfo_gmp_or)
	ZEND_FE(gmp_com, arginfo_gmp_com)
	ZEND_FE(gmp_xor, arginfo_gmp_xor)
	ZEND_FE(gmp_setbit, arginfo_gmp_setbit)
	ZEND_FE(gmp_clrbit, arginfo_gmp_clrbit)
	ZEND_FE(gmp_testbit, arginfo_gmp_testbit)
	ZEND_FE(gmp_scan0, arginfo_gmp_scan0)
	ZEND_FE(gmp_scan1, arginfo_gmp_scan1)
	ZEND_FE(gmp_popcount, arginfo_gmp_popcount)
	ZEND_FE(gmp_hamdist, arginfo_gmp_hamdist)
	ZEND_FE(gmp_nextprime, arginfo_gmp_nextprime)
	ZEND_FE(gmp_binomial, arginfo_gmp_binomial)
	ZEND_FE_END
};


static const zend_function_entry class_GMP_methods[] = {
	ZEND_FE_END
};
