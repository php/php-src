--TEST--
Returned null, expected array

--FILE--
<?php
function test1() : array {
    return null;
}

test1();

--EXPECTF--
Catchable fatal error: The function test1 was expected to return an array and returned null in %s on line %d
