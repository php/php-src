<?php

/**
 * @param int|bool|string $number
 * @return GMP|false
 */
function gmp_init($number, int $base = 0) {}

/** @return GMP|false */
function gmp_import(string $data, int $word_size = 1, int $options = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN) {}

/** @param GMP|int|bool|string $gmpnumber */
function gmp_export($gmpnumber, int $word_size = 1, int $options = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN): string|false {}

/** @param GMP|int|bool|string $gmpnumber */
function gmp_intval($gmpnumber): int {}

/**
 * @param GMP|int|bool|string $gmpnumber
 * @return GMP|false
 */
function gmp_strval($gmpnumber, int $base = 10) {}

    /**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 * @return GMP|false
 */
function gmp_add($a, $b) {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 * @return GMP|false
 */
function gmp_sub($a, $b) {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 * @return GMP|false
 */
function gmp_mul($a, $b) {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 */
function gmp_div_qr($a, $b, int $round = GMP_ROUND_ZERO): array|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 * @return GMP|false
 */
function gmp_div_q($a, $b, int $round = GMP_ROUND_ZERO) {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 * @return GMP|false
 */
function gmp_div_r($a, $b, int $round = GMP_ROUND_ZERO) {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 * @return GMP|false
 */
function gmp_div($a, $b, int $round = GMP_ROUND_ZERO) {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 * @return GMP|false
 */
function gmp_mod($a, $b) {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 * @return GMP|false
 */
function gmp_divexact($a, $b) {}

/**
 * @param GMP|int|bool|string $a
 * @return GMP|false
 */
function gmp_neg($a) {}

/**
 * @param GMP|int|bool|string $a
 * @return GMP|false
 */
function gmp_abs($a) {}

/**
 * @param GMP|int $a
 * @return GMP|false
 */
function gmp_fact($a) {}

/**
 * @param GMP|int|bool|string $a
 * @return GMP|false
 */
function gmp_sqrt($a) {}

/** @param GMP|int|bool|string $a */
function gmp_sqrtrem($a): array|false {}

/**
 * @param GMP|int|bool|string $a
 * @return GMP|false
 **/
function gmp_root($a, int $nth) {}

/** @param GMP|int|bool|string $a **/
function gmp_rootrem($a, int $nth): array|false {}

/**
 * @param GMP|int|bool|string $base
 * @return GMP|false
 **/
function gmp_pow($base, int $exp) {}

/**
 * @param GMP|int|bool|string $base
 * @param GMP|int|bool|string $exp
 * @param GMP|int|bool|string $mod
 * @return GMP|false
 **/
function gmp_powm($base, $exp, $mod) {}

/** @param GMP|int|bool|string $a **/
function gmp_perfect_square($a): bool {}

/** @param GMP|int|bool|string $a **/
function gmp_perfect_power($a): bool {}

/** @param GMP|int|bool|string $a */
function gmp_prob_prime($a, int $reps = 10): int|false {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 * @return GMP|false
 */
function gmp_gcd($a, $b) {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 * @return GMP
 */
function gmp_gcdext($a, $b) {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 * @return GMP|false
 */
function gmp_lcm($a, $b) {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 * @return GMP|false
 */
function gmp_invert($a, $b) {}

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

/** @return GMP|false */
function gmp_random_bits(int $bits) {}

/**
 * @param GMP|int|bool|string $min
 * @param GMP|int|bool|string $max
 * @return GMP|false
 **/
function gmp_random_range($min, $max) {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 * @return GMP|false
 */
function gmp_and($a, $b) {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 * @return GMP|false
 */
function gmp_or($a, $b) {}

/**
 * @param GMP|int|bool|string $a
 * @return GMP|false
 */
function gmp_com($a) {}

/**
 * @param GMP|int|bool|string $a
 * @param GMP|int|bool|string $b
 * @return GMP|false
 */
function gmp_xor($a, $b) {}

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

/**
 * @param GMP|int|bool|string $a
 * @return GMP|false
 */
function gmp_nextprime($a) {}

/**
 * @param GMP|int|bool|string $a
 * @return GMP|false
 */
function gmp_binomial($a, int $b) {}
