--TEST--
XORing arrays
--FILE--
<?php

$a = array(1,2,3);
$b = array();

try {
    $c = $a ^ $b;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
TypeError: Unsupported operand types: array ^ array
Done
