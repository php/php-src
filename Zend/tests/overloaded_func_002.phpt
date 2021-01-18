--TEST--
Overloaded function 002
--SKIPIF--
<?php
if (!extension_loaded('zend_test')) die('skip zend_test extension not loaded');
?>
--FILE--
<?php
$a = new _ZendTestClass();
var_dump($a->{trim(" test")}());
?>
--EXPECT--
string(4) "test"
