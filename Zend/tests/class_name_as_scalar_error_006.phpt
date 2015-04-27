--TEST--
class name as scalar from ::class keyword error using parent in non class context
--FILE--
<?php

$x = parent::class;

?>
--EXPECTF--
Fatal error: Cannot use "parent" when no class scope is active in %s on line 3
