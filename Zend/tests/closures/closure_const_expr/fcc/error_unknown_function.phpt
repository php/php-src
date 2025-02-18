--TEST--
FCC in initializer errors for missing function.
--FILE--
<?php

const Closure = this_function_does_not_exist(...);

var_dump(Closure);

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function this_function_does_not_exist() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
