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
Catchable fatal error: Return value of foo() must be of the type array, null returned in %s on line %d
