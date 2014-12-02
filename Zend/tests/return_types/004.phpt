--TEST--
Returned string, expected array

--FILE--
<?php
function test1() : array {
    return "hello";
}

test1();

--EXPECTF--
Catchable fatal error: The function test1 was expected to return an array and returned a string in %s on line %d
