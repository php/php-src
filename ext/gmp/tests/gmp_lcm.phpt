--TEST--
gmp_lcm(): Least common multiple
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump(gmp_lcm(100, 77));
var_dump(gmp_lcm(99, 77));
var_dump(gmp_lcm(99, -77));
var_dump(gmp_lcm(-99, -77));

var_dump(gmp_lcm(gmp_init(99), gmp_init(77)));

var_dump(gmp_lcm(93, 0));
var_dump(gmp_lcm(0, 93));

?>
--EXPECT--
object(GMP)#1 (1) {
  ["num"]=>
  string(4) "7700"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(3) "693"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(3) "693"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(3) "693"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(3) "693"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(1) "0"
}
