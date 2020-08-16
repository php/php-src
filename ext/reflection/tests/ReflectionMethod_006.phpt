--TEST--
ReflectionMethod methods - wrong num args
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php

try {
    new ReflectionMethod();
} catch (TypeError $re) {
    echo "Ok - ".$re->getMessage().PHP_EOL;
}
try {
    new ReflectionMethod('a', 'b', 'c');
} catch (TypeError $re) {
    echo "Ok - ".$re->getMessage().PHP_EOL;
}

?>
--EXPECT--
Ok - ReflectionMethod::__construct() expects exactly 1 argument, 0 given
Ok - ReflectionMethod::__construct() expects exactly 1 argument, 3 given
