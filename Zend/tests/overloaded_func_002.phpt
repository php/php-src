--TEST--
Overloaded function 002
--SKIPF--
<?php
if (!PHP_DEBUG) die("skip only run in debug version");
?>
--FILE--
<?php
$a = new _ZendTestClass();
var_dump($a->{trim(" test")}());
?>
--EXPECT--
NULL
