--TEST--
BcMath\Number calc other class by operator
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(100);
$other = new stdClass();

try {
    $num + $other;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num - $other;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num * $other;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num / $other;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num % $other;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num ** $other;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: Unsupported operand types: BcMath\Number + stdClass
TypeError: Unsupported operand types: BcMath\Number - stdClass
TypeError: Unsupported operand types: BcMath\Number * stdClass
TypeError: Unsupported operand types: BcMath\Number / stdClass
TypeError: Unsupported operand types: BcMath\Number % stdClass
TypeError: Unsupported operand types: BcMath\Number ** stdClass
