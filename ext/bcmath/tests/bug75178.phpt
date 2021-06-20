--TEST--
Bug #75178 (bcpowmod() misbehaves for non-integer base or modulus)
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    var_dump(bcpowmod('4.1', '4', '3', 3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(bcpowmod('4', '4', '3.1', 3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
bcpowmod(): Argument #1 ($num) cannot have a fractional part
bcpowmod(): Argument #3 ($modulus) cannot have a fractional part
