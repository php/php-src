--TEST--
PHP_INT_MAX tests
--EXTENSIONS--
gmp
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) print "skip";
?>
--FILE--
<?php

var_dump(gmp_mul(PHP_INT_MAX, PHP_INT_MAX));
var_dump(gmp_add(PHP_INT_MAX, PHP_INT_MAX));
var_dump(gmp_mul(PHP_INT_MAX, PHP_INT_MIN));
?>
DONE
--EXPECTF--
object(GMP)#%d (%d) {
  ["num"]=>
  string(38) "85070591730234615847396907784232501249"
}
object(GMP)#%d (%d) {
  ["num"]=>
  string(20) "18446744073709551614"
}
object(GMP)#%d (%d) {
  ["num"]=>
  string(39) "-85070591730234615856620279821087277056"
}
DONE
