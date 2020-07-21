--TEST--
gmp_div_r() tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump($r = gmp_div_r(0,1));
var_dump($r = gmp_div_r(1,0));
var_dump($r = gmp_div_r(12653,23482734));
try {
    var_dump($r = gmp_div_r(12653,23482734, 10));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump($r = gmp_div_r(1123123,123));
var_dump($r = gmp_div_r(1123123,123, 1));
var_dump($r = gmp_div_r(1123123,123, 2));
var_dump($r = gmp_div_r(1123123,123, GMP_ROUND_ZERO));
var_dump($r = gmp_div_r(1123123,123, GMP_ROUND_PLUSINF));
var_dump($r = gmp_div_r(1123123,123, GMP_ROUND_MINUSINF));

$fp = fopen(__FILE__, 'r');

try {
    var_dump(gmp_div_r($fp, $fp));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_div_r(array(), array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECTF--
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "0"
}

Warning: gmp_div_r(): Zero operand not allowed in %s on line %d
bool(false)
object(GMP)#%d (1) {
  ["num"]=>
  string(5) "12653"
}
gmp_div_r(): Argument #3 ($round) must be one of GMP_ROUND_ZERO, GMP_ROUND_PLUSINF, or GMP_ROUND_MINUSINF
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "10"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(4) "-113"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "10"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "10"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(4) "-113"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "10"
}
gmp_div_r(): Argument #1 ($a) must be of type bool|int|string|GMP, resource given
gmp_div_r(): Argument #1 ($a) must be of type bool|int|string|GMP, array given
Done
