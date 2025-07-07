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
