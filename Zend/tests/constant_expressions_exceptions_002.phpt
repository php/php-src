--TEST--
Constant Expressions with unsupported operands 002
--FILE--
<?php
try {
	require("constant_expressions_exceptions.inc");
} catch (Error $e) {
	echo "\nException: " . $e->getMessage() . " in " , $e->getFile() . " on line " . $e->getLine() . "\n";
}
?>
DONE
--EXPECTF--
Fatal error: Unsupported operand types in %sconstant_expressions_exceptions.inc on line 2
