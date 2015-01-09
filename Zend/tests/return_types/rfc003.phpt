--TEST--
RFC example: cannot return null with a return type declaration

--FILE--
<?php
function foo(): DateTime {
    return null;
}

foo();

--EXPECTF--
Catchable fatal error: Return value of foo() must be an instance of DateTime, null returned in %s on line %d
