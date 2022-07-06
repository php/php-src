--TEST--
gmp_scan1() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

try {
    var_dump(gmp_scan1("434234", -10));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(gmp_scan1("434234", 1));
var_dump(gmp_scan1(4096, 0));
var_dump(gmp_scan1("1000000000", 5));
var_dump(gmp_scan1("1000000000", 200));

$n = gmp_init("24234527465274");
var_dump(gmp_scan1($n, 10));

try {
    var_dump(gmp_scan1(array(), 200));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
gmp_scan1(): Argument #2 ($start) must be greater than or equal to 0
int(1)
int(12)
int(9)
int(-1)
int(10)
gmp_scan1(): Argument #1 ($num1) must be of type GMP|string|int, array given
Done
