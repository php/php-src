--TEST--
modulus by zero
--FILE--
<?php

$a = array(1,2,3);
$b = array();

try {
    $c = $a % $b;
    var_dump($c);
} catch (Exception $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECTF--	
Exception: Division by zero
Done
