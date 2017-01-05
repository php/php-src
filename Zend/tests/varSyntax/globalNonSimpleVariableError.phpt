--TEST--
Global keyword only accepts simple variables
--FILE--
<?php

global $$foo->bar;

?>
--EXPECTF--
Parse error: syntax error, unexpected '->' (T_OBJECT_OPERATOR), expecting ',' or ';' in %s on line %d
