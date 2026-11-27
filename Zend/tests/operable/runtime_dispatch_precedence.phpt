--TEST--
Runtime dispatch priority between left and right Operable operands
--FILE--
<?php

final readonly class LeftWins implements Operable {
    public function __add(RightWithSide|RightCommutative $other): string {
        return "LeftWins::__add";
    }
    public function __sub(int $other): string {
        return "LeftWins::__sub(int)";
    }
    public function __mul(int $other): string {
        return "LeftWins::__mul(int)";
    }
    public function __div(int $other): string {
        return "LeftWins::__div(int)";
    }
    public function __mod(int $other): string {
        return "LeftWins::__mod(int)";
    }
    public function __pow(int $other): string {
        return "LeftWins::__pow(int)";
    }
}

final readonly class RightWithSide implements Operable {
    public function __add(LeftWins $other, OperandSide $side): string {
        return "RightWithSide::__add(" . $side->name . ")";
    }
    public function __sub(LeftWins $other, OperandSide $side): string {
        return "RightWithSide::__sub(" . $side->name . ")";
    }
}

final readonly class RightCommutative implements Operable {
    public function __add(LeftWins $other): string {
        return "RightCommutative::__add";
    }
    public function __mul(LeftWins $other): string {
        return "RightCommutative::__mul";
    }
    public function __sub(LeftWins $other): string {
        return "RightCommutative::__sub";
    }
    public function __div(LeftWins $other): string {
        return "RightCommutative::__div";
    }
    public function __mod(LeftWins $other): string {
        return "RightCommutative::__mod";
    }
    public function __pow(LeftWins $other): string {
        return "RightCommutative::__pow";
    }
}

$l = new LeftWins();
$rs = new RightWithSide();
$rc = new RightCommutative();

// Step 2: Left wins when left accepts right
var_dump($l + $rs);
var_dump($l + $rc);

// Step 3: Left rejects RightWithSide for '-', falls back to RightWithSide with OperandSide::Right
var_dump($l - $rs);

// Step 4: Left rejects RightCommutative for '*', falls back to commutative swap
var_dump($l * $rc);

// Non-commutative operators do NOT swap to 1-param right operand
foreach (['-', '/', '%', '**'] as $op) {
    try {
        match ($op) {
            '-' => $l - $rc,
            '/' => $l / $rc,
            '%' => $l % $rc,
            '**' => $l ** $rc,
        };
    } catch (InvalidOperatorError $e) {
        echo "Caught $op: " . $e->getMessage() . "\n";
    }
}

?>
--EXPECT--
string(15) "LeftWins::__add"
string(15) "LeftWins::__add"
string(27) "RightWithSide::__sub(Right)"
string(23) "RightCommutative::__mul"
Caught -: Unsupported operand types: LeftWins - RightCommutative
Caught /: Unsupported operand types: LeftWins / RightCommutative
Caught %: Unsupported operand types: LeftWins % RightCommutative
Caught **: Unsupported operand types: LeftWins ** RightCommutative
