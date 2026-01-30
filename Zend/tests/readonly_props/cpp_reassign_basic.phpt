--TEST--
Promoted readonly property reassignment in constructor - basic
--FILE--
<?php

class Point {
    public function __construct(
        public readonly float $x = 0.0,
        public readonly float $y = 0.0,
    ) {
        // Reassign promoted readonly properties - allowed once
        $this->x = abs($x);
        $this->y = abs($y);
    }
}

$point = new Point();
var_dump($point->x, $point->y);

$point2 = new Point(-5.0, -3.0);
var_dump($point2->x, $point2->y);

?>
--EXPECT--
float(0)
float(0)
float(5)
float(3)
