--TEST--
gmp_pow() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_strval(gmp_pow(2,10)));
var_dump(gmp_strval(gmp_pow(-2,10)));
var_dump(gmp_strval(gmp_pow(-2,11)));
var_dump(gmp_strval(gmp_pow("2",10)));
var_dump(gmp_strval(gmp_pow("2",0)));
var_dump(gmp_strval(gmp_pow("2",-1)));
var_dump(gmp_strval(gmp_pow("-2",10)));
var_dump(gmp_strval(gmp_pow(20,10)));
var_dump(gmp_strval(gmp_pow(50,10)));
var_dump(gmp_strval(gmp_pow(50,-5)));

$n = gmp_init("20");
var_dump(gmp_strval(gmp_pow($n,10)));
$n = gmp_init("-20");
var_dump(gmp_strval(gmp_pow($n,10)));

var_dump(gmp_pow(2,10,1));
var_dump(gmp_pow(2));
var_dump(gmp_pow());
var_dump(gmp_pow(array(), array()));
var_dump(gmp_pow(2,array()));
var_dump(gmp_pow(array(),10));

echo "Done\n";
?>
--EXPECTF--
unicode(4) "1024"
unicode(4) "1024"
unicode(5) "-2048"
unicode(4) "1024"
unicode(1) "1"

Warning: gmp_pow(): Negative exponent not supported in %s on line %d
unicode(1) "0"
unicode(4) "1024"
unicode(14) "10240000000000"
unicode(17) "97656250000000000"

Warning: gmp_pow(): Negative exponent not supported in %s on line %d
unicode(1) "0"
unicode(14) "10240000000000"
unicode(14) "10240000000000"

Warning: gmp_pow() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Warning: gmp_pow() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: gmp_pow() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: gmp_pow() expects parameter 2 to be long, array given in %s on line %d
NULL

Warning: gmp_pow() expects parameter 2 to be long, array given in %s on line %d
NULL

Warning: gmp_pow(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
