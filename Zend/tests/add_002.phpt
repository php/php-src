--TEST--
adding objects to arrays
--FILE--
<?php

$a = array(1,2,3);

$o = new stdclass;
$o->prop = "value";

$c = $a + $o;
var_dump($c);

echo "Done\n";
?>
--EXPECTF--	
Notice: Object of class stdClass could not be converted to int in %s on line %d

Fatal error: Unsupported operand types in %s on line %d
