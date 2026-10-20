--TEST--
modulus by zero
--FILE--
<?php

$a = array(1,2,3);
$b = array();

try {
    $c = $a % $b;
    var_dump($c);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECTF--
TypeError: Unsupported operand types: array % array
Done
