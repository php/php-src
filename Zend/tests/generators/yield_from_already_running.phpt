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
Fatal error: Impossible to yield from the Generator being currently run in %s on line %d
