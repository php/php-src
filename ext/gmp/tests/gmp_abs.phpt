--TEST--
gmp_abs() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_strval(gmp_abs("")));
var_dump(gmp_strval(gmp_abs("0")));
var_dump(gmp_strval(gmp_abs(0)));
var_dump(gmp_strval(gmp_abs(-111111111111111111111)));
var_dump(gmp_strval(gmp_abs("111111111111111111111")));
var_dump(gmp_strval(gmp_abs("-111111111111111111111")));
var_dump(gmp_strval(gmp_abs("0000")));
var_dump(gmp_strval(gmp_abs("09876543")));
var_dump(gmp_strval(gmp_abs("-099987654")));

var_dump(gmp_abs());
var_dump(gmp_abs(1,2));
var_dump(gmp_abs(array()));

echo "Done\n";
?>
--EXPECTF--
Warning: gmp_abs(): Unable to convert variable to GMP - string is not an integer in %s on line %d
string(1) "0"
string(1) "0"
string(1) "0"

Warning: gmp_abs(): Unable to convert variable to GMP - wrong type in %s on line %d
string(1) "0"
string(21) "111111111111111111111"
string(21) "111111111111111111111"
string(1) "0"

Warning: gmp_abs(): Unable to convert variable to GMP - string is not an integer in %s on line %d
string(1) "0"

Warning: gmp_abs(): Unable to convert variable to GMP - string is not an integer in %s on line %d
string(1) "0"

Warning: gmp_abs() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: gmp_abs() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: gmp_abs(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
