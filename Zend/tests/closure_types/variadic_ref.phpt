--TEST--
Closure type param can be variadic and by-ref
--FILE--
<?php

function test(\Closure(string&...): void $closure) {}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "&", expecting ")" in %s on line %d
