--TEST--
class name as scalar from ::class keyword error using static non class context
--FILE--
<?php

$x = static::class;

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use "static" when no class scope is active in %s:3
Stack trace:
#0 {main}
  thrown in %s on line 3
