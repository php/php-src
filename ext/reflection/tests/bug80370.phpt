--TEST--
Bug #80370: Segfault on ReflectionProperty::getAttributes of dynamic property
--FILE--
<?php
class Foobar {

}

$foobar = new Foobar();
$foobar->bar = 42;

$reflectionObject = new ReflectionObject($foobar);
$reflectionProperty = $reflectionObject->getProperty('bar');
var_dump($reflectionProperty->getAttributes());
--EXPECT--
array(0) {
}
