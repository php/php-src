--TEST--
Nullable typed return without value generates friendlier error message
--FILE--
<?php

function test() : ?int {
    return;
}

test();

?>
--EXPECTF--
Fatal error: Function test() with return type ?int must return a value (did you mean "return null;" instead of "return;"?) in %s on line %d
