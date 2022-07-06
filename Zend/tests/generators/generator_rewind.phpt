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

function &gen2() {
    $foo = 'bar';
    yield $foo;
    yield $foo;
}

$gen = gen2();
foreach ($gen as $v) { }
try {
    foreach ($gen as $v) { }
} catch (Exception $e) {
    echo $e, "\n\n";
}

function gen3() {
    echo "in generator\n";

    if (false) yield;
}

$gen = gen3();
$gen->rewind();

?>
--EXPECTF--
before yield
after yield

Exception: Cannot rewind a generator that was already run in %s:%d
Stack trace:
#0 %s(%d): Generator->rewind()
#1 {main}

Exception: Cannot traverse an already closed generator in %s:%d
Stack trace:
#0 {main}

in generator
