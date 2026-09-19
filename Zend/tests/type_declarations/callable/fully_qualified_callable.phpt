--TEST--
Fully qualified (leading backslash) callable type names must fail
--FILE--
<?php

function test($i): \callable {}

?>
--EXPECTF--
Fatal error: Cannot use "callable" as a type name as it is reserved in %s on line %d
