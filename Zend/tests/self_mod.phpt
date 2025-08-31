--TEST--
Moduloing strings
--FILE--
<?php

$s = "123";
$s1 = "test";
$s2 = "45345some";

$s %= 22;
var_dump($s);

try {
    $s1 %= 11;
    var_dump($s1);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$s2 %= 33;
var_dump($s2);

echo "Done\n";
?>
--EXPECTF--
int(13)
Unsupported operand types: string % int

Warning: A non-numeric value encountered in %s on line %d
int(3)
Done
