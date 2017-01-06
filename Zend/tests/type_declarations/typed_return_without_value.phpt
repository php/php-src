--TEST--
Typed return without value generates compile-time error
--FILE--
<?php

function test() : int {
    return;
}

test();

?>
--EXPECTF--
Fatal error: A function with return type must return a value in %s on line %d
