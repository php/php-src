--TEST--
Generators must return a valid variable with return type specified
--FILE--
<?php

$gen = (function (): Generator {
    return true;
    yield;
})();

var_dump($gen->valid());
var_dump($gen->getReturn());

?>
--EXPECT--
bool(false)
bool(true)
