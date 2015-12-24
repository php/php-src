--TEST--
Bug #71211 ReflectionClass->isAbstract() returns false for traits
--FILE--
<?php

trait Foo {}

$ref = new ReflectionClass('Foo');
var_dump($ref->isAbstract());
?>
--EXPECT--	
bool(true)
