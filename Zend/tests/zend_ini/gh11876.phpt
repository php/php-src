--TEST--
Invalid INI quantities, base prefix followed by stuff eaten by strtoull()
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(zend_test_zend_ini_parse_quantity('0x0x12'));

var_dump(zend_test_zend_ini_parse_quantity('0b+10'));
var_dump(zend_test_zend_ini_parse_quantity('0o+10'));
var_dump(zend_test_zend_ini_parse_quantity('0x+10'));

var_dump(zend_test_zend_ini_parse_quantity('0b 10'));
var_dump(zend_test_zend_ini_parse_quantity('0o 10'));
var_dump(zend_test_zend_ini_parse_quantity('0x 10'));

var_dump(zend_test_zend_ini_parse_quantity('0g10'));
var_dump(zend_test_zend_ini_parse_quantity('0m10'));
var_dump(zend_test_zend_ini_parse_quantity('0k10'));

--EXPECTF--
Warning: Invalid quantity "0x0x12": no digits after base prefix, interpreting as "0" for backwards compatibility in %s on line %d
int(0)

Warning: Invalid quantity "0b+10": no digits after base prefix, interpreting as "0" for backwards compatibility in %s on line %d
int(0)

Warning: Invalid quantity "0o+10": no digits after base prefix, interpreting as "0" for backwards compatibility in %s on line %d
int(0)

Warning: Invalid quantity "0x+10": no digits after base prefix, interpreting as "0" for backwards compatibility in %s on line %d
int(0)

Warning: Invalid quantity "0b 10": no digits after base prefix, interpreting as "0" for backwards compatibility in %s on line %d
int(0)

Warning: Invalid quantity "0o 10": no digits after base prefix, interpreting as "0" for backwards compatibility in %s on line %d
int(0)

Warning: Invalid quantity "0x 10": no digits after base prefix, interpreting as "0" for backwards compatibility in %s on line %d
int(0)

Warning: Invalid quantity "0g10": unknown multiplier "0", interpreting as "0" for backwards compatibility in %s on line %d
int(0)

Warning: Invalid quantity "0m10": unknown multiplier "0", interpreting as "0" for backwards compatibility in %s on line %d
int(0)

Warning: Invalid quantity "0k10": unknown multiplier "0", interpreting as "0" for backwards compatibility in %s on line %d
int(0)
