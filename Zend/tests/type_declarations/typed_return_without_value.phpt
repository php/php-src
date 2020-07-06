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
Fatal error: Function test() with return type int must return a value in %s on line %d
