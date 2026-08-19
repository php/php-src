--TEST--
property_exists() and method_exists() with wrong-case class name fails with wrong case
--FILE--
<?php
class MyClass {
    public int $value = 1;
    public function myMethod(): void {}
}

var_dump(property_exists("MYCLASS", "value"));
var_dump(property_exists("myclass", "value"));
var_dump(method_exists("MYCLASS", "myMethod"));
var_dump(method_exists("myclass", "myMethod"));
?>
--EXPECTF--
bool(false)
bool(false)
bool(false)
bool(false)
