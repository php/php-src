--TEST--
Multiple trailing commas in function calls is not allowed
--FILE--
<?php
foo($foo,,);
?>
--EXPECTF--
Parse error: syntax error, unexpected token ",", expecting ")" in %s on line %d
