--TEST--
bcpow() does not support exponents >= 2**63
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    var_dump(bcpow('0', '9223372036854775808', 2));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(bcpow('0', '-9223372036854775808', 2));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: bcpow(): Argument #2 ($exponent) is too large
ValueError: bcpow(): Argument #2 ($exponent) is too large
