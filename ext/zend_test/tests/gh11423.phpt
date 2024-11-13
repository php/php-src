--TEST--
GH-11423 (internal constants have their namespace lowercased)
--EXTENSIONS--
zend_test
--FILE--
<?php

$re = new \ReflectionExtension('zend_test');
var_dump($re->getConstants());

define('NS1\ns2\Const1','value1');
var_dump(get_defined_constants(true)["user"]);

?>
--EXPECT--
array(4) {
  ["ZEND_TEST_DEPRECATED"]=>
  int(42)
  ["ZEND_CONSTANT_A"]=>
  string(6) "global"
  ["ZendTestNS2\ZEND_CONSTANT_A"]=>
  string(10) "namespaced"
  ["ZendTestNS2\ZendSubNS\ZEND_CONSTANT_A"]=>
  string(10) "namespaced"
}
array(1) {
  ["NS1\ns2\Const1"]=>
  string(6) "value1"
}
