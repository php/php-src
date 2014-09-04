--TEST--
gmp_legendre() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

for ($i = -1; $i < 10; $i++) {
	var_dump(gmp_strval(gmp_legendre(($i*$i)-1, 3)));
}

var_dump(gmp_strval(gmp_legendre(7, 23)));
var_dump(gmp_strval(gmp_legendre("733535124", "1234123423434535623")));
var_dump(gmp_strval(gmp_legendre(3, "1234123423434535623")));

$n = "123123";
$n1 = "1231231";

var_dump(gmp_strval(gmp_legendre($n, $n1)));
var_dump(gmp_strval(gmp_legendre($n, 3)));
var_dump(gmp_strval(gmp_legendre(3, $n1)));

var_dump(gmp_legendre(3, array()));
var_dump(gmp_legendre(array(), 3));
var_dump(gmp_legendre(array(), array()));

var_dump(gmp_legendre(array(), array(), 1));
var_dump(gmp_legendre(array()));
var_dump(gmp_legendre());

echo "Done\n";
?>
--EXPECTF--	
string(1) "0"
string(2) "-1"
string(1) "0"
string(1) "0"
string(2) "-1"
string(1) "0"
string(1) "0"
string(2) "-1"
string(1) "0"
string(1) "0"
string(2) "-1"
string(2) "-1"
string(1) "0"
string(1) "0"
string(2) "-1"
string(1) "0"
string(2) "-1"

Warning: gmp_legendre(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_legendre(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_legendre(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_legendre() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Warning: gmp_legendre() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: gmp_legendre() expects exactly 2 parameters, 0 given in %s on line %d
NULL
Done
