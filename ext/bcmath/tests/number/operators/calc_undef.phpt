--TEST--
BcMath\Number calc undefined var by operator
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(100);

try {
    $num + $undef;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num - $undef;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num * $undef;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num / $undef;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num % $undef;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num ** $undef;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
Warning: Undefined variable $undef in %s
Unsupported operand types: BcMath\Number + null

Warning: Undefined variable $undef in %s
Unsupported operand types: BcMath\Number - null

Warning: Undefined variable $undef in %s
Unsupported operand types: BcMath\Number * null

Warning: Undefined variable $undef in %s
Unsupported operand types: BcMath\Number / null

Warning: Undefined variable $undef in %s
Unsupported operand types: BcMath\Number % null

Warning: Undefined variable $undef in %s
Unsupported operand types: BcMath\Number ** null
