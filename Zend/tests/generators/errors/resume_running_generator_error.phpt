--TEST--
It is not possible to resume an already running generator
--FILE--
<?php

function gen() {
    $gen = yield;
    try {
        $gen->next();
    } catch (Error $e) {
        echo "\nException: " . $e->getMessage() . "\n";
    }
    $gen->next();
}

$gen = gen();
$gen->send($gen);
$gen->next();

?>
--EXPECTF--
Exception: Cannot resume an already running generator

Fatal error: Uncaught Error: Cannot resume an already running generator in %s:%d
Stack trace:
#0 %s(%d): Generator->next()
#1 [internal function]: gen()
#2 %s(%d): Generator->send(Object(Generator))
#3 {main}
  thrown in %s on line %d
