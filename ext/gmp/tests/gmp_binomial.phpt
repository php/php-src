--TEST--
gmp_binomial(): Binomial coefficients
--SKIPIF--
<?php if (!extension_loaded('gmp')) die('skip gmp extension not loaded'); ?>
--FILE--
<?php

var_dump(gmp_binomial(10, 5));
var_dump(gmp_binomial("10", 5));
$n = gmp_init(10);
var_dump(gmp_binomial($n, 5));

var_dump(gmp_binomial(10000, 100));

var_dump(gmp_binomial(0, 0));
var_dump(gmp_binomial(0, 1));
var_dump(gmp_binomial(1, 0));
var_dump(gmp_binomial(1, 1));

var_dump(gmp_binomial(-1, 5)); // == -(1 + 5 - 1 over 5)
var_dump(gmp_binomial(-2, 6)); // == (2 + 6 - 1 over 6)

var_dump(gmp_binomial(5, -2));

?>
--EXPECTF--
object(GMP)#1 (1) {
  ["num"]=>
  string(3) "252"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(3) "252"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(3) "252"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(242) "65208469245472575695415972927215718683781335425416743372210247172869206520770178988927510291340552990847853030615947098118282371982392705479271195296127415562705948429404753632271959046657595132854990606768967505457396473467998111950929802400"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "1"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "1"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "1"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "-1"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "7"
}

Warning: gmp_binomial(): k cannot be negative in %s on line %d
bool(false)
