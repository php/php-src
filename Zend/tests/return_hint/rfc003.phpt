--TEST--
Cannot return null with a return type declaration
--FILE--
<?php
function foo(): bar {
    return null;
}

foo();
--EXPECTF--
Catchable fatal error: the function foo was expected to return an object of class bar and returned null in %s on line %d


