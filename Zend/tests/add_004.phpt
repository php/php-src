--TEST--
adding numbers to arrays
--FILE--
<?php

$a = array(1,2,3);

try {
	var_dump($a + 5);
} catch (EngineException $e) {
	echo "\nException: " . $e->getMessage() . "\n";
}

$c = $a + 5;
var_dump($c);

echo "Done\n";
?>
--EXPECTF--	
Exception: Unsupported operand types

Fatal error: Unsupported operand types in %s on line %d
