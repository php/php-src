--TEST--
gmp_powm_sec()
--EXTENSIONS--
gmp
--SKIPIF--
<?php
if (!function_exists('gmp_powm_sec')) {
    die('skip gmp_powm_sec() is not available');
}
?>
--FILE--
<?php

var_dump(gmp_strval(gmp_powm_sec(4, 13, 497)));
var_dump(gmp_strval(gmp_powm_sec(gmp_init(7), gmp_init(3), gmp_init(13))));

foreach ([0, -1] as $exp) {
    try {
        var_dump(gmp_powm_sec(4, $exp, 497));
    } catch (\ValueError $e) {
        echo $e::class, ": ", $e->getMessage(), \PHP_EOL;
    }
}

try {
    var_dump(gmp_powm_sec(4, 13, 0));
} catch (\DivisionByZeroError $e) {
    echo $e::class, ": ", $e->getMessage(), \PHP_EOL;
}

try {
    var_dump(gmp_powm_sec(4, 13, 496));
} catch (\ValueError $e) {
    echo $e::class, ": ", $e->getMessage(), \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
string(3) "445"
string(1) "5"
ValueError: gmp_powm_sec(): Argument #2 ($exponent) must be greater than 0
ValueError: gmp_powm_sec(): Argument #2 ($exponent) must be greater than 0
DivisionByZeroError: gmp_powm_sec(): Argument #3 ($modulus) Modulo by zero
ValueError: gmp_powm_sec(): Argument #3 ($modulus) must be odd
Done
