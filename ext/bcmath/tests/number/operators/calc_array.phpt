--TEST--
BcMath\Number calc array by operator
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(100);
$array = [1];

try {
    $num + $array;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num - $array;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num * $array;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num / $array;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num % $array;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num ** $array;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: Unsupported operand types: BcMath\Number + array
TypeError: Unsupported operand types: BcMath\Number - array
TypeError: Unsupported operand types: BcMath\Number * array
TypeError: Unsupported operand types: BcMath\Number / array
TypeError: Unsupported operand types: BcMath\Number % array
TypeError: Unsupported operand types: BcMath\Number ** array
