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
Fatal error: Uncaught Error: Trying to clone an uncloneable object of class Generator in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
