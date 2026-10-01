--TEST--
Bug 72093: bcpowmod fails on negative scale and corrupts _one_ definition
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    var_dump(bcpowmod(1, 0, 128, -200));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump(bcpowmod(1, 1.2, 1, 1));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: bcpowmod(): Argument #4 ($scale) must be between 0 and 2147483647
ValueError: bcpowmod(): Argument #2 ($exponent) cannot have a fractional part
