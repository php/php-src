--TEST--
Bug #65035: yield / exit segfault
--FILE--
<?php

function gen() {
    f();
    yield;
}

function f() {
    exit('Done');
}

$gen = gen();
$gen->current();

?>
--EXPECT--
Done
