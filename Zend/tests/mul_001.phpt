--TEST--
multiplying arrays
--FILE--
<?php

$a = array(1,2,3);
$b = array(1);

try {
	var_dump($a * $b);
} catch (EngineException $e) {
	echo "\nException: " . $e->getMessage() . "\n";
}

$c = $a * $b;
var_dump($c);

echo "Done\n";
?>
--EXPECTF--	
Exception: Unsupported operand types

Fatal error: Unsupported operand types in %s on line %d
