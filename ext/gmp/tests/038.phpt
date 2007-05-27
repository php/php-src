--TEST--
gmp_scan1() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_scan1("434234", -10));
var_dump(gmp_scan1("434234", 1));
var_dump(gmp_scan1(4096, 0));
var_dump(gmp_scan1("1000000000", 5));
var_dump(gmp_scan1("1000000000", 200));

$n = gmp_init("24234527465274");
var_dump(gmp_scan1($n, 10));

var_dump(gmp_scan1(array(), 200));
var_dump(gmp_scan1(array()));
var_dump(gmp_scan1());

echo "Done\n";
?>
--EXPECTF--	
Warning: gmp_scan1(): Starting index must be greater than or equal to zero in %s on line %d
bool(false)
int(1)
int(12)
int(9)
int(-1)
int(10)

Warning: gmp_scan1(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: Wrong parameter count for gmp_scan1() in %s on line %d
NULL

Warning: Wrong parameter count for gmp_scan1() in %s on line %d
NULL
Done
