--TEST--
Overloaded function 001
--SKIPIF--
<?php
if (!extension_loaded('zend-test')) die('skip zend-test extension not loaded');
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
