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
Fatal error: Exceptions must be valid objects derived from the Exception base class in %s on line %d
