--TEST--
class name as scalar from ::class keyword error using parent in non class context
--FILE--
<?php

$x = parent::class;

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use "parent" in the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line 3
