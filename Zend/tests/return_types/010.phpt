--TEST--
Returned null, expected array reference

--FILE--
<?php
function &foo(array &$in) : array {
    return null;
}

$array = [1, 2, 3];
var_dump(foo($array));

--EXPECTF--
Catchable fatal error: The function foo was expected to return an array and returned null in %s on line %d
