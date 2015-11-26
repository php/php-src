--TEST--
Only the last argument can be variadic
--FILE--
<?php

function test($foo, ...$bar, $baz) {}

?>
--EXPECTF--
Fatal error: Only the last parameter can be variadic in %s on line %d
