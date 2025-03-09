--TEST--
Copy on write
--FILE--
<?php

data class Point {
    public function __construct(public int $x, public int $y) {}
    public function add(Point $other): Point {
        $this->x += $other->x;
        $this->y += $other->y;
        return $this;
    }
    public function __toString(): string {
        return "({$this->x},{$this->y})";
    }
}

$p1 = new Point(1, 2);
$p2 = $p1;

// p2 is copy on write (3,2)
$p2->x = 3;
echo "p1: $p1\n";
echo "p2 (x+3): $p2\n";

echo "p1->add(p2): " . $p1->add($p2) . "\n";
echo "p1: $p1\n";
$po = new Point(1,1);
$p2 = $po;
$p2->x++;
echo "p2: $p2\n";
echo "po: $po\n";

function modify(Point $p): Point {
    $p->y++;
    return $p;
}

$p1 = new Point(1,1);
$p2 = modify($p1);
echo "p1: $p1\n";
echo "p2: $p2\n";

--EXPECT--
p1: (1,2)
p2 (x+3): (3,2)
p1->add(p2): (4,4)
p1: (1,2)
p2: (2,1)
po: (1,1)
p1: (1,1)
p2: (1,2)
