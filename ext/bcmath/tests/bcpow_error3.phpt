--TEST--
bcpow() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcpow('a', '1');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    bcpow('1', 'a');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: bcpow(): Argument #1 ($num) is not well-formed
ValueError: bcpow(): Argument #2 ($exponent) is not well-formed
