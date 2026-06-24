--TEST--
FCC names in const expressions are case-sensitive.
--FILE--
<?php

const Closure = StrRev(...);

var_dump(Closure);
var_dump((Closure)("abc"));

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function StrRev() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
