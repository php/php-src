--TEST--
Structs in nested references
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
        public Point &$position,
    ) {}
}

$p = new Point(1, 1);
$s = new Shape($p);

$p->x = 2;
$p->y = 2;
var_dump($p);
var_dump($s);

$s->position->zero!();
var_dump($p);
var_dump($s);

unset($p);
unset($s);

?>
--EXPECT--
object(Point)#1 (2) {
  ["x"]=>
  int(2)
  ["y"]=>
  int(2)
}
object(Shape)#2 (1) {
  ["position"]=>
  &object(Point)#1 (2) {
    ["x"]=>
    int(2)
    ["y"]=>
    int(2)
  }
}
object(Point)#1 (2) {
  ["x"]=>
  int(0)
  ["y"]=>
  int(0)
}
object(Shape)#2 (1) {
  ["position"]=>
  &object(Point)#1 (2) {
    ["x"]=>
    int(0)
    ["y"]=>
    int(0)
  }
}
