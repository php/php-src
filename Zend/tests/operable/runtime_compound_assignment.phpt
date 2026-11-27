--TEST--
Compound assignments, aliasing, references, properties, arrays, and exception safety for Operable
--FILE--
<?php

final readonly class Money implements Operable {
    public function __construct(public int $amount) {}

    public function __add(self|int $other): self {
        $val = $other instanceof self ? $other->amount : $other;
        if ($val < 0) {
            throw new DomainException("negative add");
        }
        if ($val === 0) {
            return $this; // returns same object instance
        }
        return new self($this->amount + $val);
    }

    public function __sub(self|int $other, OperandSide $side): self {
        $val = $other instanceof self ? $other->amount : $other;
        return new self($side === OperandSide::Left ? $this->amount - $val : $val - $this->amount);
    }

    public function __mul(int $factor): self {
        return new self($this->amount * $factor);
    }

    public function __div(int $d): self {
        return new self(intdiv($this->amount, $d));
    }

    public function __mod(int $m): self {
        return new self($this->amount % $m);
    }

    public function __pow(int $e): self {
        return new self($this->amount ** $e);
    }
}

// Basic compound assignment for all 6 ops
$m = new Money(10);
$m += new Money(5);   // 15
var_dump($m->amount);
$m -= 3;              // 12
var_dump($m->amount);
$m *= 2;              // 24
var_dump($m->amount);
$m /= 4;              // 6
var_dump($m->amount);
$m **= 2;             // 36
var_dump($m->amount);
$m %= 10;             // 6
var_dump($m->amount);

// Self-aliasing: $m += $m (result == op1 == op2)
$m += $m;             // 12
var_dump($m->amount);

// Returning $this in compound assignment (refcount stability)
$m += 0;              // 12
var_dump($m->amount);

// Scalar LHS with Operable RHS compound assignment
$s = 100;
$s += new Money(25);
var_dump($s->amount);

$s2 = 500;
$s2 -= new Money(75);
var_dump($s2->amount);

// References
$orig = new Money(10);
$ref = &$orig;
$ref += new Money(20);
var_dump($orig->amount, $ref->amount);

// Property and array element compound assignment
$box = (object)['v' => new Money(7)];
$box->v *= 3;
var_dump($box->v->amount);

$arr = [new Money(8)];
$arr[0] += new Money(4);
var_dump($arr[0]->amount);

// Exception during compound assignment leaves LHS intact
$safe = new Money(42);
try {
    $safe += -1;
} catch (DomainException $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}
var_dump($safe->amount);

?>
--EXPECT--
int(15)
int(12)
int(24)
int(6)
int(36)
int(6)
int(12)
int(12)
int(125)
int(425)
int(30)
int(30)
int(21)
int(12)
Caught: negative add
int(42)
