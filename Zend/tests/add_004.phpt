--TEST--
adding numbers to arrays
--FILE--
<?php

$a = array(1,2,3);

$c = $a + 5;
var_dump($c);

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Unsupported operand types in %s on line %d
