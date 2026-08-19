--TEST--
BcMath\Number calc non-numeric string by operator
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(100);

try {
    $num + 'a';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num - 'a';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num * 'a';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num / 'a';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num % 'a';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num ** 'a';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: Right string operand cannot be converted to BcMath\Number
ValueError: Right string operand cannot be converted to BcMath\Number
ValueError: Right string operand cannot be converted to BcMath\Number
ValueError: Right string operand cannot be converted to BcMath\Number
ValueError: Right string operand cannot be converted to BcMath\Number
ValueError: Right string operand cannot be converted to BcMath\Number
