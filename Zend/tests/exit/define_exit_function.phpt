--TEST--
Attempting to define exit() function
--FILE--
<?php

function exit() { }

?>
--EXPECTF--
Parse error: syntax error, unexpected token "exit", expecting "(" in %s on line %d
