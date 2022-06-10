--TEST--
Closure type param can be variadic and by-ref
--FILE--
<?php

function test(\Closure(string&...): void $closure) {}

?>
--EXPECT--
