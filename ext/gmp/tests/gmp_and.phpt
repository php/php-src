--TEST--
gmp_and() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_strval(gmp_and("111111", "2222222")));
var_dump(gmp_strval(gmp_and(123123, 435234)));
var_dump(gmp_strval(gmp_and(555, "2342341123")));
var_dump(gmp_strval(gmp_and(-1, 3333)));
var_dump(gmp_strval(gmp_and(4545, -20)));
var_dump(gmp_strval(gmp_and("test", "no test")));

$n = gmp_init("987657876543456");
var_dump(gmp_strval(gmp_and($n, "34332")));
$n1 = gmp_init("987657878765436543456");
var_dump(gmp_strval(gmp_and($n, $n1)));

var_dump(gmp_and($n, $n1, 1));
var_dump(gmp_and(1));
var_dump(gmp_and(array(), 1));
var_dump(gmp_and(1, array()));
var_dump(gmp_and(array(), array()));

echo "Done\n";
?>
--EXPECTF--
string(6) "106502"
string(5) "40994"
string(3) "515"
string(4) "3333"
string(4) "4544"

Warning: gmp_and(): Unable to convert variable to GMP - string is not an integer in %s on line %d
string(1) "0"
string(4) "1536"
string(15) "424703623692768"

Warning: gmp_and() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Warning: gmp_and() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: gmp_and(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_and(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_and(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
