--TEST--
Zend signed multiply 32-bit
--PLATFORM--
bits: 32
--FILE--
<?php
var_dump(0x8000 * -0xffff);
var_dump(0x8001 * 0xfffe);
var_dump(0x8001 * -0xffff);
?>
--EXPECT--
int(-2147450880)
int(2147483646)
float(-2147516415)
