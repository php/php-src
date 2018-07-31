--TEST--
Overloaded function 002
--SKIPIF--
<?php
if (!extension_loaded('zend-test')) die('skip zend-test extension not loaded');
?>
--FILE--
<?php
$a = new _ZendTestClass();
var_dump($a->{trim(" test")}());
?>
--EXPECT--
string(4) "test"
