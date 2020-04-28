--TEST--
Test that the number parameter type doesn't accept non-numeric types in strict mode
--FILE--
<?php
declare(strict_types=1);

function foo(number $a)
{
}

try {
    foo(null);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    foo("0");
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
foo(): Argument #1 ($a) must be of type number, null given, called in %s on line %d
foo(): Argument #1 ($a) must be of type number, string given, called in %s on line %d
foo(): Argument #1 ($a) must be of type number, array given, called in %s on line %d
foo(): Argument #1 ($a) must be of type number, object given, called in %s on line %d
foo(): Argument #1 ($a) must be of type number, resource given, called in %s on line %d
