--TEST--
Bug #65035: yield / exit segfault
--FILE--
<?php

function gen() {
    fn();
    yield;
}

function fn() {
    exit('Done');
}

$gen = gen();
$gen->current();

?>
--EXPECT--
Done
