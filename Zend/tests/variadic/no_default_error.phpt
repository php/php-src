--TEST--
Variadic argument cannot have a default value
--FILE--
<?php

function test(...$args = 123) {}

?>
--EXPECTF--
Fatal error: Variadic parameter cannot have a default value in %s on line %d
