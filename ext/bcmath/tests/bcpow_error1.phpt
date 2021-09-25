--TEST--
bcpow() does not support non-integral exponents
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    var_dump(bcpow('1', '1.1', 2));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
bcpow(): Argument #2 ($exponent) cannot have a fractional part
