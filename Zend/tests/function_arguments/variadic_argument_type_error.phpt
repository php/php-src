--TEST--
Call userland function with incorrect variadic argument type
--FILE--
<?php

function foo($foo, int ...$bar) {}

try {
    foo(1, []);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    foo(1, 1, 1, []);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

--EXPECTF--
foo() expects argument #2 ($bar) to be of type int, array given, called in %s on line %d
foo() expects argument #4 ($bar) to be of type int, array given, called in %s on line %d
