--TEST--
Invalid octal
--FILE--
<?php

$x = 08;
?>
--EXPECTF--
Parse error: Invalid numeric literal in %s on line 3
