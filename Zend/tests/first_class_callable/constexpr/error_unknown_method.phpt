--TEST--
FCC in initializer errors for missing method.
--FILE--
<?php

class ThisClassDoesExist { }

const Closure = ThisClassDoesExist::thisMethodDoesNotExist(...);

var_dump(Closure);

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined method ThisClassDoesExist::thisMethodDoesNotExist() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
