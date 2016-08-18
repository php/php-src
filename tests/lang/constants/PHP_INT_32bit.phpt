--TEST--
Test PHP_INT_MIN, PHP_INT_MAX and PHP_INT_SIZE (32-bit)
--SKIPIF--
<?php if (PHP_INT_SIZE !== 4)
    die("skip this test is for 32-bit platforms only"); ?>
--FILE--
<?php

var_dump(PHP_INT_MIN);
var_dump(PHP_INT_MAX);
var_dump(PHP_INT_SIZE);

?>
--EXPECT--
int(-2147483648)
int(2147483647)
int(4)
