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

Fatal error: Uncaught Exception: test in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
