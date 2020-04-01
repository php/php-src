--TEST--
Constant Expressions with unsupported operands 001
--FILE--
<?php
const T = array(1,2) - array(0);
--EXPECT--
Fatal error: Uncaught TypeError: Unsupported operand types: array - array in [no active file]:0
Stack trace:
#0 {main}
  thrown in [no active file] on line 0
