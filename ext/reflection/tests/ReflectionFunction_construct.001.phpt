--TEST--
ReflectionFunction constructor errors
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php

$a = new ReflectionFunction(array(1, 2, 3));
try {
	$a = new ReflectionFunction('nonExistentFunction');
} catch (Exception $e) {
	echo $e->getMessage();
}
$a = new ReflectionFunction();
$a = new ReflectionFunction(1, 2);
?>
--EXPECTF--
Warning: ReflectionFunction::__construct() expects parameter 1 to be string, array given in %s on line %d
Function nonExistentFunction() does not exist
Warning: ReflectionFunction::__construct() expects exactly 1 parameter, 0 given in %s on line %d

Warning: ReflectionFunction::__construct() expects exactly 1 parameter, 2 given in %s on line %d
