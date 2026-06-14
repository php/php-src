--TEST--
Bug #76946: Cyclic reference in generator not detected
--FILE--
<?php

function gen() {
    $gen = yield;
    foreach ([1, $gen] as $v) {
        yield $v;
    }
}

function gen2() {
    $gen = yield;
    $gen + yield;
}

$gen = gen();
$gen->send($gen);

$gen2 = gen2();
$gen2->send($gen2);

?>
===DONE===
--EXPECT--
===DONE===
