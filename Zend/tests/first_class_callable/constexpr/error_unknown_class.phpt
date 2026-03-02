--TEST--
FCC in initializer errors for missing class.
--FILE--
<?php

const Closure = ThisClassNotDoesExist::thisMethodIsNotRelevant(...);

var_dump(Closure);

?>
--EXPECTF--
Fatal error: Uncaught Error: Class "ThisClassNotDoesExist" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
