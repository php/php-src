--TEST--
GMP functions reject values larger than unsigned long on LLP64
--EXTENSIONS--
gmp
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Windows" || PHP_INT_SIZE !== 8) die("skip LLP64 (Windows 64-bit) only");
?>
--FILE--
<?php

try {
    gmp_pow(2, PHP_INT_MAX);
} catch (ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    gmp_binomial(10, PHP_INT_MAX);
} catch (ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    gmp_root(10, PHP_INT_MAX);
} catch (ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    gmp_rootrem(10, PHP_INT_MAX);
} catch (ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

$n = gmp_init(2);
try {
    $n << PHP_INT_MAX;
} catch (ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    $n ** PHP_INT_MAX;
} catch (ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

echo "Done\n";
?>
--EXPECTF--
gmp_pow(): Argument #2 ($exponent) must be between 0 and %d
gmp_binomial(): Argument #2 ($k) must be between 0 and %d
gmp_root(): Argument #2 ($nth) must be between 1 and %d
gmp_rootrem(): Argument #2 ($nth) must be between 1 and %d
Shift must be between 0 and %d
Exponent must be between 0 and %d
Done
