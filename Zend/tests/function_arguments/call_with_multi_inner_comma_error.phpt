--TEST--
Multiple inner commas in function calls is not allowed
--FILE--
<?php
foo($foo,,$bar);
?>
--EXPECTF--
Parse error: syntax error, unexpected ',', expecting ')' in %s on line %d
