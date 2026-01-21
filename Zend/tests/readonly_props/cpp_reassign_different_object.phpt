--TEST--
Promoted readonly property reassignment in constructor - different object fails
--FILE--
<?php

class Point {
    public function __construct(
        public readonly float $x = 0.0,
    ) {
        $this->x = abs($x);
    }

    public static function createFrom(Point $other): Point {
        $new = new self();
        // Cannot modify another object's readonly property
        try {
            $other->x = 999.0;
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        return $new;
    }
}

$p1 = new Point(-5.0);
var_dump($p1->x);

$p2 = Point::createFrom($p1);
var_dump($p1->x);  // Unchanged

// Also test: constructor cannot modify another instance of the same class
class Counter {
    private static ?Counter $last = null;

    public function __construct(
        public readonly int $value = 0,
    ) {
        $this->value = $value + 1;  // Allowed: own property

        // Cannot modify previous instance
        if (self::$last !== null) {
            try {
                self::$last->value = 999;
            } catch (Error $e) {
                echo $e->getMessage(), "\n";
            }
        }
        self::$last = $this;
    }
}

$c1 = new Counter(10);
var_dump($c1->value);

$c2 = new Counter(20);
var_dump($c1->value, $c2->value);  // $c1 unchanged

?>
--EXPECT--
float(5)
Cannot modify readonly property Point::$x
float(5)
int(11)
Cannot modify readonly property Counter::$value
int(11)
int(21)
