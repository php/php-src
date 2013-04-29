--TEST--
Generator::throw() where the exception is caught in the generator
--FILE--
<?php

function gen() {
    try {
        yield;
    } catch (RuntimeException $e) {
        echo $e, "\n\n";
    }

    yield 'result';
}

$gen = gen();
var_dump($gen->throw(new RuntimeException('Test')));

?>
--EXPECTF--
exception 'RuntimeException' with message 'Test' in %s:%d
Stack trace:
#0 {main}

string(6) "result"
