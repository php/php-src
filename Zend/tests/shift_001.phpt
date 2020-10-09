--TEST--
shifting strings left
--FILE--
<?php

$s = "123";
$s1 = "test";
$s2 = "45345some";

$s <<= 2;
var_dump($s);

try {
    $s1 <<= 1;
    var_dump($s1);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$s2 <<= 3;
var_dump($s2);

echo "Done\n";
?>
--EXPECTF--
int(492)
Unsupported operand types: string << int

Warning: A non-numeric value encountered in %s on line %d
int(362760)
Done
