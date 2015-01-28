--TEST--
Zend signed multiply 64-bit
--FILE--
<?php
var_dump(0x80000000 * -0xffffffff);
var_dump(0x80000001 * 0xfffffffe);
var_dump(0x80000001 * -0xffffffff);
?>
--EXPECTF--
int(-9223372034707292160)
int(9223372036854775806)
int(-9223372039002259455)