--TEST--
Test ini setting with invalid quantity
--EXTENSIONS--
zend_test
--INI--
zend_test.quantity_value=1MB
--FILE--
<?php

var_dump(ini_get("zend_test.quantity_value"));
?>
--EXPECTF--
Warning: Invalid "zend_test.quantity_value" setting. Invalid quantity "1MB": unknown multiplier "B", interpreting as "1" for backwards compatibility in %s on line %d
string(3) "1MB"
