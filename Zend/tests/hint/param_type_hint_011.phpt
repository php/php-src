--TEST--
Parameter type hint - Testing with undefined variable
--FILE--
<?php

function test(string $a) {
}

test($x);

?>
--EXPECTF--
Notice: Undefined variable: x in %s on line %d

Catchable fatal error: Argument 1 passed to test() must be of the type string, null given, called in %s on line %d and defined in %s on line %d
