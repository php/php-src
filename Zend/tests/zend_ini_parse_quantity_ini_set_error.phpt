--TEST--
Test ini_set() with invalid quantity
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(ini_set("zend_test.quantity_value", "1MB"));
var_dump(ini_get("zend_test.quantity_value"));
--EXPECTF--
Warning: Invalid "zend_test.quantity_value" setting. Invalid quantity "1MB": unknown multiplier "B", interpreting as "1" for backwards compatibility in %s on line %d
string(1) "0"
string(3) "1MB"
