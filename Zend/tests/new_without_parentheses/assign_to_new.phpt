--TEST--
Cannot assign to new expression
--FILE--
<?php

new ArrayObject() = 1;

?>
--EXPECTF--
Parse error: syntax error, unexpected token "=" in %s on line %d
