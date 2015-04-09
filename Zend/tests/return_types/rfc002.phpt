--TEST--
RFC example: expected class int and returned integer

--FILE--
<?php
function answer(): int {
    return 42;
}

answer();

--EXPECTF--
Catchable fatal error: Return value of answer() must be an instance of int, integer returned in %s on line %d
