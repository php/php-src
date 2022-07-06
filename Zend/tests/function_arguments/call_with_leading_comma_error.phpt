--TEST--
Leading commas in function calls is not allowed
--FILE--
<?php
foo(,$foo);
?>
--EXPECTF--
Parse error: syntax error, unexpected token "," in %s on line %d
