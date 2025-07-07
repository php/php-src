--TEST--
Constructor for GMP on 32bit int
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) print "skip, this test is for 32bit int only";
?>
--EXTENSIONS--
gmp
--FILE--
<?php
var_dump(new GMP(PHP_INT_MAX));
var_dump(new GMP((string)PHP_INT_MAX));
var_dump(new GMP('0x7FFFFFFF'));
var_dump(new GMP(PHP_INT_MIN));
var_dump(new GMP((string)PHP_INT_MIN));
--EXPECT--
object(GMP)#1 (1) {
  ["num"]=>
  string(10) "2147483647"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(10) "2147483647"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(10) "2147483647"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(11) "-2147483648"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(11) "-2147483648"
}
