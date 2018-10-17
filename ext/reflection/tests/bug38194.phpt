--TEST--
Reflection Bug #38194 (ReflectionClass::isSubclassOf() returns TRUE for the class itself)
--FILE--
<?php
class ObjectOne { }

$objectClass = new ReflectionClass('ObjectOne');
var_dump($objectClass->isSubclassOf($objectClass));
?>
--EXPECT--
bool(false)
