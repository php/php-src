--TEST--
Structs in references
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

$p = new Point(1, 1);
$pRef = &$p;
$pRef->x = 2;
$pRef->y = 2;
var_dump($p);
var_dump($pRef);
$pRef->zero!();
var_dump($p);
var_dump($pRef);

?>
--EXPECT--
object(Point)#1 (2) {
  ["x"]=>
  int(2)
  ["y"]=>
  int(2)
}
object(Point)#1 (2) {
  ["x"]=>
  int(2)
  ["y"]=>
  int(2)
}
object(Point)#1 (2) {
  ["x"]=>
  int(0)
  ["y"]=>
  int(0)
}
object(Point)#1 (2) {
  ["x"]=>
  int(0)
  ["y"]=>
  int(0)
}
