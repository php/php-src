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
Warning: Undefined variable $a in %s on line %d

Warning: foreach() argument must be of type array|object, null given in %s on line %d
