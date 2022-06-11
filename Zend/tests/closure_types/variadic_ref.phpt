--TEST--
Closure type param can be variadic and by-ref
--FILE--
<?php

function test(fn(string&...): void $closure) {}

?>
--EXPECT--
