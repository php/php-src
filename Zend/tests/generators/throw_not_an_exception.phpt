--TEST--
Generator::throw() with something that's not an exception
--FILE--
<?php

function gen() {
    yield;
}

$gen = gen();
$gen->throw(new stdClass);

?>
--EXPECTF--
Fatal error: Uncaught TypeError: Generator::throw() expects parameter 1 to be Throwable, object given in %s:%d
Stack trace:
#0 %s(%d): Generator->throw(Object(stdClass))
#1 {main}
  thrown in %s on line %d
