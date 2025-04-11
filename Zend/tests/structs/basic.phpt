--TEST--
Basic structs
--FILE--
<?php

struct Point {
    public function __construct(
        public int $x,
        public int $y,
    ) {}
}

$a = new Point(1, 1);
$b = $a;
$b->x = 2;
$b->y = 2;
var_dump($a);
var_dump($b);

?>
--EXPECT--
object(Point)#1 (2) {
  ["x"]=>
  int(1)
  ["y"]=>
  int(1)
}
object(Point)#2 (2) {
  ["x"]=>
  int(2)
  ["y"]=>
  int(2)
}
