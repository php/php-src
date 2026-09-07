--TEST--
BcMath\Number calc undefined var by operator
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(100);

try {
    $num + $undef;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num - $undef;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num * $undef;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num / $undef;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num % $undef;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num ** $undef;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
Warning: Undefined variable $undef in %s
TypeError: Unsupported operand types: BcMath\Number + null

Warning: Undefined variable $undef in %s
TypeError: Unsupported operand types: BcMath\Number - null

Warning: Undefined variable $undef in %s
TypeError: Unsupported operand types: BcMath\Number * null

Warning: Undefined variable $undef in %s
TypeError: Unsupported operand types: BcMath\Number / null

Warning: Undefined variable $undef in %s
TypeError: Unsupported operand types: BcMath\Number % null

Warning: Undefined variable $undef in %s
TypeError: Unsupported operand types: BcMath\Number ** null
