--TEST--
Using clone statement on undefined variable
--FILE--
<?php

$a = clone $b;

?>
--EXPECTF--
Notice: Undefined variable: b in %s on line %d

Fatal error: Uncaught exception 'EngineException' with message '__clone method called on non-object' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
