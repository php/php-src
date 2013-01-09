--TEST--
class name as scalar from ::class keyword error using parent in non class context
--FILE--
<?php

$x = parent::class;

?>
--EXPECTF--
Fatal error: Cannot access parent::class when no class scope is active in %s on line %d
