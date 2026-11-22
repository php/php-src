--TEST--
FCC in initializer errors for missing method.
--FILE--
<?php

class ThisClassDoesExist { }

const Closure = ThisClassDoesExist::thisMethodDoesNotExist(...);

var_dump(Closure);

?>
--EXPECTF--
Fatal error: Uncaught Error: Failed to create closure from undefined method ThisClassDoesExist::thisMethodDoesNotExist() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
