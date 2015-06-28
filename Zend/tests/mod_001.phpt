--TEST--
modulus by zero
--FILE--
<?php

$a = array(1,2,3);
$b = array();

try {
    $c = $a % $b;
    var_dump($c);
} catch (Error $e) {
	echo "Error: " . $e->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECTF--	
Error: Modulo by zero
Done
