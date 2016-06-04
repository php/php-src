--TEST--
Static property access on constexpr class evaluating to integer
--FILE--
<?php
((int)1)::$b;
?>
--EXPECTF--
Fatal error: Uncaught Error: Class name must be a valid object or a string in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
