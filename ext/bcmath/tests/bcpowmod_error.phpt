--TEST--
bcpowmod() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcpowmod('a', '1', '1');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    bcpowmod('1', 'a', '1');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    bcpowmod('1', '1', 'a');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: bcpowmod(): Argument #1 ($num) is not well-formed
ValueError: bcpowmod(): Argument #2 ($exponent) is not well-formed
ValueError: bcpowmod(): Argument #3 ($modulus) is not well-formed
