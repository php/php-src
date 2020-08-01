--TEST--
JIT ZEND_MATCH with non-string and non-integer expression
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.jit=1205
--SKIPIF--
<?php require_once(__DIR__ . '/../skipif.inc'); ?>
--FILE--
<?php

function test($callable, ...$args) {
    try {
       echo $callable(...$args) . "\n";
    } catch (\Throwable $e) {
       echo 'Uncaught ' . get_class($e) . ': ' . $e->getMessage() . "\n";
    }
}

test(fn(bool $x) => match ($x) {
    1 => 1,
    2 => 2,
    3 => 3,
    4 => 4,
    5 => 5,
    default => 'default'
}, true);

test(fn(float $x) => match ($x) {
    1 => 1,
    2 => 2,
    3 => 3,
    4 => 4,
    5 => 5,
    default => 'default'
}, pi());

test(fn(bool|float $x) => match ($x) {
    1 => 1,
    2 => 2,
    3 => 3,
    4 => 4,
    5 => 5,
    default => 'default'
}, pi());

test(fn(bool $x) => match ($x) {
    1 => 1,
    2 => 2,
    3 => 3,
    4 => 4,
    5 => 5,
}, true);

test(fn(float $x) => match ($x) {
    1 => 1,
    2 => 2,
    3 => 3,
    4 => 4,
    5 => 5,
}, pi());

test(fn(bool|float $x) => match ($x) {
    1 => 1,
    2 => 2,
    3 => 3,
    4 => 4,
    5 => 5,
}, pi());

?>
--EXPECT--
default
default
default
Uncaught UnhandledMatchError: Unhandled match value of type bool
Uncaught UnhandledMatchError: Unhandled match value of type float
Uncaught UnhandledMatchError: Unhandled match value of type float
