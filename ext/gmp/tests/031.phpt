--TEST--
gmp_com() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_strval(gmp_com(0)));
var_dump(gmp_strval(gmp_com("0")));
var_dump(gmp_strval(gmp_com("test")));
var_dump(gmp_strval(gmp_com("2394876545678")));
var_dump(gmp_strval(gmp_com("-111")));
var_dump(gmp_strval(gmp_com(874653)));
var_dump(gmp_strval(gmp_com(-9876)));

$n = gmp_init("98765467");
var_dump(gmp_strval(gmp_com($n)));
$n = gmp_init("98765463337");
var_dump(gmp_strval(gmp_com($n)));

var_dump(gmp_strval(gmp_com(array())));
var_dump(gmp_strval(gmp_com()));

echo "Done\n";
?>
--EXPECTF--	
string(2) "-1"
string(2) "-1"
string(1) "0"
string(14) "-2394876545679"
string(3) "110"
string(7) "-874654"
string(4) "9875"
string(9) "-98765468"
string(12) "-98765463338"

Warning: gmp_com(): Unable to convert variable to GMP - wrong type in %s on line %d
string(1) "0"

Warning: gmp_com() expects exactly 1 parameter, 0 given in %s on line %d

Warning: gmp_strval(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
