--TEST--
gmp_neg() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_intval(gmp_neg(0)));
var_dump(gmp_intval(gmp_neg(1)));
var_dump(gmp_intval(gmp_neg(-1)));
var_dump(gmp_intval(gmp_neg("-1")));
var_dump(gmp_intval(gmp_neg("")));
var_dump(gmp_intval(gmp_neg(0)));

$n = gmp_init("0");
var_dump(gmp_intval(gmp_neg($n)));
$n = gmp_init("12345678901234567890");
var_dump(gmp_strval(gmp_neg($n)));

var_dump(gmp_neg(1,1));
var_dump(gmp_neg());
var_dump(gmp_neg(array()));

echo "Done\n";
?>
--EXPECTF--	
int(0)
int(-1)
int(1)
int(1)

Warning: gmp_neg(): Unable to convert variable to GMP - string is not an integer in %s on line %d
int(0)
int(0)
int(0)
string(21) "-12345678901234567890"

Warning: gmp_neg() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: gmp_neg() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: gmp_neg(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
