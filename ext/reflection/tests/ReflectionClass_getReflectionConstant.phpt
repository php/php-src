--TEST--
Test ReflectionClass::getReflectionConstant method
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--FILE--
<?php
$refleClass = new ReflectionClass(ReflectionClass::class);
var_dump(gettype($refleClass->getReflectionConstant('IS_IMPLICIT_ABSTRACT')));
var_dump($refleClass->getReflectionConstant('FOO_BAR'));
?>
--EXPECT--
string(6) "object"
bool(false)
