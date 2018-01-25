--TEST--
gmp_sqrtrem() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

$r = gmp_sqrtrem(-1);
var_dump($r);

$r = gmp_sqrtrem("0");
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

$r = gmp_sqrtrem(2);
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

$r = gmp_sqrtrem(10);
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

$r = gmp_sqrtrem(7);
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

$r = gmp_sqrtrem(3);
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

$r = gmp_sqrtrem(100000);
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

$r = gmp_sqrtrem("1000000");
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

$r = gmp_sqrtrem("1000001");
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));


$n = gmp_init(-1);
$r = gmp_sqrtrem($n);
var_dump($r);

$n = gmp_init(1000001);
$r = gmp_sqrtrem($n);
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

var_dump(gmp_sqrtrem(array()));
var_dump(gmp_sqrtrem());

echo "Done\n";
?>
--EXPECTF--
Warning: gmp_sqrtrem(): Number has to be greater than or equal to 0 in %s on line %d
bool(false)
string(1) "0"
string(1) "0"
string(1) "1"
string(1) "1"
string(1) "3"
string(1) "1"
string(1) "2"
string(1) "3"
string(1) "1"
string(1) "2"
string(3) "316"
string(3) "144"
string(4) "1000"
string(1) "0"
string(4) "1000"
string(1) "1"

Warning: gmp_sqrtrem(): Number has to be greater than or equal to 0 in %s on line %d
bool(false)
string(4) "1000"
string(1) "1"

Warning: gmp_sqrtrem(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_sqrtrem() expects exactly 1 parameter, 0 given in %s on line %d
NULL
Done
