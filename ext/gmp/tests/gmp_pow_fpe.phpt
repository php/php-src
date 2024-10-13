--TEST--
gmp_pow() floating point exception
--EXTENSIONS--
gmp
--FILE--
<?php
$g = gmp_init(256);

var_dump(gmp_pow($g, PHP_INT_MAX));
var_dump(gmp_pow(256, PHP_INT_MAX));
?>
--EXPECTF--
object(GMP)#2 (1) {
  ["num"]=>
  string(%d) "%s"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(%d) "%s"
}
