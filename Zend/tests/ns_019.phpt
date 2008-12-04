--TEST--
019: __NAMESPACE__ constant and runtime names (php name)
--FILE--
<?php
function foo() {
	return __FUNCTION__;
}

$x = __NAMESPACE__ . "\\foo"; 
echo $x(),"\n";
--EXPECT--
foo
