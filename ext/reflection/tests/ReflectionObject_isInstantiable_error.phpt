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
Warning: Wrong parameter count for ReflectionClass::isInstantiable() in %s on line 10
NULL

Warning: Wrong parameter count for ReflectionClass::isInstantiable() in %s on line 11
NULL
