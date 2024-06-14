--TEST--
Attempting to define die constant
--FILE--
<?php

const die = 5;

var_dump(die);

?>
--EXPECTF--
Fatal error: Cannot define constant with name die in %s on line %d
