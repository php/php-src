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
} catch (Throwable $re) {
    echo $re::class, ': ', $re->getMessage(), "\n";
}
try {
    $a = new ReflectionFunction('nonExistentFunction');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $a = new ReflectionFunction();
} catch (Throwable $re) {
    echo $re::class, ': ', $re->getMessage(), "\n";
}
try {
    $a = new ReflectionFunction(1, 2);
} catch (Throwable $re) {
    echo $re::class, ': ', $re->getMessage(), "\n";
}
try {
    $a = new ReflectionFunction([]);
} catch (Throwable $re) {
    echo $re::class, ': ', $re->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: ReflectionFunction::__construct(): Argument #1 ($function) must be of type Closure|string, array given
ReflectionException: Function nonExistentFunction() does not exist
ArgumentCountError: ReflectionFunction::__construct() expects exactly 1 argument, 0 given
ArgumentCountError: ReflectionFunction::__construct() expects exactly 1 argument, 2 given
TypeError: ReflectionFunction::__construct(): Argument #1 ($function) must be of type Closure|string, array given
