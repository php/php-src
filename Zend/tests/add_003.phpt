--TEST--
adding arrays to objects 
--FILE--
<?php

$a = array(1,2,3);

$o = new stdclass;
$o->prop = "value";

try {
	var_dump($o + $a);
} catch (EngineException $e) {
	echo "\nException: " . $e->getMessage() . "\n";
}

$c = $o + $a;
var_dump($c);

echo "Done\n";
?>
--EXPECTF--	
Notice: Object of class stdClass could not be converted to int in %sadd_003.php on line %d

Exception: Unsupported operand types

Notice: Object of class stdClass could not be converted to int in %s on line %d

Fatal error: Unsupported operand types in %s on line %d
