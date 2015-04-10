--TEST--
bitwise NOT and arrays
--FILE--
<?php

$a = array(1,2,3);
$b = array(1,2);

try {
	var_dump(~$b);
} catch (EngineException $e) {
	echo "\nException: " . $e->getMessage() . "\n";
}

$a = ~$b;
var_dump($a);

echo "Done\n";
?>
--EXPECTF--	
Exception: Unsupported operand types

Fatal error: Unsupported operand types in %s on line %d
