--TEST--
Yield from does not leave a dangling send target
--FILE--
<?php
function gen1() {
    yield from [yield];
}

$gen = gen1();
$gen->send(new StdClass);

function gen2() {
    $x = yield;
    yield from [1, 2, 3];
}
$gen = gen2();
$gen->send(new StdClass);
$gen->send(new StdClass);
?>
===DONE===
--EXPECT--
===DONE===
