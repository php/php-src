--TEST--
Test parsing of quantities with null byte
--FILE--
<?php

var_dump(zend_test_zend_ini_parse_quantity(" 123\x00K"));
var_dump(zend_test_zend_ini_parse_quantity("\x00 123K"));
var_dump(zend_test_zend_ini_parse_quantity(" \x00123K"));
var_dump(zend_test_zend_ini_parse_quantity(" 123\x00K"));
var_dump(zend_test_zend_ini_parse_quantity(" 123K\x00"));
var_dump(zend_test_zend_ini_parse_quantity(" 123\x00"));

--EXPECTF--
Warning: Invalid quantity ' 123\x00K', interpreting as ' 123K' for backwards compatibility in %s on line %d
int(125952)

Warning: Invalid quantity '\x00 123K': no valid leading digits, interpreting as '0' for backwards compatibility in %s on line %d
int(0)

Warning: Invalid quantity ' \x00123K': no valid leading digits, interpreting as '0' for backwards compatibility in %s on line %d
int(0)

Warning: Invalid quantity ' 123\x00K', interpreting as ' 123K' for backwards compatibility in %s on line %d
int(125952)

Warning: Invalid quantity ' 123K\x00': unknown multipler '\x00', interpreting as ' 123' for backwards compatibility in %s on line %d
int(123)

Warning: Invalid quantity ' 123\x00': unknown multipler '\x00', interpreting as ' 123' for backwards compatibility in %s on line %d
int(123)
