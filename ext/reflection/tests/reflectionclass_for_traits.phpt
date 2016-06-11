--TEST--
Tests some parts of ReflectionClass behavior for traits
--FILE--
<?php

trait T {}

$r = new ReflectionClass('T');
var_dump(Reflection::getModifierNames($r->getModifiers()));
var_dump($r->isAbstract());
var_dump($r->isInstantiable());
var_dump($r->isCloneable());

?>
--EXPECT--
array(0) {
}
bool(false)
bool(false)
bool(false)
