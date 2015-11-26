--TEST--
bitwise NOT and arrays
--FILE--
<?php

$a = array(1,2,3);
$b = array(1,2);

try {
	var_dump(~$b);
} catch (Error $e) {
	echo "\nException: " . $e->getMessage() . "\n";
}

$a = ~$b;
var_dump($a);

echo "Done\n";
?>
--EXPECTF--	
Exception: Unsupported operand types

Fatal error: Uncaught Error: Unsupported operand types in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
