--TEST--
BcMath\Number calc undefined var by operator
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(100);

try {
    $num + null;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num - null;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num * null;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num / null;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num % null;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num ** null;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: Unsupported operand types: BcMath\Number + null
TypeError: Unsupported operand types: BcMath\Number - null
TypeError: Unsupported operand types: BcMath\Number * null
TypeError: Unsupported operand types: BcMath\Number / null
TypeError: Unsupported operand types: BcMath\Number % null
TypeError: Unsupported operand types: BcMath\Number ** null
