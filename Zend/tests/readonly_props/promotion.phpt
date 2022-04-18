--TEST--
Promoted readonly property
--FILE--
<?php

class Point {
    public function __construct(
        public readonly float $x = 0.0,
        public readonly float $y = 0.0,
        public readonly float $z = 0.0,
    ) {}
}

var_dump(new Point);
$point = new Point(1.0, 2.0, 3.0);
try {
    $point->x = 4.0;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($point);

?>
--EXPECT--
object(Point)#1 (3) {
  ["x"]=>
  float(0)
  ["y"]=>
  float(0)
  ["z"]=>
  float(0)
}
Cannot modify readonly property Point::$x
object(Point)#1 (3) {
  ["x"]=>
  float(1)
  ["y"]=>
  float(2)
  ["z"]=>
  float(3)
}
