--TEST--
gmp_div_q() tests
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump(gmp_div_q(0,1));

try {
    var_dump(gmp_div_q(1, 0));
} catch (\DivisionByZeroError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

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

try {
    var_dump(gmp_div_q($fp, $fp));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_div_q(array(), array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
object(GMP)#1 (1) {
  ["num"]=>
  string(1) "0"
}
Division by zero
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "0"
}
gmp_div_q(): Argument #3 ($rounding_mode) must be one of GMP_ROUND_ZERO, GMP_ROUND_PLUSINF, or GMP_ROUND_MINUSINF
object(GMP)#1 (1) {
  ["num"]=>
  string(4) "9131"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(4) "9132"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(4) "9131"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(4) "9131"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(4) "9132"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(4) "9131"
}
gmp_div_q(): Argument #1 ($num1) must be of type GMP|string|int, resource given
gmp_div_q(): Argument #1 ($num1) must be of type GMP|string|int, array given
Done
