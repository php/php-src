--TEST--
Using clone statement on non-object
--FILE--
<?php

$a = clone array();

?>
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message '__clone method called on non-object' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
