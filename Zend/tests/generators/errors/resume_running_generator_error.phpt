--TEST--
It is not possible to resume an already running generator
--FILE--
<?php

function gen() {
    $gen = yield;
    $gen->next();
}

$gen = gen();
$gen->send($gen);
$gen->next();

?>
--EXPECTF--
Fatal error: Cannot resume an already running generator in %s on line %d
