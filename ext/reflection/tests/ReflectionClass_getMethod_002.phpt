--TEST--
ReflectionClass::getMethod() - error cases
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class C {
	function f() {}
}

$rc = new ReflectionClass("C");
echo "Check invalid params:\n";
try {
	var_dump($rc->getMethod());
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->getMethod("f", "f"));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->getMethod(null));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->getMethod(1));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->getMethod(1.5));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->getMethod(true));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->getMethod(array(1,2,3)));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rc->getMethod(new C));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}


?>
--EXPECTF--
Check invalid params:

Warning: ReflectionClass::getMethod() expects exactly 1 parameter, 0 given in %s on line 9
NULL

Warning: ReflectionClass::getMethod() expects exactly 1 parameter, 2 given in %s on line 14
NULL
Method  does not exist
Method 1 does not exist
Method 1.5 does not exist
Method 1 does not exist

Warning: ReflectionClass::getMethod() expects parameter 1 to be string, array given in %s on line 39
NULL

Warning: ReflectionClass::getMethod() expects parameter 1 to be string, object given in %s on line 44
NULL
