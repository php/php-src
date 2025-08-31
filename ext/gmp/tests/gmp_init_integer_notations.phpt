--TEST--
gmp_init() with various integer notations
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump(gmp_init("0x16"));
var_dump(gmp_init("0X16"));
var_dump(gmp_init("0o16"));
var_dump(gmp_init("0o16"));
var_dump(gmp_init("016"));
var_dump(gmp_init("016"));
var_dump(gmp_init("0b11"));
var_dump(gmp_init("0b11"));

?>
--EXPECT--
object(GMP)#1 (1) {
  ["num"]=>
  string(2) "22"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(2) "22"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(2) "14"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(2) "14"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(2) "14"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(2) "14"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(1) "3"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(1) "3"
}
