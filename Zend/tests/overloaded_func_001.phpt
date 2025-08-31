--TEST--
Overloaded function 001
--EXTENSIONS--
zend_test
--FILE--
<?php
$o = new _ZendTestChildClass();
var_dump($o->test());
var_dump(_ZendTestClass::test());
?>
--EXPECT--
string(4) "test"
string(4) "test"
