--TEST--
Returned string, expected array

--FILE--
<?php
function test1() : array {
    return "hello";
}

test1();

--EXPECTF--
Catchable fatal error: Return value of test1() must be of the type array, string returned in %s on line %d
