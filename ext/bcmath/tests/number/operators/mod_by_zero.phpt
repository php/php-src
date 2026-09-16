--TEST--
BcMath\Number mod by zero by operator
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(100);

try {
    $num % 0;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num % '0';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num % (new BcMath\Number(0));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$zero = new BcMath\Number(0);

try {
    100 % $zero;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    '100' % $zero;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num % $zero;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
DivisionByZeroError: Modulo by zero
DivisionByZeroError: Modulo by zero
DivisionByZeroError: Modulo by zero
DivisionByZeroError: Modulo by zero
DivisionByZeroError: Modulo by zero
DivisionByZeroError: Modulo by zero
