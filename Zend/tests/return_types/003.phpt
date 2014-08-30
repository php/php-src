--TEST--
Returned 1, expected array

--FILE--
<?php
function test1() : array {
    return 1;
}
test1();

--EXPECTF--
Catchable fatal error: The function test1 was expected to return an array and returned an integer in %s on line %d
