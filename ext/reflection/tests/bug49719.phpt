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

class A2 {
	private $a = 1;
}

class B2 extends A2 {
	private $a = 2;
}

$b2 = new ReflectionClass('B2');
$prop = $b2->getProperty('a');
$prop->setAccessible(true);
var_dump($prop->getValue(new b2));

?>
--EXPECTF--
bool(false)
bool(false)
bool(false)
string(25) "Property a does not exist"
int(2)
