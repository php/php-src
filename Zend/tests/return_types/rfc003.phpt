--TEST--
RFC example: cannot return null with a return type declaration

--FILE--
<?php
function foo(): DateTime {
    return null;
}

foo();

--EXPECTF--
Catchable fatal error: The function foo was expected to return an object of class DateTime and returned null in %s on line %d
