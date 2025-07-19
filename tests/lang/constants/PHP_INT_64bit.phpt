--TEST--
Test PHP_INT_MIN, PHP_INT_MAX, PHP_INT_MAX_SAFE and PHP_INT_SIZE (64-bit)
--SKIPIF--
<?php if (PHP_INT_SIZE !== 8)
    die("skip this test is for 64-bit platforms only"); ?>
--FILE--
<?php

var_dump(PHP_INT_MIN);
var_dump(PHP_INT_MAX);
var_dump(PHP_INT_MAX_SAFE);
var_dump(PHP_INT_MAX_SAFE === (2 << 52)-1);
var_dump(PHP_INT_SIZE);

?>
--EXPECT--
int(-9223372036854775808)
int(9223372036854775807)
int(9007199254740991)
bool(true)
int(8)
