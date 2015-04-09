--TEST--
ReflectionMethod constructor errors
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php

class TestClass
{
    public function foo() {
    }
}


try {
	echo "Too few arguments:\n";
	$methodInfo = new ReflectionMethod();
} catch (Exception $e) {
	print $e->__toString();
}
try {
	echo "\nToo many arguments:\n";
	$methodInfo = new ReflectionMethod("TestClass", "foo", true);
} catch (Exception $e) {
	print $e->__toString();
}

?>
--EXPECTF--
Too few arguments:

Warning: ReflectionMethod::__construct() expects exactly 1 parameter, 0 given in %s on line 12

Too many arguments:

Warning: ReflectionMethod::__construct() expects exactly 1 parameter, 3 given in %s on line 18
