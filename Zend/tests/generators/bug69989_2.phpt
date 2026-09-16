--TEST--
Collection of some cycles on unfinished generators
--FILE--
<?php

// CV
function gen1() {
    $gen = yield;
    yield;
}

$gen = gen1();
$gen->send($gen);

// This
class Test {
    public $gen;
    public function gen2() {
        yield;
    }
}

$test = new Test;
$test->gen = $test->gen2();

// Closure object
$gen3 = (function() use (&$gen3) {
    yield;
})();

// Yield from array
function gen4() {
    yield from [yield];
}

$gen = gen4();
$gen->send($gen);

?>
===DONE===
--EXPECT--
===DONE===
