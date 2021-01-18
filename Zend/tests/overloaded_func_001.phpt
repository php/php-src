--TEST--
Overloaded function 001
--SKIPIF--
<?php
if (!extension_loaded('zend_test')) die('skip zend_test extension not loaded');
?>
--FILE--
<?php
$o = new _ZendTestChildClass();
var_dump($o->test());
var_dump(_ZendTestClass::test());
?>
--EXPECT--
string(4) "test"
string(4) "test"
