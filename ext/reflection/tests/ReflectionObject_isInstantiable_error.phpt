--TEST--
ReflectionObject::IsInstantiable() - invalid params
--FILE--
<?php
class privateCtorOld {
	private function privateCtorOld() {}
	public static function reflectionObjectFactory() {
		return new ReflectionObject(new self);
	}	
}
$reflectionObject =  privateCtorOld::reflectionObjectFactory();

var_dump($reflectionObject->IsInstantiable('X'));
var_dump($reflectionObject->IsInstantiable(0, null));

?>
--EXPECTF--
Warning: ReflectionClass::isInstantiable() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::isInstantiable() expects exactly 0 parameters, 2 given in %s on line %d
NULL
