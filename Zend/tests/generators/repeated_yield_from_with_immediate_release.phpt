--TEST--
A generator can be yielded from multiple times, testing immediate release of the yield from'ing generator
--FILE--
<?php

function gen() {
    yield 42;
}
function yield_from($gen) {
    yield from $gen;
}
$gen = gen();
var_dump(yield_from($gen)->current());
var_dump(yield_from($gen)->current());

?>
--EXPECT--
int(42)
int(42)
