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
$methodInfo = new ReflectionMethod();

echo "\nToo many arguments:\n";
$methodInfo = new ReflectionMethod("TestClass", "foo", true);

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

Warning: Wrong parameter count for ReflectionMethod::__construct() in %s on line %d

Too many arguments:

Warning: Wrong parameter count for ReflectionMethod::__construct() in %s on line %d
Ok - Class InvalidClassName does not exist
Ok - The parameter class is expected to be either a string or an object
Ok - ReflectionMethod::__construct() expects parameter 2 to be string, array given