--TEST--
Other features work with data classes
--FILE--
<?php

interface Point {
  public function add(Point $point): Point;
  public float $length { get; }
}

trait PythagoreanTheorem {
    public readonly float $length;

    public function memoizeLength(): void {
        $this->length = sqrt($this->x ** 2 + $this->y ** 2);
    }
}

final readonly data class Point2D implements Point {
    use PythagoreanTheorem; // contains implementation of $length

    public function __construct(public int $x, public int $y) {
        $this->memoizeLength(); // from the trait
    }

    public function add(Point $point): Point {
        return new static($this->x + $point->x, $this->y + $point->y);
    }
}

$p1 = new Point2D(1, 1);
$p2 = new Point2D(2, 2);
$p3 = $p1->add($p2);
echo $p3->length, "\n";
?>
--EXPECT--
6
