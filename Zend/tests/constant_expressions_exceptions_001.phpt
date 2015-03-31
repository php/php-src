--TEST--
Constant Expressions with unsupported operands 001
--FILE--
<?php
const T = array(1,2) - array(0);
--EXPECTF--
Fatal error: Unsupported operand types in %sconstant_expressions_exceptions_001.php on line 2
