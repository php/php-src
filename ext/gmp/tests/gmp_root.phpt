--TEST--
gmp_root() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_root());

var_dump(gmp_root(1000, 3));
var_dump(gmp_root(100, 3));
var_dump(gmp_root(-100, 3));

var_dump(gmp_root(1000, 4));
var_dump(gmp_root(100, 4));
var_dump(gmp_root(-100, 4));

var_dump(gmp_root(0, 3));
var_dump(gmp_root(100, 0));
var_dump(gmp_root(100, -3));

?>
--EXPECTF--	
Warning: gmp_root() expects exactly 2 parameters, 0 given in %s on line %d
NULL
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "10"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "4"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "-4"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "5"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "3"
}

Warning: gmp_root(): Can't take even root of negative number in %s on line %d
bool(false)
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "0"
}

Warning: gmp_root(): The root must be positive in %s on line %d
bool(false)

Warning: gmp_root(): The root must be positive in %s on line %d
bool(false)
