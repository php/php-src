<?php

/** @generate-function-entries */

class GMP
{
}

function gmp_init(int|string $num, int $base = 0): GMP {}

function gmp_import(string $data, int $word_size = 1, int $flags = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN): GMP {}

function gmp_export(GMP|int|string $num, int $word_size = 1, int $flags = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN): string {}

function gmp_intval(GMP|int|string $num): int {}

function gmp_strval(GMP|int|string $num, int $base = 10): string {}

function gmp_add(GMP|int|string $num1, GMP|int|string $num2): GMP {}

function gmp_sub(GMP|int|string $num1, GMP|int|string $num2): GMP {}

function gmp_mul(GMP|int|string $num1, GMP|int|string $num2): GMP {}

function gmp_div_qr(GMP|int|string $num1, GMP|int|string $num2, int $rounding_mode = GMP_ROUND_ZERO): array {}

function gmp_div_q(GMP|int|string $num1, GMP|int|string $num2, int $rounding_mode = GMP_ROUND_ZERO): GMP {}

function gmp_div_r(GMP|int|string $num1, GMP|int|string $num2, int $rounding_mode = GMP_ROUND_ZERO): GMP {}

/** @alias gmp_div_q */
function gmp_div(GMP|int|string $num1, GMP|int|string $num2, int $rounding_mode = GMP_ROUND_ZERO): GMP {}

function gmp_mod(GMP|int|string $num1, GMP|int|string $num2): GMP {}

function gmp_divexact(GMP|int|string $num1, GMP|int|string $num2): GMP {}

function gmp_neg(GMP|int|string $num): GMP {}

function gmp_abs(GMP|int|string $num): GMP {}

function gmp_fact(GMP|int|string $num): GMP {}

function gmp_sqrt(GMP|int|string $num): GMP {}

function gmp_sqrtrem(GMP|int|string $num): array {}

function gmp_root(GMP|int|string $num, int $nth): GMP {}

function gmp_rootrem(GMP|int|string $num, int $nth): array {}

function gmp_pow(GMP|int|string $num, int $exponent): GMP {}

function gmp_powm(GMP|int|string $num, GMP|int|string $exponent, GMP|int|string $modulus): GMP {}

function gmp_perfect_square(GMP|int|string $num): bool {}

function gmp_perfect_power(GMP|int|string $num): bool {}

function gmp_prob_prime(GMP|int|string $num, int $repetitions = 10): int {}

function gmp_gcd(GMP|int|string $num1, GMP|int|string $num2): GMP {}

function gmp_gcdext(GMP|int|string $num1, GMP|int|string $num2): array {}

function gmp_lcm(GMP|int|string $num1, GMP|int|string $num2): GMP {}

function gmp_invert(GMP|int|string $num1, GMP|int|string $num2): GMP|false {}

function gmp_jacobi(GMP|int|string $num1, GMP|int|string $num2): int {}

function gmp_legendre(GMP|int|string $num1, GMP|int|string $num2): int {}

function gmp_kronecker(GMP|int|string $num1, GMP|int|string $num2): int {}

function gmp_cmp(GMP|int|string $num1, GMP|int|string $num2): int {}

function gmp_sign(GMP|int|string $num): int {}

function gmp_random_seed(GMP|int|string $seed): void {}

function gmp_random_bits(int $bits): GMP {}

function gmp_random_range(GMP|int|string $min, GMP|int|string $max): GMP {}

function gmp_and(GMP|int|string $num1, GMP|int|string $num2): GMP {}

function gmp_or(GMP|int|string $num1, GMP|int|string $num2): GMP {}

function gmp_com(GMP|int|string $num): GMP {}

function gmp_xor(GMP|int|string $num1, GMP|int|string $num2): GMP {}

function gmp_setbit(GMP $num, int $index, bool $value = true): void {}

function gmp_clrbit(GMP $num, int $index): void {}

function gmp_testbit(GMP|int|string $num, int $index): bool {}

function gmp_scan0(GMP|int|string $num1, int $start): int {}

function gmp_scan1(GMP|int|string $num1, int $start): int {}

function gmp_popcount(GMP|int|string $num): int {}

function gmp_hamdist(GMP|int|string $num1, GMP|int|string $num2): int {}

function gmp_nextprime(GMP|int|string $num): GMP {}

function gmp_binomial(GMP|int|string $n, int $k): GMP {}
