--TEST--
Promoted readonly property reassignment in constructor - outside constructor fails
--FILE--
<?php

class Point {
    public function __construct(
        public readonly int $x = 0,
    ) {
        $this->x = abs($x);
    }

    public function tryModify(): void {
        $this->x = 999;
    }
}

$point = new Point(-5);
var_dump($point->x);

// Cannot reassign from outside constructor
try {
    $point->x = 100;
} catch (Throwable $e) {
    echo get_class($e), ": ", $e->getMessage(), "\n";
}
var_dump($point->x);

// Cannot reassign from a method
try {
    $point->tryModify();
} catch (Throwable $e) {
    echo get_class($e), ": ", $e->getMessage(), "\n";
}
var_dump($point->x);

?>
--EXPECT--
int(5)
Error: Cannot modify readonly property Point::$x
int(5)
Error: Cannot modify readonly property Point::$x
int(5)
