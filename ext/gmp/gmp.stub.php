<?php

/** @generate-function-entries */

class GMP
{
}

function gmp_init(int|string $number, int $base = 0): GMP {}

function gmp_import(string $data, int $word_size = 1, int $options = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN): GMP {}

function gmp_export(GMP|int|string $gmpnumber, int $word_size = 1, int $options = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN): string {}

function gmp_intval(GMP|int|string $gmpnumber): int {}

function gmp_strval(GMP|int|string $gmpnumber, int $base = 10): string {}

function gmp_add(GMP|int|string $a, GMP|int|string $b): GMP {}

function gmp_sub(GMP|int|string $a, GMP|int|string $b): GMP {}

function gmp_mul(GMP|int|string $a, GMP|int|string $b): GMP {}

function gmp_div_qr(GMP|int|string $a, GMP|int|string $b, int $round = GMP_ROUND_ZERO): array {}

function gmp_div_q(GMP|int|string $a, GMP|int|string $b, int $round = GMP_ROUND_ZERO): GMP {}

function gmp_div_r(GMP|int|string $a, GMP|int|string $b, int $round = GMP_ROUND_ZERO): GMP {}

/** @alias gmp_div_q */
function gmp_div(GMP|int|string $a, GMP|int|string $b, int $round = GMP_ROUND_ZERO): GMP {}

function gmp_mod(GMP|int|string $a, GMP|int|string $b): GMP {}

function gmp_divexact(GMP|int|string $a, GMP|int|string $b): GMP {}

function gmp_neg(GMP|int|string $a): GMP {}

function gmp_abs(GMP|int|string $a): GMP {}

function gmp_fact(GMP|int|string $a): GMP {}

function gmp_sqrt(GMP|int|string $a): GMP {}

function gmp_sqrtrem(GMP|int|string $a): array {}

function gmp_root(GMP|int|string $a, int $nth): GMP {}

function gmp_rootrem(GMP|int|string $a, int $nth): array {}

function gmp_pow(GMP|int|string $base, int $exp): GMP {}

function gmp_powm(GMP|int|string $base, GMP|int|string $exp, GMP|int|string $mod): GMP {}

function gmp_perfect_square(GMP|int|string $a): bool {}

function gmp_perfect_power(GMP|int|string $a): bool {}

function gmp_prob_prime(GMP|int|string $a, int $reps = 10): int {}

function gmp_gcd(GMP|int|string $a, GMP|int|string $b): GMP {}

function gmp_gcdext(GMP|int|string $a, GMP|int|string $b): array {}

function gmp_lcm(GMP|int|string $a, GMP|int|string $b): GMP {}

function gmp_invert(GMP|int|string $a, GMP|int|string $b): GMP|false {}

function gmp_jacobi(GMP|int|string $a, GMP|int|string $b): int {}

function gmp_legendre(GMP|int|string $a, GMP|int|string $b): int {}

function gmp_kronecker(GMP|int|string $a, GMP|int|string $b): int {}

function gmp_cmp(GMP|int|string $a, GMP|int|string $b): int {}

function gmp_sign(GMP|int|string $a): int {}

function gmp_random_seed(GMP|int|string $seed): void {}

function gmp_random_bits(int $bits): GMP {}

function gmp_random_range(GMP|int|string $min, GMP|int|string $max): GMP {}

function gmp_and(GMP|int|string $a, GMP|int|string $b): GMP {}

function gmp_or(GMP|int|string $a, GMP|int|string $b): GMP {}

function gmp_com(GMP|int|string $a): GMP {}

function gmp_xor(GMP|int|string $a, GMP|int|string $b): GMP {}

function gmp_setbit(GMP $a, int $index, bool $set_clear = true): void {}

function gmp_clrbit(GMP $a, int $index): void {}

function gmp_testbit(GMP|int|string $a, int $index): bool {}

function gmp_scan0(GMP|int|string $a, int $start): int {}

function gmp_scan1(GMP|int|string $a, int $start): int {}

function gmp_popcount(GMP|int|string $a): int {}

function gmp_hamdist(GMP|int|string $a, GMP|int|string $b): int {}

function gmp_nextprime(GMP|int|string $a): GMP {}

function gmp_binomial(GMP|int|string $a, int $b): GMP {}
