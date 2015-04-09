--TEST--
Returned null, expected array

--FILE--
<?php
function test1() : array {
    return null;
}

test1();

--EXPECTF--
Fatal error: Return value of test1() must be of the type array, null returned in %s on line %d
