--TEST--
Basic automatic property initialization
--FILE--
<?php

class Point1 {
    public $x, $y, $z;

    public function __construct($this->x, $this->y, $this->z) {}
}

class Point2 {
    public $x, $y, $z;

    public function __construct($this->x, $this->y, $z) {
        $this->z = 10 * $z;
    }
}

class Point3 {
    public $x, $y, $z;

    public function __construct($x, $this->y, $this->z) {
        $this->x = 10 * $x;
    }
}

class Point4 {
    public $x, $y, $z;

    public function __construct($this->x, $this->y, $this->z) {
        $this->y *= 10;
    }
}

class Point5 {
    public $x, $y, $z;

    public function __construct($this->x, $this->y = 500, $this->z = 1000) {}
}

var_dump(new Point1(1, 2, 3));
var_dump(new Point2(1, 2, 3));
var_dump(new Point3(1, 2, 3));
var_dump(new Point4(1, 2, 3));
var_dump(new Point5(1, 2));

?>
--EXPECT--
object(Point1)#1 (3) {
  ["x"]=>
  int(1)
  ["y"]=>
  int(2)
  ["z"]=>
  int(3)
}
object(Point2)#1 (3) {
  ["x"]=>
  int(1)
  ["y"]=>
  int(2)
  ["z"]=>
  int(30)
}
object(Point3)#1 (3) {
  ["x"]=>
  int(10)
  ["y"]=>
  int(2)
  ["z"]=>
  int(3)
}
object(Point4)#1 (3) {
  ["x"]=>
  int(1)
  ["y"]=>
  int(20)
  ["z"]=>
  int(3)
}
object(Point5)#1 (3) {
  ["x"]=>
  int(1)
  ["y"]=>
  int(2)
  ["z"]=>
  int(1000)
}
