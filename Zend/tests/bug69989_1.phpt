--TEST--
Bug #69989: Cycle collection for yielded values
--FILE--
<?php

function gen() {
    yield yield;
}
$gen = gen();
$gen->send($gen);

?>
===DONE===
--EXPECT--
===DONE===
