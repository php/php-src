--TEST--
subtracting arrays
--FILE--
<?php

$a = array(1,2,3);
$b = array(1);

$c = $a - $b;
var_dump($c);

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Unsupported operand types in %s on line %d
