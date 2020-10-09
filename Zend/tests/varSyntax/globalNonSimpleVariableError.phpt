--TEST--
Global keyword only accepts simple variables
--FILE--
<?php

global $$foo->bar;

?>
--EXPECTF--
Parse error: syntax error, unexpected token "->", expecting "," or ";" in %s on line %d
