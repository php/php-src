--TEST--
Generator::throw() where the exception is not caught in the generator
--FILE--
<?php

function gen() {
    yield 'thisThrows';
    yield 'notReached';
}

$gen = gen();
var_dump($gen->throw(new RuntimeException('test')));

?>
--EXPECTF--
RuntimeException: test in %s on line %d
Stack trace:
#0 {main}
