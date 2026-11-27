--TEST--
Operable interop with GMP internal operator overloading
--EXTENSIONS--
gmp
--FILE--
<?php

final readonly class Money implements Operable {
    public function __construct(public int $amount) {}

    public function __add(GMP $other): self {
        return new self($this->amount + gmp_intval($other));
    }

    public function __sub(GMP $other, OperandSide $side): self {
        $val = gmp_intval($other);
        return new self($side === OperandSide::Left ? $this->amount - $val : $val - $this->amount);
    }
}

final readonly class NonMatching implements Operable {
    public function __add(int $x): self {
        return $this;
    }
}

$m = new Money(100);
$g = gmp_init(25);

// Operable takes precedence on left and right
var_dump(($m + $g)->amount);
var_dump(($g + $m)->amount);
var_dump(($m - $g)->amount);
var_dump(($g - $m)->amount);

// Non-matching Operable falls through to GMP's TypeError
$nm = new NonMatching();
foreach ([fn() => $nm + $g, fn() => $g + $nm] as $fn) {
    try {
        $fn();
    } catch (TypeError $e) {
        echo get_class($e) . ": " . $e->getMessage() . "\n";
    }
}

?>
--EXPECT--
int(125)
int(125)
int(75)
int(-75)
TypeError: Number must be of type GMP|string|int, NonMatching given
TypeError: Number must be of type GMP|string|int, NonMatching given
