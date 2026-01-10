--TEST--
ReflectionClass::getConstant
--FILE--
<?php
class Foo {
    const c1 = 1;
}
$class = new ReflectionClass("Foo");
var_dump($class->getConstant("c1"));
var_dump($class->getConstant("c2"));
?>
--EXPECTF--
int(1)

Deprecated: ReflectionClass::getConstant() for a non-existent constant is deprecated, use ReflectionClass::hasConstant() to check if the constant exists in %s on line %d
bool(false)
