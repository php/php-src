--TEST--
Bug #27354 (Modulus operator crashes PHP)
--FILE--
<?php
var_dump(-2147483647 % -1);
var_dump(-9_223_372_036_860_776_000 % -1); // Min value for 64bit int -1
var_dump(-2147483648 % -1);
var_dump(-2147483648 % -2);
?>
--EXPECTF--
int(0)

Deprecated: Implicit conversion from float -9.223372036860776E+18 to int loses precision in %s on line %d
int(0)
int(0)
int(0)
