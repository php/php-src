--TEST--
Yield from by multiple generators where a child is destroyed before the parent (no longer supported)
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
--EXPECTF--
int(42)

Fatal error: Uncaught Error: A different generator already yields from this generator in %s:%d
Stack trace:
#0 [internal function]: yield_from(Object(Generator))
#1 %s(%d): Generator->current()
#2 {main}
  thrown in %s on line %d
