--TEST--
Mutating method calls on structs
--FILE--
<?php

struct Point {
    public function __construct(
        public int $x,
        public int $y,
    ) {}

    public mutating function zero() {
        $this->x = 0;
        $this->y = 0;
    }
}

struct Shape {
    public function __construct(
        public Point $position,
    ) {}
}

$s = new Shape(new Point(1, 1));
$s2 = $s;
$s2->position->zero!();
var_dump($s);
var_dump($s2);

?>
--EXPECT--
object(Shape)#1 (1) {
  ["position"]=>
  object(Point)#2 (2) {
    ["x"]=>
    int(1)
    ["y"]=>
    int(1)
  }
}
object(Shape)#3 (1) {
  ["position"]=>
  object(Point)#4 (2) {
    ["x"]=>
    int(0)
    ["y"]=>
    int(0)
  }
}
