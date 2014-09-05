--TEST--
gmp_popcount() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_popcount(-1));
var_dump(gmp_popcount(0));
var_dump(gmp_popcount(12123));
var_dump(gmp_popcount("52638927634234"));
var_dump(gmp_popcount("-23476123423433"));
$n = gmp_init("9876546789222");
var_dump(gmp_popcount($n));
var_dump(gmp_popcount(array()));
var_dump(gmp_popcount());

echo "Done\n";
?>
--EXPECTF--	
int(-1)
int(0)
int(10)
int(31)
int(-1)
int(20)

Warning: gmp_popcount(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_popcount() expects exactly 1 parameter, 0 given in %s on line %d
NULL
Done
