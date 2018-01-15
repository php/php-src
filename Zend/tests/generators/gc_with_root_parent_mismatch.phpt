--TEST--
Generator GC when the yield from parent chain does not reach the root
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
--EXPECT--
int(1)
int(1)
