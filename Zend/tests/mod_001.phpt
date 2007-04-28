--TEST--
modulus by zero
--FILE--
<?php

$a = array(1,2,3);
$b = array();

$c = $a % $b;
var_dump($c);

echo "Done\n";
?>
--EXPECTF--	
Warning: Division by zero in %s on line %d
bool(false)
Done
