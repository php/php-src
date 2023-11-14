--TEST--
JIT INC: 021
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
function inc(int|float $x) {
    return ++$x;
}
function dec(int|float $x) {
    return --$x;
}
var_dump(inc(PHP_INT_MAX));
var_dump(inc(1.1));
var_dump(dec(PHP_INT_MIN));
var_dump(dec(1.1));
?>
--EXPECT--
float(9.223372036854776E+18)
float(2.1)
float(-9.223372036854776E+18)
float(0.10000000000000009)
