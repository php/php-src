--TEST--
Generator::throw() where the generator throws a different exception
--FILE--
<?php

function gen() {
    echo "before yield\n";
    try {
        yield;
    } catch (RuntimeException $e) {
        echo 'Caught: ', $e, "\n\n";

        throw new LogicException('new throw');
    }
}

$gen = gen();
var_dump($gen->throw(new RuntimeException('throw')));

?>
--EXPECTF--
before yield
Caught: RuntimeException: throw in %s:%d
Stack trace:
#0 {main}


Fatal error: Uncaught LogicException: new throw in %s:%d
Stack trace:
#0 [internal function]: gen()
#1 %s(%d): Generator->throw(Object(RuntimeException))
#2 {main}
  thrown in %s on line %d
