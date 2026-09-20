--TEST--
ReflectionMethod methods - wrong num args
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php

try {
    new ReflectionMethod();
} catch (Throwable $re) {
    echo $re::class, ': ', $re->getMessage(), "\n";
}
try {
    new ReflectionMethod('a', 'b', 'c');
} catch (Throwable $re) {
    echo $re::class, ': ', $re->getMessage(), "\n";
}

?>
--EXPECT--
ArgumentCountError: ReflectionMethod::__construct() expects at least 1 argument, 0 given
ArgumentCountError: ReflectionMethod::__construct() expects at most 2 arguments, 3 given
