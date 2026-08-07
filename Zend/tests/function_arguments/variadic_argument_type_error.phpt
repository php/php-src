--TEST--
Call userland function with incorrect variadic argument type
--FILE--
<?php

function foo($foo, int ...$bar) {}

try {
    foo(1, []);
} catch (TypeError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    foo(1, 1, 1, []);
} catch (TypeError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

?>
--EXPECTF--
TypeError: foo(): Argument #2 must be of type int, array given, called in %s on line %d
TypeError: foo(): Argument #4 must be of type int, array given, called in %s on line %d
