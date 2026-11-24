--TEST--
Operable interop with BcMath\Number internal operator overloading
--EXTENSIONS--
bcmath
--FILE--
<?php

use BcMath\Number;

final readonly class Money implements Operable {
    public function __construct(public string $amount) {}

    public function __add(Number $other): self {
        return new self((new Number($this->amount))->add($other)->value);
    }

    public function __sub(Number $other, OperandSide $side): self {
        $selfNum = new Number($this->amount);
        $res = $side === OperandSide::Left ? $selfNum->sub($other) : $other->sub($selfNum);
        return new self($res->value);
    }
}

final readonly class NonMatching implements Operable {
    public function __add(int $x): self {
        return $this;
    }
}

$m = new Money("100");
$n = new Number("25");

// Operable takes precedence on left and right
var_dump(($m + $n)->amount);
var_dump(($n + $m)->amount);
var_dump(($m - $n)->amount);
var_dump(($n - $m)->amount);

// Compound assignments
$m2 = $m;
$m2 += $n;
var_dump($m2->amount);

$n2 = $n;
$n2 += $m;
var_dump($n2 instanceof Money, $n2->amount);

// Non-matching Operable falls through to BcMath\Number (which rejects it -> InvalidOperatorError)
$nm = new NonMatching();
foreach ([fn() => $nm + $n, fn() => $n + $nm] as $fn) {
    try {
        $fn();
    } catch (InvalidOperatorError $e) {
        echo get_class($e) . ": " . $e->getMessage() . "\n";
    }
}

?>
--EXPECT--
string(3) "125"
string(3) "125"
string(2) "75"
string(3) "-75"
string(3) "125"
bool(true)
string(3) "125"
InvalidOperatorError: Unsupported operand types: NonMatching + BcMath\Number
InvalidOperatorError: Unsupported operand types: BcMath\Number + NonMatching
