--TEST--
gmp_div_q() tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_div_q());
var_dump(gmp_div_q(""));

var_dump(gmp_div_q(0,1));
var_dump(gmp_div_q(1,0));
var_dump(gmp_div_q(12653,23482734));
var_dump(gmp_div_q(12653,23482734, 10));
var_dump(gmp_div_q(1123123,123));
var_dump(gmp_div_q(1123123,123, 1));
var_dump(gmp_div_q(1123123,123, 2));
var_dump(gmp_div_q(1123123,123, GMP_ROUND_ZERO));
var_dump(gmp_div_q(1123123,123, GMP_ROUND_PLUSINF));
var_dump(gmp_div_q(1123123,123, GMP_ROUND_MINUSINF));

$fp = fopen(__FILE__, 'r');

var_dump(gmp_div_q($fp, $fp));
var_dump(gmp_div_q(array(), array()));

echo "Done\n";
?>
--EXPECTF--	
Warning: gmp_div_q() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: gmp_div_q() expects at least 2 parameters, 1 given in %s on line %d
NULL
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "0"
}

Warning: gmp_div_q(): Zero operand not allowed in %s on line %d
bool(false)
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "0"
}

Warning: gmp_div_q(): Invalid rounding mode %s on line %d
bool(false)
object(GMP)#%d (1) {
  ["num"]=>
  string(4) "9131"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(4) "9132"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(4) "9131"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(4) "9131"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(4) "9132"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(4) "9131"
}

Warning: gmp_div_q(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_div_q(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
