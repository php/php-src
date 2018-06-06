--TEST--
Leading commas in function calls is not allowed
--FILE--
<?php
foo(,$foo);
?>
--EXPECTF--
Parse error: syntax error, unexpected ',' in %s on line %d
