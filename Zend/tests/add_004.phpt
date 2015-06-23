--TEST--
adding numbers to arrays
--FILE--
<?php

$a = array(1,2,3);

try {
	var_dump($a + 5);
} catch (Error $e) {
	echo "\nException: " . $e->getMessage() . "\n";
}

$c = $a + 5;
var_dump($c);

echo "Done\n";
?>
--EXPECTF--	
Exception: Unsupported operand types

Fatal error: Uncaught Error: Unsupported operand types in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
