--TEST--
Promoted readonly property reassignment in constructor - basic
--FILE--
<?php

class Point {
    public function __construct(
        public readonly int $x = 0,
        public readonly int $y = 0,
    ) {
        // Reassign promoted readonly properties - allowed once
        $this->x = abs($x);
        $this->y = abs($y);
    }
}

$point = new Point();
var_dump($point->x, $point->y);

$point2 = new Point(-5, -3);
var_dump($point2->x, $point2->y);

?>
--EXPECT--
int(0)
int(0)
int(5)
int(3)
