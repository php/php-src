--TEST--
RFC example: returned type does not match the type declaration

--FILE--
<?php

function get_config(): array {
    return 42;
}

get_config();

--EXPECTF--
Catchable fatal error: Return value of get_config() must be of the type array, integer returned in %s on line %d
