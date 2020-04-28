--TEST--
Test that the number parameter type doesn't accept any kind of non-numeric value in weak mode
--FILE--
<?php

function foo(number $a)
{
}

try {
    foo("foo");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    foo([]);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    foo(new stdClass());
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    foo(fopen(__FILE__, "r"));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECTF--
foo(): Argument #1 ($a) must be of type number, string given, called in %s on line %d
foo(): Argument #1 ($a) must be of type number, array given, called in %s on line %d
foo(): Argument #1 ($a) must be of type number, object given, called in %s on line %d
foo(): Argument #1 ($a) must be of type number, resource given, called in %s on line %d
