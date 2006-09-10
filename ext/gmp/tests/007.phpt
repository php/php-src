--TEST--
gmp_div_qr() tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_div_qr());
var_dump(gmp_div_qr(""));

var_dump($r = gmp_div_qr(0,1));
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));
var_dump($r = gmp_div_qr(1,0));
var_dump($r = gmp_div_qr(12653,23482734));
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));
var_dump($r = gmp_div_qr(12653,23482734, 10));
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));
var_dump($r = gmp_div_qr(1123123,123));
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));
var_dump($r = gmp_div_qr(1123123,123, 1));
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));
var_dump($r = gmp_div_qr(1123123,123, 2));
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));
var_dump($r = gmp_div_qr(1123123,123, GMP_ROUND_ZERO));
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));
var_dump($r = gmp_div_qr(1123123,123, GMP_ROUND_PLUSINF));
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));
var_dump($r = gmp_div_qr(1123123,123, GMP_ROUND_MINUSINF));
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

$fp = fopen(__FILE__, 'r');

var_dump(gmp_div_qr($fp, $fp));
var_dump(gmp_div_qr(array(), array()));

echo "Done\n";
?>
--EXPECTF--	
Warning: Wrong parameter count for gmp_div_qr() in %s on line %d
NULL

Warning: Wrong parameter count for gmp_div_qr() in %s on line %d
NULL
array(2) {
  [0]=>
  resource(%d) of type (GMP integer)
  [1]=>
  resource(%d) of type (GMP integer)
}
string(1) "0"
string(1) "0"

Warning: gmp_div_qr(): Zero operand not allowed in %s on line %d
bool(false)
array(2) {
  [0]=>
  resource(%d) of type (GMP integer)
  [1]=>
  resource(%d) of type (GMP integer)
}
string(1) "0"
string(5) "12653"
NULL

Warning: gmp_strval(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_strval(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
array(2) {
  [0]=>
  resource(%d) of type (GMP integer)
  [1]=>
  resource(%d) of type (GMP integer)
}
string(4) "9131"
string(2) "10"
array(2) {
  [0]=>
  resource(%d) of type (GMP integer)
  [1]=>
  resource(%d) of type (GMP integer)
}
string(4) "9132"
string(4) "-113"
array(2) {
  [0]=>
  resource(%d) of type (GMP integer)
  [1]=>
  resource(%d) of type (GMP integer)
}
string(4) "9131"
string(2) "10"
array(2) {
  [0]=>
  resource(%d) of type (GMP integer)
  [1]=>
  resource(%d) of type (GMP integer)
}
string(4) "9131"
string(2) "10"
array(2) {
  [0]=>
  resource(%d) of type (GMP integer)
  [1]=>
  resource(%d) of type (GMP integer)
}
string(4) "9132"
string(4) "-113"
array(2) {
  [0]=>
  resource(%d) of type (GMP integer)
  [1]=>
  resource(%d) of type (GMP integer)
}
string(4) "9131"
string(2) "10"

Warning: gmp_div_qr(): supplied resource is not a valid GMP integer resource in %s on line %d
bool(false)

Warning: gmp_div_qr(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
