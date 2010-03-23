--TEST--
adding strings to arrays 
--FILE--
<?php

$a = array(1,2,3);

$s1 = "some string";

$c = $a + $s1;
var_dump($c);

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Unsupported operand types in %s on line %d
