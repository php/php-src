--TEST--
Bug 72093: bcpowmod fails on negative scale and corrupts _one_ definition
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    var_dump(bcpowmod(1, 0, 128, -200));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(bcpowmod(1, 1.2, 1, 1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
bcpowmod(): Argument #4 ($scale) must be between 0 and 2147483647
bcpowmod(): Argument #2 ($exponent) cannot have a fractional part
