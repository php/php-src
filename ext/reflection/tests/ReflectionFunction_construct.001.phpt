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
    echo $re::class, ': ', $re->getMessage(), PHP_EOL;
}
try {
    $a = new ReflectionFunction('nonExistentFunction');
} catch (ReflectionException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $a = new ReflectionFunction();
} catch (TypeError $re) {
    echo $re::class, ': ', $re->getMessage(), PHP_EOL;
}
try {
    $a = new ReflectionFunction(1, 2);
} catch (TypeError $re) {
    echo $re::class, ': ', $re->getMessage(), PHP_EOL;
}
try {
    $a = new ReflectionFunction([]);
} catch (TypeError $re) {
    echo $re::class, ': ', $re->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: ReflectionFunction::__construct(): Argument #1 ($function) must be of type Closure|string, array given
ReflectionException: Function nonExistentFunction() does not exist
ArgumentCountError: ReflectionFunction::__construct() expects exactly 1 argument, 0 given
ArgumentCountError: ReflectionFunction::__construct() expects exactly 1 argument, 2 given
TypeError: ReflectionFunction::__construct(): Argument #1 ($function) must be of type Closure|string, array given
