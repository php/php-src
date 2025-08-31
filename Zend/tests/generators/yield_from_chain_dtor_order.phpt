--TEST--
Leaf link may need to be invalidated depending on dtor order
--FILE--
<?php

function gen2() {
    yield 1;
}
function gen() {
    yield from gen2();
}
function bar($g) {
    yield from $g;
}

$gen = gen();
$bar = bar($gen);
var_dump($bar->current());
$copy = $bar;
unset($gen);

?>
--EXPECT--
int(1)
