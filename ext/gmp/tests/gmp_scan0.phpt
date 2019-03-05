--TEST--
gmp_scan0() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_scan0("434234", -10));
var_dump(gmp_scan0("434234", 1));
var_dump(gmp_scan0(4096, 0));
var_dump(gmp_scan0("1000000000", 5));
var_dump(gmp_scan0("1000000000", 200));

$n = gmp_init("24234527465274");
var_dump(gmp_scan0($n, 10));

var_dump(gmp_scan0(array(), 200));

echo "Done\n";
?>
--EXPECTF--
Warning: gmp_scan0(): Starting index must be greater than or equal to zero in %s on line %d
bool(false)
int(2)
int(0)
int(5)
int(200)
int(13)

Warning: gmp_scan0(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
