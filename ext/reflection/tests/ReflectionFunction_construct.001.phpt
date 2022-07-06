--TEST--
ReflectionFunction constructor errors
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php

try {
    $a = new ReflectionFunction(array(1, 2, 3));
    echo "exception not thrown.".PHP_EOL;
} catch (TypeError $re) {
    echo "Ok - ".$re->getMessage().PHP_EOL;
}
try {
    $a = new ReflectionFunction('nonExistentFunction');
} catch (ReflectionException $e) {
    echo $e->getMessage().PHP_EOL;
}
try {
    $a = new ReflectionFunction();
} catch (TypeError $re) {
    echo "Ok - ".$re->getMessage().PHP_EOL;
}
try {
    $a = new ReflectionFunction(1, 2);
} catch (TypeError $re) {
    echo "Ok - ".$re->getMessage().PHP_EOL;
}
try {
    $a = new ReflectionFunction([]);
} catch (TypeError $re) {
    echo "Ok - ".$re->getMessage().PHP_EOL;
}

?>
--EXPECT--
Ok - ReflectionFunction::__construct(): Argument #1 ($function) must be of type Closure|string, array given
Function nonExistentFunction() does not exist
Ok - ReflectionFunction::__construct() expects exactly 1 argument, 0 given
Ok - ReflectionFunction::__construct() expects exactly 1 argument, 2 given
Ok - ReflectionFunction::__construct(): Argument #1 ($function) must be of type Closure|string, array given
