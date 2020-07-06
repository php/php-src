--TEST--
Only the last argument can be variadic
--FILE--
<?php

function test($foo, ...$bar, $baz) {}

?>
--EXPECTF--
Fatal error: test(): Parameter #3 ($baz) cannot be variadic in %s on line %d
