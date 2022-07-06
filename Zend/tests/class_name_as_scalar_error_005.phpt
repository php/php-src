--TEST--
class name as scalar from ::class keyword error using static non class context
--FILE--
<?php

$x = static::class;

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use "static" in the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line 3
