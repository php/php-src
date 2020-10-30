--TEST--
Make sure type inference upholds invariants for dead arrays
--FILE--
<?php

function test() {
    foreach ($a as $v) {
        $b[] = $v;
    }
}

test();

?>
--EXPECTF--
Notice: Undefined variable: a in %s on line %d

Warning: Invalid argument supplied for foreach() in %s on line %d
