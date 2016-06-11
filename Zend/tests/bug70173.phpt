--TEST--
Bug #70173 (ZVAL_COPY_VALUE_EX broken for 32bit Solaris Sparc)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platforms only");
?>
--FILE--
<?php
$var = 2900000000;
var_dump($var);
?>
--EXPECT--
float(2900000000)
