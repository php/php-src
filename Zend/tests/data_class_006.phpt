--TEST--
A convoluted path
--FILE--
<?php

data class Length {
    public function __construct(public float $length) {}
}

data class Point {
    public int $x;
    private Length $l;
    public function __construct(int $x, public int $y) {
        $previous = $this;
        $this->x = $x;
        $this->l = new Length($x + $y);
        echo "$previous === $this\n";
    }

    public function addNoisily(Point $other): Point {
        echo "Adding $this and $other\n";
        $previous = $this;
        $this->x += $other->x;
        $this->y += $other->y;
        $this->l = new Length($this->x + $this->y);
        echo "$previous !== $this\n";
        return $this;
    }

    public function __toString(): string {
        return "({$this->x},{$this->y}|{$this->l->length})";
    }
}

$p1 = new Point(1, 2);
$p2 = $p1->addNoisily($p1);

?>
--EXPECT--
(1,2|3) === (1,2|3)
Adding (1,2|3) and (1,2|3)
(1,2|3) !== (2,4|6)
