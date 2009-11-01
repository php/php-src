--TEST--
Bug #49719 (ReflectionClass::hasProperty returns true for a private property in base class)
--FILE--
<?php

class A {
	private $a;
}
class B extends A {
	private $b;
}

try {
	$b = new B;
	$ref = new ReflectionClass($b);
	
	var_dump(property_exists('b', 'a'));
	var_dump(property_exists($b, 'a'));
	var_dump($ref->hasProperty('a'));	
	var_dump($ref->getProperty('a'));
} catch (Exception $e) {
	var_dump($e->getMessage());
}

?>
--EXPECTF--
bool(false)
bool(false)
bool(false)
%string|unicode%(25) "Property a does not exist"
