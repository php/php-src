--TEST--
Static type outside class generates compile error
--FILE--
<?php

function test(): static {}

?>
--EXPECTF--
Fatal error: Cannot use "static" when no class scope is active in %s on line %d
