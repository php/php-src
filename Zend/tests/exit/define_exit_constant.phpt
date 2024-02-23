--TEST--
Attempting to define exit constant
--FILE--
<?php

const exit = 5;

var_dump(exit);

?>
--EXPECTF--
Fatal error: Cannot define constant with name exit in %s on line %d
