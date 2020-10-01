--TEST--
gmp_scan0() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

try {
    var_dump(gmp_scan0("434234", -10));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(gmp_scan0("434234", 1));
var_dump(gmp_scan0(4096, 0));
var_dump(gmp_scan0("1000000000", 5));
var_dump(gmp_scan0("1000000000", 200));

$n = gmp_init("24234527465274");
var_dump(gmp_scan0($n, 10));

try {
    var_dump(gmp_scan0(array(), 200));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
gmp_scan0(): Argument #2 ($start) must be greater than or equal to 0
int(2)
int(0)
int(5)
int(200)
int(13)
gmp_scan0(): Argument #1 ($num1) must be of type GMP|string|int, array given
Done
