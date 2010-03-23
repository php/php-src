--TEST--
gmp_div_r() tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_div_r());
var_dump(gmp_div_r(""));

var_dump($r = gmp_div_r(0,1));
var_dump(gmp_strval($r));
var_dump($r = gmp_div_r(1,0));
var_dump($r = gmp_div_r(12653,23482734));
var_dump(gmp_strval($r));
var_dump($r = gmp_div_r(12653,23482734, 10));
var_dump(gmp_strval($r));
var_dump($r = gmp_div_r(1123123,123));
var_dump(gmp_strval($r));
var_dump($r = gmp_div_r(1123123,123, 1));
var_dump(gmp_strval($r));
var_dump($r = gmp_div_r(1123123,123, 2));
var_dump(gmp_strval($r));
var_dump($r = gmp_div_r(1123123,123, GMP_ROUND_ZERO));
var_dump(gmp_strval($r));
var_dump($r = gmp_div_r(1123123,123, GMP_ROUND_PLUSINF));
var_dump(gmp_strval($r));
var_dump($r = gmp_div_r(1123123,123, GMP_ROUND_MINUSINF));
var_dump(gmp_strval($r));

$fp = fopen(__FILE__, 'r');

var_dump(gmp_div_r($fp, $fp));
var_dump(gmp_div_r(array(), array()));

echo "Done\n";
?>
--EXPECTF--	
Warning: gmp_div_r() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: gmp_div_r() expects at least 2 parameters, 1 given in %s on line %d
NULL
int(0)
string(1) "0"

Warning: gmp_div_r(): Zero operand not allowed in %s on line %d
bool(false)
int(12653)
string(5) "12653"
NULL

Warning: gmp_strval(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
int(10)
string(2) "10"
int(113)
string(3) "113"
int(10)
string(2) "10"
int(10)
string(2) "10"
int(113)
string(3) "113"
int(10)
string(2) "10"

Warning: gmp_div_r(): supplied resource is not a valid GMP integer resource in %s on line %d
bool(false)

Warning: gmp_div_r(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
