--TEST--
Attempting to define die() function
--FILE--
<?php

function die() { }

?>
--EXPECTF--
Parse error: syntax error, unexpected token "exit", expecting "(" in %s on line %d
