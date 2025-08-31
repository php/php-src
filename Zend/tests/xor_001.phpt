--TEST--
XORing arrays
--FILE--
<?php

$a = array(1,2,3);
$b = array();

try {
    $c = $a ^ $b;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
Unsupported operand types: array ^ array
Done
