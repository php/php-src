--TEST--
Reflection Bug #38194 (ReflectionClass::isSubclassOf() returns TRUE for the class itself)
--FILE--
<?php
class Object { }
  
$objectClass= new ReflectionClass('Object');
var_dump($objectClass->isSubclassOf($objectClass));
?>
--EXPECT--
bool(false)
