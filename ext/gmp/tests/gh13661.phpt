--TEST--
gmp_prevprime()
--EXTENSIONS--
gmp
--SKIPIF--
<?php if (!function_exists("gmp_prevprime")) die("skip no gmp_prevprime"); ?>
--FILE--
<?php

var_dump(gmp_prevprime(-1));
var_dump(gmp_prevprime(1));
var_dump(gmp_prevprime(3));
var_dump(gmp_prevprime(gmp_init("3")));
var_dump(gmp_prevprime(gmp_init(PHP_INT_MAX)));

?>
--EXPECTF--
object(GMP)#1 (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(1) "2"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(1) "2"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(19) "%d"
}
