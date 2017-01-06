--TEST--
Overloaded function 002
--SKIPIF--
<?php
if (!PHP_DEBUG) die("skip only run in debug version");
?>
--FILE--
<?php
$a = new _ZendTestClass();
var_dump($a->{trim(" test")}());
?>
--EXPECT--
string(4) "test"
