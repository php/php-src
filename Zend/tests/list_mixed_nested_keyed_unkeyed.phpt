--TEST--
list() with nested unkeyed and keyed list()
--FILE--
<?php

$points = [
    ["x" => 1, "y" => 2],
    ["x" => 2, "y" => 1]
];

list(list("x" => $x1, "y" => $y1), list("x" => $x2, "y" => $y2)) = $points;
var_dump($x1, $y1, $x2, $y2);

echo PHP_EOL;

$invertedPoints = [
    "x" => [1, 2],
    "y" => [2, 1]
];

list("x" => list($x1, $x2), "y" => list($y1, $y2)) = $invertedPoints;
var_dump($x1, $y1, $x2, $y2);

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
