--TEST--
Generator cycle collection edge cases
--FILE--
<?php

// Extra args
function gen1() {
    yield;
}
$obj = new stdClass;
$obj->gen = gen1($obj);

// Symtable
function gen2() {
    $varName = 'a';
    $$varName = yield;
    yield;
}
$gen = gen2();
$gen->send($gen);

// Symtable indirect
function gen3() {
    $varName = 'a';
    $$varName = 42;
    $var = yield;
    yield;
}
$gen = gen3();
$gen->send($gen);

// Yield from root
function gen4() {
    yield from yield;
}
$gen = gen4();
$gen2 = gen4($gen);
$gen2->send([1, 2, 3]);
$gen->send($gen2);

?>
===DONE===
--EXPECT--
===DONE===
