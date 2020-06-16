<?php

/** @generate-function-entries */

function bcadd(string $left_operand, string $right_operand, ?int $scale = null): string {}

function bcsub(string $left_operand, string $right_operand, ?int $scale = null): string {}

function bcmul(string $left_operand, string $right_operand, ?int $scale = null): string {}

function bcdiv(string $dividend, string $divisor, ?int $scale = null): string {}

function bcmod(string $dividend, string $divisor, ?int $scale = null): string {}

function bcpowmod(string $base, string $exponent, string $modulus, ?int $scale = null): string|false {}

function bcpow(string $base, string $exponent, ?int $scale = null): string {}

function bcsqrt(string $operand, ?int $scale = null): string {}

function bccomp(string $left_operand, string $right_operand, ?int $scale = null): int {}

function bcscale(?int $scale = null): int {}
