--TEST--
round() rejects invalid rounding modes.
--FILE--
<?php
var_dump(round(1.5, mode: 1234));
?>
--EXPECTF--
Fatal error: Uncaught ValueError: round(): Argument #3 ($mode) must be a valid rounding mode (PHP_ROUND_*) in %s:%d
Stack trace:
#0 %s(%d): round(1.5, 0, 1234)
#1 {main}
  thrown in %s on line %d
