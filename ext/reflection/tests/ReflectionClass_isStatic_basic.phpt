--TEST--
ReflectionClass::isStatic() method
--FILE--
<?php

class TestClass {}
static class TestStaticClass {}

$normalClass = new ReflectionClass('TestClass');
$staticClass = new ReflectionClass('TestStaticClass');

var_dump($normalClass->isStatic());
var_dump($staticClass->isStatic());

?>
--EXPECT--
bool(false)
bool(true)
