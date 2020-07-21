--TEST--
gmp_div_q() tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_div_q(0,1));
var_dump(gmp_div_q(1,0));
var_dump(gmp_div_q(12653,23482734));
try {
    var_dump(gmp_div_q(12653,23482734, 10));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
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
gmp_div_q(): Argument #3 ($round) must be one of GMP_ROUND_ZERO, GMP_ROUND_PLUSINF, or GMP_ROUND_MINUSINF
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
