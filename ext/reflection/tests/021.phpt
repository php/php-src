--TEST--
ReflectionClass::hasConstant
--FILE--
<?php
class Foo {
    const c1 = 1;
}
$class = new ReflectionClass("Foo");
var_dump($class->hasConstant("c1"));
var_dump($class->hasConstant("c2"));
?>
--EXPECT--
bool(true)
bool(false)
