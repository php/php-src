--TEST--
Constant Expressions with unsupported operands 001
--FILE--
<?php
const T = array(1,2) - array(0);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Unsupported operand types: array - array in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
