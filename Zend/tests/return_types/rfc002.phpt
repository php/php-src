--TEST--
RFC example: expected class int and returned integer

--FILE--
<?php
function answer(): int {
    return 42;
}

answer();

--EXPECTF--
Catchable fatal error: The function answer was expected to return an object of class int and returned an integer in %s on line %d
