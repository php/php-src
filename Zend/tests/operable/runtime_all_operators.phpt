--TEST--
Runtime dispatch for all 6 arithmetic operators on Operable classes
--FILE--
<?php

final readonly class Num implements Operable {
    public function __construct(public int|float $v) {}

    public function __add(self|int|float $rhs): self {
        return new self($this->v + ($rhs instanceof self ? $rhs->v : $rhs));
    }

    public function __sub(self|int|float $rhs, OperandSide $side): self {
        $other = $rhs instanceof self ? $rhs->v : $rhs;
        return new self($side === OperandSide::Left ? $this->v - $other : $other - $this->v);
    }

    public function __mul(self|int|float $rhs): self {
        return new self($this->v * ($rhs instanceof self ? $rhs->v : $rhs));
    }

    public function __div(self|int|float $rhs, OperandSide $side): self {
        $other = $rhs instanceof self ? $rhs->v : $rhs;
        return new self($side === OperandSide::Left ? $this->v / $other : $other / $this->v);
    }

    public function __mod(self|int|float $rhs, OperandSide $side): self {
        $other = $rhs instanceof self ? $rhs->v : $rhs;
        $res = $side === OperandSide::Left ? fmod($this->v, $other) : fmod($other, $this->v);
        return new self($res);
    }

    public function __pow(self|int|float $rhs, OperandSide $side): self {
        $other = $rhs instanceof self ? $rhs->v : $rhs;
        return new self($side === OperandSide::Left ? $this->v ** $other : $other ** $this->v);
    }
}

$a = new Num(10);
$b = new Num(4);

// + (left, object+object, commutative right swap)
var_dump(($a + $b)->v);
var_dump(($a + 5)->v);
var_dump((5 + $a)->v);

// - (left with OperandSide::Left, right with OperandSide::Right)
var_dump(($a - $b)->v);
var_dump(($a - 3)->v);
var_dump((3 - $a)->v);

// * (left, commutative right swap, chained expression)
var_dump(($a * 3)->v);
var_dump((3 * $a)->v);
var_dump(($a * 3 + new Num(5))->v);

// / (left and right with OperandSide)
var_dump(($a / $b)->v);
var_dump((20 / $a)->v);

// % (left and right with OperandSide, including float % Operable without truncation)
var_dump(($a % 3)->v);
var_dump((25 % $a)->v);
var_dump((12.5 % $a)->v);

// ** (left and right with OperandSide)
var_dump(($b ** 3)->v);
var_dump((2 ** $b)->v);

?>
--EXPECT--
int(14)
int(15)
int(15)
int(6)
int(7)
int(-7)
int(30)
int(30)
int(35)
float(2.5)
int(2)
float(1)
float(5)
float(2.5)
int(64)
int(16)
