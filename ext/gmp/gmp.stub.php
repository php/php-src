<?php

/** @generate-function-entries */

class GMP
{
}

/** @param int|bool|string $number */
function gmp_init($number, int $base = 0): GMP {}

function gmp_import(string $data, int $word_size = 1, int $options = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN): GMP {}

/** @param GMP|int|bool|string $gmpnumber */
function gmp_export($gmpnumber, int $word_size = 1, int $options = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN): string {}

/** @param GMP|int|bool|string $gmpnumber */
function gmp_intval($gmpnumber): int {}

/** @param GMP|int|bool|string $gmpnumber */
function gmp_strval($gmpnumber, int $base = 10): string {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_add($a, $b): GMP {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_sub($a, $b): GMP {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_mul($a, $b): GMP {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_div_qr($a, $b, int $round = GMP_ROUND_ZERO): array {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_div_q($a, $b, int $round = GMP_ROUND_ZERO): GMP {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_div_r($a, $b, int $round = GMP_ROUND_ZERO): GMP {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 * @alias gmp_div_q
 */
function gmp_div($a, $b, int $round = GMP_ROUND_ZERO): GMP {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_mod($a, $b): GMP {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_divexact($a, $b): GMP {}

/** @param GMP|int|bool|string $a */
function gmp_neg($a): GMP {}

/** @param GMP|int|bool|string $a */
function gmp_abs($a): GMP {}

/** @param GMP|int|bool|string $a */
function gmp_fact($a): GMP {}

/** @param GMP|int|bool|string $a */
function gmp_sqrt($a): GMP {}

/** @param GMP|int|bool|string $a */
function gmp_sqrtrem($a): array {}

/** @param GMP|int|bool|string $a */
function gmp_root($a, int $nth): GMP {}

/** @param GMP|int|bool|string $a */
function gmp_rootrem($a, int $nth): array {}

/** @param GMP|int|bool|string $base */
function gmp_pow($base, int $exp): GMP {}

/**
 * @param GMP|int|bool|string $base
 * @param GMP|int|bool|string $exp
 * @param GMP|int|bool|string $mod
 */
function gmp_powm($base, $exp, $mod): GMP {}

/** @param GMP|int|bool|string $a */
function gmp_perfect_square($a): bool {}

/** @param GMP|int|bool|string $a */
function gmp_perfect_power($a): bool {}

/** @param GMP|int|bool|string $a */
function gmp_prob_prime($a, int $reps = 10): int {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_gcd($a, $b): GMP {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_gcdext($a, $b): array {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_lcm($a, $b): GMP {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_invert($a, $b): GMP|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_jacobi($a, $b): int {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_legendre($a, $b): int {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_kronecker($a, $b): int {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_cmp($a, $b): int {}

/** @param GMP|int|bool|string $a */
function gmp_sign($a): int {}

/** @param GMP|int|bool|string $seed */
function gmp_random_seed($seed): void {}

function gmp_random_bits(int $bits): GMP {}

/**
 * @param GMP|int|bool|string $min
 * @param GMP|int|bool|string $max
 **/
function gmp_random_range($min, $max): GMP {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_and($a, $b): GMP {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_or($a, $b): GMP {}

/** @param GMP|int|bool|string $a */
function gmp_com($a): GMP {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_xor($a, $b): GMP {}

function gmp_setbit(GMP $a, int $index, bool $set_clear = true): void {}

function gmp_clrbit(GMP $a, int $index): void {}

/** @param GMP|int|bool|string $a */
function gmp_testbit($a, int $index): bool {}

/** @param GMP|int|bool|string $a */
function gmp_scan0($a, int $start): int {}

/** @param GMP|int|bool|string $a */
function gmp_scan1($a, int $start): int {}

/** @param GMP|int|bool|string $a */
function gmp_popcount($a): int {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_hamdist($a, $b): int {}

/** @param GMP|int|bool|string $a */
function gmp_nextprime($a): GMP {}

/** @param GMP|int|bool|string $a */
function gmp_binomial($a, int $b): GMP {}
