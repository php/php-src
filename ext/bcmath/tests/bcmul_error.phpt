--TEST--
bcmul() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcmul('a', '1');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    bcmul('1', 'a');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: bcmul(): Argument #1 ($num1) is not well-formed
ValueError: bcmul(): Argument #2 ($num2) is not well-formed
