--TEST--
Return from by-ref generator
--FILE--
<?php

function &gen() {
    yield;
    $arr = [42];
    return $arr[0];
}

function gen2() {
    var_dump(yield from gen());
}

gen2()->next();

?>
--EXPECTF--
Notice: Only variable references should be yielded by reference in %s on line %d
int(42)
