--TEST--
class name as scalar from ::class keyword error using parent in non class context
--FILE--
<?php

$x = parent::class;

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use "parent" when no class scope is active in %s:3
Stack trace:
#0 {main}
  thrown in %s on line 3
