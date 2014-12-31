--TEST--
Generators cannot be cloned
--FILE--
<?php

function gen() {
    yield;
}

$gen = gen();
clone $gen;

?>
--EXPECTF--
Fatal error: Trying to clone an uncloneable object of class Generator in %s on line %d
