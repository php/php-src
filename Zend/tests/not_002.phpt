--TEST--
bitwise NOT and arrays
--FILE--
<?php

$a = array(1,2,3);
$b = array(1,2);

$a = ~$b;
var_dump($a);

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Unsupported operand types in %s on line %d
