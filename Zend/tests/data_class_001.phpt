--TEST--
Data classes can be defined and compared.
--FILE--
<?php

data class Point {
    public function __construct(public int $x, public int $y) {}
}

$p1 = new Point(1, 2);
$p2 = new Point(1, 2);
$p3 = new Point(2, 3);

var_dump($p1 === $p2);
var_dump($p1 !== $p2);
var_dump($p1 === $p3);
var_dump($p1 !== $p3);
var_dump($p1);
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
data object(Point)#1 (2) {
  ["x"]=>
  int(1)
  ["y"]=>
  int(2)
}
