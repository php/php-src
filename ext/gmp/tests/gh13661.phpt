--TEST--
gmp_prevprime()
--EXTENSIONS--
gmp
--SKIPIF--
<?php if (!function_exists("gmp_prevprime")) die("skip no gmp_prevprime"); ?>
--FILE--
<?php

var_dump(gmp_strval(gmp_prevprime(-1)));
var_dump(gmp_strval(gmp_prevprime(1)));
var_dump(gmp_strval(gmp_prevprime(3)));
var_dump(gmp_strval(gmp_prevprime(gmp_init("3"))));
var_dump(gmp_strval(gmp_prevprime(gmp_init("10003"))));
var_dump(gmp_strval(gmp_prevprime(gmp_init("-10003"))));
var_dump(gmp_strval(gmp_prevprime(gmp_init(PHP_INT_MAX))));

?>
--EXPECTF--
string(1) "0"
string(1) "0"
string(1) "2"
string(1) "2"
string(4) "9973"
string(1) "0"
string(%d) "%d"
