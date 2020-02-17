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

try {
    var_dump(gmp_pow(2,array()));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump(gmp_pow(array(),10));

echo "Done\n";
?>
--EXPECTF--
string(4) "1024"
string(4) "1024"
string(5) "-2048"
string(4) "1024"
string(1) "1"

Warning: gmp_pow(): Negative exponent not supported in %s on line %d
string(1) "0"
string(4) "1024"
string(14) "10240000000000"
string(17) "97656250000000000"

Warning: gmp_pow(): Negative exponent not supported in %s on line %d
string(1) "0"
string(14) "10240000000000"
string(14) "10240000000000"
gmp_pow() expects argument #2 ($exp) to be of type int, array given

Warning: gmp_pow(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
