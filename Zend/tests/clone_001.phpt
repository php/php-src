--TEST--
Using clone statement on non-object
--FILE--
<?php

$a = clone array();

?>
--EXPECTF--
Fatal error: Uncaught EngineException: __clone method called on non-object in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
