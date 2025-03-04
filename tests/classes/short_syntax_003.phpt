--TEST--
test all features
--FILE--
<?php

interface Point {
    public function x(): int;
    public function y(): int;
}

trait XTrait {
    public function x(): int {
        return $this->x;
    }
}

trait YTrait {
    public function y(): int {
        return $this->y;
    }
}

class BasePoint {
    public int $x;
    public int $y;

    public function __construct(int $x, int $y) {
        $this->x = $x;
        $this->y = $y;
    }
}

class PointImpl(public int $x, public int $y) extends BasePoint implements Point use XTrait, YTrait;

function foo(Point $p): void {
    var_dump($p->x());
    var_dump($p->y());
}

$p = new PointImpl(1, 2);
foo($p);
?>
--EXPECT--
int(1)
int(2)
