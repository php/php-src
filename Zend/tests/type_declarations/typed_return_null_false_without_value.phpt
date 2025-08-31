--TEST--
Typed null|false return without value generates compile-time error
--FILE--
<?php

function test() : null|false {
    return;
}

test();

?>
--EXPECTF--
Fatal error: A function with return type must return a value (did you mean "return null;" instead of "return;"?) in %s on line %d
