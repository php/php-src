--TEST--
bcpow() does not support non-integral exponents
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    var_dump(bcpow('1', '1.1', 2));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump(bcpow('1', '0.1', 2));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: bcpow(): Argument #2 ($exponent) cannot have a fractional part
ValueError: bcpow(): Argument #2 ($exponent) cannot have a fractional part
