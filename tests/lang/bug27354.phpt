--TEST--
Bug #27354 (Modulus operator crashes PHP)
--FILE--
<?php
var_dump(-2147483647 % -1);
var_dump(-9_223_372_036_854_775_900 % -1); // Min value for 64bit int -1
var_dump(-2147483648 % -1);
var_dump(-2147483648 % -2);
?>
--EXPECT--
int(0)
int(0)
int(0)
int(0)
