--TEST--
Unary minus and unary plus dispatch through __mul(-1) and __mul(1) on Operable classes
--FILE--
<?php

final readonly class Num implements Operable {
    public function __construct(public int|float $v) {}

    public function __add(self $rhs): self {
        return new self($this->v + $rhs->v);
    }

    public function __mul(self|int $rhs): self {
        return new self($this->v * ($rhs instanceof self ? $rhs->v : $rhs));
    }
}

final readonly class SideAwareNum implements Operable {
    public function __construct(public int $v) {}

    public function __mul(int $rhs, OperandSide $side): string {
        return ($side === OperandSide::Left ? 'L' : 'R') . ':' . ($this->v * $rhs);
    }
}

final readonly class FloatOnlyMul implements Operable {
    public function __construct(public float $v) {}

    public function __mul(float $rhs): self {
        return new self($this->v * $rhs);
    }
}

final readonly class SelfOnlyMul implements Operable {
    public function __mul(self $rhs): self {
        return $this;
    }
}

final readonly class NoMul implements Operable {
    public function __add(int $rhs): self {
        return $this;
    }
}

$a = new Num(7);
var_dump((-$a)->v);
var_dump((+$a)->v);
var_dump((-(-$a))->v);
var_dump((-$a + new Num(10))->v);

$s = new SideAwareNum(5);
var_dump(-$s);
var_dump(+$s);

$f = new FloatOnlyMul(2.5);
var_dump((-$f)->v);
var_dump((+$f)->v);

foreach ([new SelfOnlyMul(), new NoMul()] as $obj) {
    foreach (['-' => fn() => -$obj, '+' => fn() => +$obj] as $op => $fn) {
        try {
            $fn();
            echo "UNEXPECTED\n";
        } catch (InvalidOperatorError $e) {
            echo $op . ' ' . get_class($obj) . ': ' . $e->getMessage() . "\n";
        }
    }
}

?>
--EXPECT--
int(-7)
int(7)
int(7)
int(3)
string(4) "L:-5"
string(3) "L:5"
float(-2.5)
float(2.5)
- SelfOnlyMul: Unsupported operand types: SelfOnlyMul * int
+ SelfOnlyMul: Unsupported operand types: SelfOnlyMul * int
- NoMul: Unsupported operand types: NoMul * int
+ NoMul: Unsupported operand types: NoMul * int
