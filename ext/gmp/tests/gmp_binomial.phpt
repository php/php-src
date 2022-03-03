--TEST--
gmp_binomial(): Binomial coefficients
--EXTENSIONS--
gmp
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

try {
    var_dump(gmp_binomial(5, -2));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
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
gmp_binomial(): Argument #2 ($k) must be greater than or equal to 0
