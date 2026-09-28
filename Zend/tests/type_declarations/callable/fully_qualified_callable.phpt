--TEST--
Fully qualified (leading backslash) callable type names must fail
--FILE--
<?php

function test($i): \callable {}

?>
--EXPECTF--
Fatal error: Type declaration 'callable' must be unqualified in %s on line %d
