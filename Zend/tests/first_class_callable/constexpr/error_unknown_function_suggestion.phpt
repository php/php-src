--TEST--
FCC in initializer suggests a similar function name for a missing function.
--FILE--
<?php

const Closure = strlenn(...);

var_dump(Closure);

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function strlenn() (did you mean strlen()?) in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
