--TEST--
Division by zero in static
--FILE--
<?php
static $a = 1/0;
?>
--EXPECTF--
Fatal error: Uncaught DivisionByZeroError: Division by zero in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
