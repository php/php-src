/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_gmp_init, 0, 1, GMP, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, number)
	ZEND_ARG_TYPE_INFO(0, base, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_gmp_import, 0, 1, GMP, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, word_size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gmp_export, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, gmpnumber)
	ZEND_ARG_TYPE_INFO(0, word_size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_intval, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, gmpnumber)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gmp_strval, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, gmpnumber)
	ZEND_ARG_TYPE_INFO(0, base, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_gmp_add, 0, 2, GMP, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

#define arginfo_gmp_sub arginfo_gmp_add

#define arginfo_gmp_mul arginfo_gmp_add

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gmp_div_qr, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
	ZEND_ARG_TYPE_INFO(0, round, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_gmp_div_q, 0, 2, GMP, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
	ZEND_ARG_TYPE_INFO(0, round, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_gmp_div_r arginfo_gmp_div_q

#define arginfo_gmp_div arginfo_gmp_div_q

#define arginfo_gmp_mod arginfo_gmp_add

#define arginfo_gmp_divexact arginfo_gmp_add

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_gmp_neg, 0, 1, GMP, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

#define arginfo_gmp_abs arginfo_gmp_neg

#define arginfo_gmp_fact arginfo_gmp_neg

#define arginfo_gmp_sqrt arginfo_gmp_neg

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gmp_sqrtrem, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_gmp_root, 0, 2, GMP, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_TYPE_INFO(0, nth, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gmp_rootrem, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_TYPE_INFO(0, nth, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_gmp_pow, 0, 2, GMP, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, base)
	ZEND_ARG_TYPE_INFO(0, exp, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_gmp_powm, 0, 3, GMP, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, base)
	ZEND_ARG_INFO(0, exp)
	ZEND_ARG_INFO(0, mod)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_perfect_square, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

#define arginfo_gmp_perfect_power arginfo_gmp_perfect_square

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gmp_prob_prime, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_TYPE_INFO(0, reps, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_gmp_gcd arginfo_gmp_add

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gmp_gcdext, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

#define arginfo_gmp_lcm arginfo_gmp_add

#define arginfo_gmp_invert arginfo_gmp_add

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gmp_jacobi, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

#define arginfo_gmp_legendre arginfo_gmp_jacobi

#define arginfo_gmp_kronecker arginfo_gmp_jacobi

#define arginfo_gmp_cmp arginfo_gmp_jacobi

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gmp_sign, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_random_seed, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_INFO(0, seed)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_gmp_random_bits, 0, 1, GMP, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, bits, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_gmp_random_range, 0, 2, GMP, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, max)
ZEND_END_ARG_INFO()

#define arginfo_gmp_and arginfo_gmp_add

#define arginfo_gmp_or arginfo_gmp_add

#define arginfo_gmp_com arginfo_gmp_neg

#define arginfo_gmp_xor arginfo_gmp_add

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_setbit, 0, 2, _IS_BOOL, 1)
	ZEND_ARG_OBJ_INFO(0, a, GMP, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, set_clear, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_clrbit, 0, 2, _IS_BOOL, 1)
	ZEND_ARG_OBJ_INFO(0, a, GMP, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gmp_testbit, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gmp_scan0, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_gmp_scan1 arginfo_gmp_scan0

#define arginfo_gmp_popcount arginfo_gmp_sign

#define arginfo_gmp_hamdist arginfo_gmp_jacobi

#define arginfo_gmp_nextprime arginfo_gmp_neg

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_gmp_binomial, 0, 2, GMP, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_TYPE_INFO(0, b, IS_LONG, 0)
ZEND_END_ARG_INFO()
