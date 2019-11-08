<?php

function bcadd(string $left_operand, string $right_operand, int $scale = UNKNOWN) : string {}

function bcsub(string $left_operand, string $right_operand, int $scale = UNKNOWN) : string {}

function bcmul(string $left_operand, string $right_operand, int $scale = UNKNOWN) : string {}

function bcdiv(string $dividend, string $divisor, int $scale = UNKNOWN) : string {}

function bcmod(string $dividend, string $divisor, int $scale = UNKNOWN) : string {}

function bcpowmod(string $base, string $exponent, string $modulus, int $scale = UNKNOWN): string|false {}

function bcpow(string $base, string $exponent, int $scale = UNKNOWN) : string {}

function bcsqrt(string $operand, int $scale = UNKNOWN) : string {}

function bccomp(string $left_operand, string $right_operand, int $scale = UNKNOWN) : int {}

function bcscale(int $scale = UNKNOWN) : int {}
