--TEST--
gmp_root() basic tests
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump(gmp_root(1000, 3));
var_dump(gmp_root(100, 3));
var_dump(gmp_root(-100, 3));

var_dump(gmp_root(1000, 4));
var_dump(gmp_root(100, 4));

try {
    var_dump(gmp_root(-100, 4));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(gmp_root(0, 3));

try {
    var_dump(gmp_root(100, 0));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_root(100, -3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECTF--
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
gmp_root(): Argument #2 ($nth) must be odd if argument #1 ($a) is negative
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "0"
}
gmp_root(): Argument #2 ($nth) must be greater than 0
gmp_root(): Argument #2 ($nth) must be greater than 0
