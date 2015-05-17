--TEST--
Generator::throw() on an already closed generator
--FILE--
<?php

function gen() {
    yield;
}

$gen = gen();
$gen->next();
$gen->next();
var_dump($gen->valid());
$gen->throw(new Exception('test'));

?>
--EXPECTF--
bool(false)

Exception: test in %s on line %d
Stack trace:
#0 {main}
