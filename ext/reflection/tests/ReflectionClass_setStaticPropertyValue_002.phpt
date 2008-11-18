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
	var_dump($rc->setStaticPropertyValue("x", "default value", 'blah'));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->setStaticPropertyValue());
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->setStaticPropertyValue(null));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->setStaticPropertyValue(null,null));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->setStaticPropertyValue(1.5, 'def'));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->setStaticPropertyValue(array(1,2,3), 'blah'));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}


?>
--EXPECTF--

Warning: ReflectionClass::setStaticPropertyValue() expects exactly 2 parameters, 3 given in %s on line 8
NULL

Warning: ReflectionClass::setStaticPropertyValue() expects exactly 2 parameters, 0 given in %s on line 13
NULL

Warning: ReflectionClass::setStaticPropertyValue() expects exactly 2 parameters, 1 given in %s on line 18
NULL
Class C does not have a property named 
Class C does not have a property named 1.5

Warning: ReflectionClass::setStaticPropertyValue() expects parameter 1 to be string, array given in %s on line 33
NULL