--TEST--
gmp_powm() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_strval(gmp_powm(0,1,10)));
var_dump(gmp_strval(gmp_powm(5,1,10)));
var_dump(gmp_strval(gmp_powm(-5,1,-10)));
var_dump(gmp_strval(gmp_powm(-5,1,10)));
var_dump(gmp_strval(gmp_powm(-5,11,10)));
var_dump(gmp_strval(gmp_powm("77",3,1000)));

$n = gmp_init(11);
var_dump(gmp_strval(gmp_powm($n,3,1000)));
$e = gmp_init(7);
var_dump(gmp_strval(gmp_powm($n,$e,1000)));
$m = gmp_init(900);
var_dump(gmp_strval(gmp_powm($n,$e,$m)));

var_dump(gmp_powm(array(),$e,$m));
var_dump(gmp_powm($n,array(),$m));
var_dump(gmp_powm($n,$e,array()));
var_dump(gmp_powm(array(),array(),array()));
var_dump(gmp_powm(array(),array()));
var_dump(gmp_powm(array()));
var_dump(gmp_powm());

$n = gmp_init("-5");
var_dump(gmp_powm(10, $n, 10));

$n = gmp_init("0");
var_dump(gmp_powm(10, $n, 10));

echo "Done\n";
?>
--EXPECTF--	
string(1) "0"
string(1) "5"
string(1) "5"
string(1) "5"
string(1) "5"
string(3) "533"
string(3) "331"
string(3) "171"
string(3) "371"

Warning: gmp_powm(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_powm(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_powm(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_powm(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_powm() expects exactly 3 parameters, 2 given in %s on line %d
NULL

Warning: gmp_powm() expects exactly 3 parameters, 1 given in %s on line %d
NULL

Warning: gmp_powm() expects exactly 3 parameters, 0 given in %s on line %d
NULL

Warning: gmp_powm(): Second parameter cannot be less than 0 in %s on line %d
bool(false)
resource(%d) of type (GMP integer)
Done
