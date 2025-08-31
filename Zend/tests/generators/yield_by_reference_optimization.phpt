--TEST--
Make sure optimization does not interfere with yield by ref
--FILE--
<?php

function &gen() {
    yield $v = 0;
    yield $v = 1;
}

foreach (gen() as $v) {
    var_dump($v);
}

?>
--EXPECTF--
Notice: Only variable references should be yielded by reference in %s on line %d
int(0)

Notice: Only variable references should be yielded by reference in %s on line %d
int(1)
