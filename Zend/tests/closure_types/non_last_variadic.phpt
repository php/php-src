--TEST--
Only last closure type parameter can be variadic
--FILE--
<?php

function test(\Closure(string..., int): void $closure) {}

?>
--EXPECTF--
Fatal error: Only the last parameter can be variadic in %s on line %d
