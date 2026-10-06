--TEST--
Constructor for GMP on 64bit int
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) print "skip, this test is for 64bit int only";
?>
--EXTENSIONS--
gmp
--FILE--
<?php
var_dump(new GMP(PHP_INT_MAX));
var_dump(new GMP((string)PHP_INT_MAX));
var_dump(new GMP('0x7FFFFFFFFFFFFFFF'));
var_dump(new GMP(PHP_INT_MIN));
var_dump(new GMP((string)PHP_INT_MIN));

/* Magnitudes a signed 32bit long cannot hold. PHP_INT_MAX and PHP_INT_MIN
 * above do not cover this: the former is positive and the latter is the one
 * negative value whose magnitude equals its own two's complement pattern. */
var_dump(new GMP(2147483648));
var_dump(new GMP(-2147483649));
var_dump(new GMP(-3000000000));
--EXPECT--
object(GMP)#1 (1) {
  ["num"]=>
  string(19) "9223372036854775807"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(19) "9223372036854775807"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(19) "9223372036854775807"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(20) "-9223372036854775808"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(20) "-9223372036854775808"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(10) "2147483648"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(11) "-2147483649"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(11) "-3000000000"
}
