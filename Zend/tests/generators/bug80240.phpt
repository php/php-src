--TEST--
Bug #80240: Use after free multi yield from
--FILE--
<?php

function gen() {
    yield 0;
    yield from gen();
}

function bar($gen) {
    yield from $gen;
}

$gen = gen();
$a = bar($gen);
$b = bar($gen);
$a->rewind();
$b->rewind();
$a->next();
unset($gen);
unset($a);
unset($b);

?>
===DONE===
--EXPECT--
===DONE===
