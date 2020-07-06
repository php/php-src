--TEST--
Static type outside class generates compile error
--FILE--
<?php

function test(): static {}

?>
--EXPECTF--
Fatal error: "static" cannot be used in the global scope in %s on line %d
