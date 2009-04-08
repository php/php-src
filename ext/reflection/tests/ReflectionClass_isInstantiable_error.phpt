--TEST--
ReflectionClass::IsInstantiable()
--FILE--
<?php
class privateCtorOld {
	private function privateCtorOld() {}
}
$reflectionClass = new ReflectionClass("privateCtorOld");

var_dump($reflectionClass->IsInstantiable('X'));
var_dump($reflectionClass->IsInstantiable(0, null));

?>
--EXPECTF--
Warning: Wrong parameter count for ReflectionClass::isInstantiable() in %s on line 7
NULL

Warning: Wrong parameter count for ReflectionClass::isInstantiable() in %s on line 8
NULL
