--TEST--
Yielding from the already running Generator should fail (bug #69458)
--FILE--
<?php

function gen() {
	yield from yield;
}

($gen = gen())->send($gen);

?>
--EXPECTF--
Fatal error: Uncaught Error: Impossible to yield from the Generator being currently run in %s:%d
Stack trace:
#0 [internal function]: gen()
#1 %s(%d): Generator->send(Object(Generator))
#2 {main}
  thrown in %s on line %d
