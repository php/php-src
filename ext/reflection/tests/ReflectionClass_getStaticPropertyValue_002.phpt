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
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->getStaticPropertyValue());
} catch (Exception $e) {
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
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}


?>
--EXPECTF--
Warning: ReflectionClass::getStaticPropertyValue() expects at most 2 parameters, 3 given in %s on line 8
NULL

Warning: ReflectionClass::getStaticPropertyValue() expects at least 1 parameter, 0 given in %s on line 13
NULL
Class C does not have a property named 
string(3) "def"

Warning: ReflectionClass::getStaticPropertyValue() expects parameter 1 to be string, array given in %s on line 28
NULL
