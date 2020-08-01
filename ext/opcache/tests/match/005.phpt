--TEST--
JIT ZEND_MATCH with const expression
--INI--
opcache.enable=1
opcache.enable_cli=1
; Disable optimizations, otherwise the MATCH instruction gets optimized away
opcache.optimization_level=0
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.jit=1205
--SKIPIF--
<?php require_once(__DIR__ . '/../skipif.inc'); ?>
--FILE--
<?php

function test($callable) {
    try {
       echo $callable() . "\n";
    } catch (\Throwable $e) {
       echo 'Uncaught ' . get_class($e) . ': ' . $e->getMessage() . "\n";
    }
}

test(fn() => match (3) {
    1 => 1,
    2 => 2,
    3 => 3,
    4 => 4,
    5 => 5,
    default => 'default'
});

test(fn() => match (6) {
    1 => 1,
    2 => 2,
    3 => 3,
    4 => 4,
    5 => 5,
    default => 'default'
});

test(fn() => match (3) {
    1 => 1,
    2 => 2,
    3 => 3,
    4 => 4,
    5 => 5,
});

test(fn() => match (6) {
    1 => 1,
    2 => 2,
    3 => 3,
    4 => 4,
    5 => 5,
});

test(fn() => match ('3') {
    1 => 1,
    2 => 2,
    3 => 3,
    4 => 4,
    5 => 5,
});

test(fn() => match (true) {
    1 => 1,
    2 => 2,
    3 => 3,
    4 => 4,
    5 => 5,
});

?>
--EXPECT--
3
default
3
Uncaught UnhandledMatchError: Unhandled match value of type int
Uncaught UnhandledMatchError: Unhandled match value of type string
Uncaught UnhandledMatchError: Unhandled match value of type bool
