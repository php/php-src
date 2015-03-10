--TEST--
Returned 1, expected array

--FILE--
<?php
function test1() : array {
    return 1;
}
test1();

--EXPECTF--
Fatal error: Return value of test1() must be of the type array, integer returned in %s on line %d
