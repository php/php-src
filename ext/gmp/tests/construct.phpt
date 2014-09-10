--TEST--
Creating GMP instances
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

$a = new GMP();
$b = new GMP(123);
$c = new GMP('456');
$d = new GMP('ff', 16);
$e = new GMP('nan', 2);
$f = new GMP('000', 1);

var_dump($a, $b, $c, $d, $e, $f);
--EXPECTF--

Warning: GMP::__construct(): Unable to convert variable to GMP - string is not an integer in %s on line %d

Warning: GMP::__construct(): Bad base for conversion: 1 (should be between 2 and 62) in %s on line %d
object(GMP)#1 (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(3) "123"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(3) "456"
}
object(GMP)#4 (1) {
  ["num"]=>
  string(3) "255"
}
NULL
NULL
