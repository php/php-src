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
} catch (ArgumentCountError $re) {
    echo "Ok - ".$re->getMessage().PHP_EOL;
}
try {
    echo "\nToo many arguments:\n";
    $methodInfo = new ReflectionMethod("TestClass", "foo", true);
} catch (ArgumentCountError $re) {
    echo "Ok - ".$re->getMessage().PHP_EOL;
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
} catch (TypeError $re) {
    echo "Ok - ".$re->getMessage().PHP_EOL;
}

try{
    //invalid 2nd param
    $methodInfo = new ReflectionMethod("TestClass", []);
} catch (TypeError $re) {
    echo "Ok - ".$re->getMessage().PHP_EOL;
}

?>
--EXPECT--
Too few arguments:
Ok - ReflectionMethod::__construct() expects at least 1 argument, 0 given

Too many arguments:
Ok - ReflectionMethod::__construct() expects at most 2 arguments, 3 given
Ok - Class "InvalidClassName" does not exist
Ok - ReflectionMethod::__construct(): Argument #1 ($objectOrMethod) must be of type object|string, array given
Ok - ReflectionMethod::__construct(): Argument #2 ($method) must be of type ?string, array given
