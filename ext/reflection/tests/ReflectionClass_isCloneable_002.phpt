--TEST--
Testing ReflectionClass::isCloneable() with non instantiable objects
--FILE--
<?php

trait foo {
}
$obj = new ReflectionClass('foo');
var_dump($obj->isCloneable());

abstract class bar {
}
$obj = new ReflectionClass('bar');
var_dump($obj->isCloneable());

interface baz {
}
$obj = new ReflectionClass('baz');
var_dump($obj->isCloneable());

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
