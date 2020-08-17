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

echo "Too few arguments:\n";

try {
	$methodInfo = new ReflectionMethod();
} catch (Throwable $e) {
	var_dump($e->getMessage());
}

echo "\nToo many arguments:\n";

try {
	$methodInfo = new ReflectionMethod("TestClass", "foo", true);
} catch (Throwable $e) {
	var_dump($e->getMessage());
}

try {
	//invalid class
	$methodInfo = new ReflectionMethod("InvalidClassName", "foo");
} catch (ReflectionException $re) {
	echo "Ok - ".$re->getMessage().PHP_EOL;
}


try {
	//invalid 1st param
	$methodInfo = new ReflectionMethod([], "foo");
} catch (ReflectionException $re) {
	echo "Ok - ".$re->getMessage().PHP_EOL;
}

try{
	//invalid 2nd param
	$methodInfo = new ReflectionMethod("TestClass", []);
} catch (TypeError $re) {
	echo "Ok - ".$re->getMessage().PHP_EOL;
}

?>
--EXPECTF--
Too few arguments:
string(69) "ReflectionMethod::__construct() expects at least 1 parameter, 0 given"

Too many arguments:
string(69) "ReflectionMethod::__construct() expects at most 2 parameters, 3 given"
Ok - Class InvalidClassName does not exist
Ok - The parameter class is expected to be either a string or an object
Ok - ReflectionMethod::__construct() expects parameter 2 to be string, array given