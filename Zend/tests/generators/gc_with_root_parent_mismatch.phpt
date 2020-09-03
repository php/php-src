--TEST--
Generator GC when the yield from parent chain does not reach the root (no longer supported)
--FILE--
<?php

function root() {
    yield 1;
    yield 2;
}

function delegate($gen) {
    yield from $gen;
}

$gen = delegate(delegate(root()));
$gen1 = delegate(delegate($gen));
$gen2 = delegate(delegate($gen));
var_dump($gen1->current());
var_dump($gen2->current());
$gen1->next();
$gen1->next();
gc_collect_cycles();

?>
--EXPECTF--
int(1)

Fatal error: Uncaught Error: A different generator already yields from this generator in %s:%d
Stack trace:
#0 %s(%d): delegate(Object(Generator))
#1 [internal function]: delegate(Object(Generator))
#2 %s(%d): Generator->current()
#3 {main}
  thrown in %s on line %d
