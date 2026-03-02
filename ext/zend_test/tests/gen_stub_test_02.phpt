--TEST--
gen_stub.php: constants
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(ZEND_CONSTANT_A);
var_dump(ZendTestNS2\ZEND_CONSTANT_A);
var_dump(ZendTestNS2\ZendSubNS\ZEND_CONSTANT_A);

?>
--EXPECT--
string(6) "global"
string(10) "namespaced"
string(10) "namespaced"
