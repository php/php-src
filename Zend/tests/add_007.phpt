--TEST--
adding strings to arrays 
--FILE--
<?php

$a = array(1,2,3);

$s1 = "some string";

try {
	var_dump($a + $s1);
} catch (Error $e) {
	echo "\nException: " . $e->getMessage() . "\n";
}

$c = $a + $s1;
var_dump($c);

echo "Done\n";
?>
--EXPECTF--	

Warning: A non-numeric value encountered in %s on line %d

Exception: Unsupported operand types

Warning: A non-numeric value encountered in %s on line %d

Fatal error: Uncaught Error: Unsupported operand types in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
