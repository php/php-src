--TEST--
Bug #70173 (ZVAL_COPY_VALUE_EX broken for 32bit Solaris Sparc)
--PLATFORM--
bits: 32
--FILE--
<?php
$var = 2900000000;
var_dump($var);
?>
--EXPECT--
float(2900000000)
