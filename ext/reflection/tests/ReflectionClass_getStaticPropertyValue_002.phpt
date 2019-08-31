--TEST--
ReflectionClass::getStaticPropertyValue() - bad params
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class C {
	public static $x;
}

$rc = new ReflectionClass('C');
try {
	var_dump($rc->getStaticPropertyValue("x", "default value", 'blah'));
} catch (TypeError $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->getStaticPropertyValue());
} catch (TypeError $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->getStaticPropertyValue(null));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->getStaticPropertyValue(1.5, 'def'));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->getStaticPropertyValue(array(1,2,3)));
} catch (TypeError $e) {
	echo $e->getMessage() . "\n";
}


?>
--EXPECT--
ReflectionClass::getStaticPropertyValue() expects at most 2 parameters, 3 given
ReflectionClass::getStaticPropertyValue() expects at least 1 parameter, 0 given
Class C does not have a property named 
string(3) "def"
ReflectionClass::getStaticPropertyValue() expects parameter 1 to be string, array given
