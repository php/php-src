--TEST--
Generators must return a valid variable with return type specified
--FILE--
<?php

$gen = (function (): Generator {
	1 + $a = 1; // force a temporary
	return true;
	yield;
})();

var_dump($gen->valid());
var_dump($gen->getReturn());

?>
--EXPECT--
bool(false)
bool(true)
