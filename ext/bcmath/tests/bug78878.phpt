--TEST--
Bug #78878 (Buffer underflow in bc_shift_addsub)
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    print bcmul("\xB26483605105519922841849335928742092", bcpowmod(2, 65535, -4e-4));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
bcpowmod(): Argument #3 ($modulus) cannot have a fractional part
