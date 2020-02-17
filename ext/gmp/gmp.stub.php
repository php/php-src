<?php

/** @param int|bool|string $number */
function gmp_init($number, int $base = 0): GMP|false {}

function gmp_import(string $data, int $word_size = 1, int $options = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN): GMP|false {}

/** @param GMP|int|bool|string $gmpnumber */
function gmp_export($gmpnumber, int $word_size = 1, int $options = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN): string|false {}

/** @param GMP|int|bool|string $gmpnumber */
function gmp_intval($gmpnumber): int {}

/** @param GMP|int|bool|string $gmpnumber */
function gmp_strval($gmpnumber, int $base = 10): string|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_add($a, $b): GMP|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_sub($a, $b): GMP|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_mul($a, $b): GMP|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_div_qr($a, $b, int $round = GMP_ROUND_ZERO): array|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_div_q($a, $b, int $round = GMP_ROUND_ZERO): GMP|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_div_r($a, $b, int $round = GMP_ROUND_ZERO): GMP|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_div($a, $b, int $round = GMP_ROUND_ZERO): GMP|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_mod($a, $b): GMP|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_divexact($a, $b): GMP|false {}

/** @param GMP|int|bool|string $a */
function gmp_neg($a): GMP|false {}

/** @param GMP|int|bool|string $a */
function gmp_abs($a): GMP|false {}

/** @param GMP|int $a */
function gmp_fact($a): GMP|false {}

/** @param GMP|int|bool|string $a */
function gmp_sqrt($a): GMP|false {}

/** @param GMP|int|bool|string $a */
function gmp_sqrtrem($a): array|false {}

/** @param GMP|int|bool|string $a **/
function gmp_root($a, int $nth): GMP|false {}

/** @param GMP|int|bool|string $a **/
function gmp_rootrem($a, int $nth): array|false {}

/** @param GMP|int|bool|string $base **/
function gmp_pow($base, int $exp): GMP|false {}

/**
 * @param GMP|int|bool|string $base
 * @param GMP|int|bool|string $exp
 * @param GMP|int|bool|string $mod
 **/
function gmp_powm($base, $exp, $mod): GMP|false {}

/** @param GMP|int|bool|string $a **/
function gmp_perfect_square($a): bool {}

/** @param GMP|int|bool|string $a **/
function gmp_perfect_power($a): bool {}

/** @param GMP|int|bool|string $a */
function gmp_prob_prime($a, int $reps = 10): int|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_gcd($a, $b): GMP|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_gcdext($a, $b): array|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_lcm($a, $b): GMP|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_invert($a, $b): GMP|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_jacobi($a, $b): int|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_legendre($a, $b): int|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_kronecker($a, $b): int|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_cmp($a, $b): int|false {}

/** @param GMP|int|bool|string $a */
function gmp_sign($a): int|false {}

/** @param GMP|int|bool|string $seed */
function gmp_random_seed($seed): ?bool {}

function gmp_random_bits(int $bits): GMP|false {}

/**
 * @param GMP|int|bool|string $min
 * @param GMP|int|bool|string $max
 **/
function gmp_random_range($min, $max): GMP|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_and($a, $b): GMP|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_or($a, $b): GMP|false {}

/** @param GMP|int|bool|string $a */
function gmp_com($a): GMP|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_xor($a, $b): GMP|false {}

function gmp_setbit(GMP $a, int $index, bool $set_clear = true): ?bool {}

function gmp_clrbit(GMP $a, int $index): ?bool {}

/** @param GMP|int|bool|string $a */
function gmp_testbit($a, int $index): bool {}

/** @param GMP|int|bool|string $a */
function gmp_scan0($a, int $start): int|false {}

/** @param GMP|int|bool|string $a */
function gmp_scan1($a, int $start): int|false {}

/** @param GMP|int|bool|string $a */
function gmp_popcount($a): int|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_hamdist($a, $b): int|false {}

/** @param GMP|int|bool|string $a */
function gmp_nextprime($a): GMP|false {}

/** @param GMP|int|bool|string $a */
function gmp_binomial($a, int $b): GMP|false {}
