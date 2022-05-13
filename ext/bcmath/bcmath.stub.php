<?php

/** @generate-class-entries */

/** @refcount 1 */
function bcadd(string $num1, string $num2, ?int $scale = null): string {}

/** @refcount 1 */
function bcsub(string $num1, string $num2, ?int $scale = null): string {}

/** @refcount 1 */
function bcmul(string $num1, string $num2, ?int $scale = null): string {}

/** @refcount 1 */
function bcdiv(string $num1, string $num2, ?int $scale = null): string {}

/** @refcount 1 */
function bcmod(string $num1, string $num2, ?int $scale = null): string {}

/** @refcount 1 */
function bcpowmod(string $num, string $exponent, string $modulus, ?int $scale = null): string {}

/** @refcount 1 */
function bcpow(string $num, string $exponent, ?int $scale = null): string {}

/** @refcount 1 */
function bcsqrt(string $num, ?int $scale = null): string {}

function bccomp(string $num1, string $num2, ?int $scale = null): int {}

function bcscale(?int $scale = null): int {}
