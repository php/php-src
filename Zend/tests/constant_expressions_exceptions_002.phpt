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
Exception: Unsupported operand types: array - array in %s on line %d
DONE
