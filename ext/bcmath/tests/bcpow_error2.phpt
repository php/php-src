--TEST--
bcpow() does not support exponents >= 2**63
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    var_dump(bcpow('0', '9223372036854775808', 2));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(bcpow('0', '-9223372036854775808', 2));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
bcpow(): Argument #2 ($exponent) is too large
bcpow(): Argument #2 ($exponent) is too large
