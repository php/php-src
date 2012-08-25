--TEST--
A generator can only be rewinded before or at the first yield
--FILE--
<?php

function gen() {
    echo "before yield\n";
    yield;
    echo "after yield\n";
    yield;
}

$gen = gen();
$gen->rewind();
$gen->rewind();
$gen->next();

try {
    $gen->rewind();
} catch (Exception $e) {
    echo "\n", $e, "\n\n";
}

function gen2() {
    echo "in generator\n";

    if (false) yield;
}

$gen = gen2();
$gen->rewind();

?>
--EXPECTF--
before yield
after yield

exception 'Exception' with message 'Cannot rewind a generator that was already run' in %s:%d
Stack trace:
#0 %s(%d): Generator->rewind()
#1 {main}

in generator
