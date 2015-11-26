--TEST--
Generator::throw() where the exception is caught in the generator
--FILE--
<?php

function gen() {
    echo "before yield\n";
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
before yield
RuntimeException: Test in %s:%d
Stack trace:
#0 {main}

string(6) "result"
