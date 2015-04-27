--TEST--
class name as scalar from ::class keyword error using static non class context
--FILE--
<?php

$x = static::class;

?>
--EXPECTF--
Fatal error: Cannot use "static" when no class scope is active in %s on line 3
