--TEST--
oss-fuzz #60441 (Double compilation of arrow function)
--FILE--
<?php
assert(fn()=>y)[y]??=y;
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "y" in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
