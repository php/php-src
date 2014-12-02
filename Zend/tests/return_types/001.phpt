--TEST--
Returned nothing, expected array

--FILE--
<?php
function test1() : array {
}

test1();

--EXPECTF--
Catchable fatal error: The function test1 was expected to return an array and returned nothing in %s on line %d
