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
Fatal error: Uncaught Error: Cannot throw objects that do not implement Throwable in %s:%d
Stack trace:
#0 [internal function]: gen()
#1 %s(%d): Generator->throw(Object(stdClass))
#2 {main}
  thrown in %s on line %d
