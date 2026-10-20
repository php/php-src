--TEST--
Bug #78878 (Buffer underflow in bc_shift_addsub)
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    print bcmul("\xB26483605105519922841849335928742092", bcpowmod(2, 65535, -4e-4));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: bcpowmod(): Argument #3 ($modulus) cannot have a fractional part
