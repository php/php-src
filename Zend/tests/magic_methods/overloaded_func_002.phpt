--TEST--
Overloaded function 002
--EXTENSIONS--
zend_test
--FILE--
<?php
$a = new _ZendTestClass();
var_dump($a->{trim(" test")}());
?>
--EXPECT--
string(4) "test"
