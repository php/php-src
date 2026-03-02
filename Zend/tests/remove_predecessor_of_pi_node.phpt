--TEST--
Regression test for incorrect update of pi node users when removing a predecessor block
--FILE--
<?php

function test() {
    for (; $n--; )
        C;
}
test();

?>
--EXPECTF--
Warning: Undefined variable $n in %s on line %d

Warning: Decrement on type null has no effect, this will change in the next major version of PHP in %s on line %d
