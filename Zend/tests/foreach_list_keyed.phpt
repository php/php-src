--TEST--
foreach with list syntax, keyed
--FILE--
<?php

$points = [
    ["x" => 1, "y" => 2],
    ["x" => 2, "y" => 1]
];

foreach ($points as list("x" => $x, "y" => $y)) {
    var_dump($x, $y);
}

echo PHP_EOL;

$invertedPoints = [
    "x" => [1, 2],
    "y" => [2, 1]
];

foreach ($invertedPoints as list(0 => $row1, 1 => $row2)) {
    var_dump($row1, $row2);
}

?>
--EXPECT--
int(1)
int(2)
int(2)
int(1)

int(1)
int(2)
int(2)
int(1)
