--TEST--
FETCH_THIS in struct method must separate
--FILE--
<?php

struct Point {
    public function __construct(
        public int $x,
        public int $y,
    ) {}

    public function replace($x, $y) {
        $self = $this;
        $this->x = $x;
        $this->y = $y;
        return $self;
    }
}

$a = new Point(1, 1);
$b = $a->replace(2, 2);
var_dump($a);
var_dump($b);

?>
--EXPECT--
object(Point)#1 (2) {
  ["x"]=>
  int(2)
  ["y"]=>
  int(2)
}
object(Point)#2 (2) {
  ["x"]=>
  int(1)
  ["y"]=>
  int(1)
}
