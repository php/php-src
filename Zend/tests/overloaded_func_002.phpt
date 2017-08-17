--TEST--
Overloaded function 002
--SKIPIF--
<?php
if (!class_exists('_ZendTestClass')) die("skip needs class with overloaded function");
?>
--FILE--
<?php
$a = new _ZendTestClass();
var_dump($a->{trim(" test")}());
?>
--EXPECT--
string(4) "test"
