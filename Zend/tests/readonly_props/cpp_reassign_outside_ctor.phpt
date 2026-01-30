--TEST--
Promoted readonly property reassignment in constructor - outside constructor fails
--FILE--
<?php

class Point {
    public function __construct(
        public readonly float $x = 0.0,
    ) {
        $this->x = abs($x);
    }

    public function tryModify(): void {
        $this->x = 999.0;
    }
}

$point = new Point(-5.0);
var_dump($point->x);

// Cannot reassign from outside constructor
try {
    $point->x = 100.0;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

// Cannot reassign from a method
try {
    $point->tryModify();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
float(5)
Cannot modify readonly property Point::$x
Cannot modify readonly property Point::$x
