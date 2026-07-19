--TEST--
GH-13661: Add gmp_prevprime()
--EXTENSIONS--
gmp
--SKIPIF--
<?php
if (!function_exists('gmp_prevprime')) {
    die('skip gmp_prevprime() is not available');
}
?>
--FILE--
<?php

var_dump(gmp_prevprime(-1));
var_dump(gmp_prevprime(0));
var_dump(gmp_prevprime(1));
var_dump(gmp_prevprime(2));
var_dump(gmp_prevprime(3));
var_dump(gmp_prevprime(4));
var_dump(gmp_prevprime(10000));

?>
--EXPECTF--
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "2"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "3"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(4) "9973"
}
