--TEST--
Returned nothing, expected array

--FILE--
<?php
function test1() : array {
}

test1();

--EXPECTF--
Catchable fatal error: Return value of test1() must be of the type array, none returned in %s on line %d
