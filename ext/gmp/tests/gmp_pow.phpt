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
try {
    gmp_pow("2", -1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
var_dump(gmp_strval(gmp_pow("-2",10)));
var_dump(gmp_strval(gmp_pow(20,10)));
var_dump(gmp_strval(gmp_pow(50,10)));
try {
    gmp_pow(50,-5);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

$n = gmp_init("20");
var_dump(gmp_strval(gmp_pow($n,10)));
$n = gmp_init("-20");
var_dump(gmp_strval(gmp_pow($n,10)));

try {
    var_dump(gmp_pow(2,array()));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(gmp_pow(array(),10));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
string(4) "1024"
string(4) "1024"
string(5) "-2048"
string(4) "1024"
string(1) "1"
gmp_pow(): Argument #2 ($exponent) must be greater than or equal to 0
string(4) "1024"
string(14) "10240000000000"
string(17) "97656250000000000"
gmp_pow(): Argument #2 ($exponent) must be greater than or equal to 0
string(14) "10240000000000"
string(14) "10240000000000"
gmp_pow(): Argument #2 ($exponent) must be of type int, array given
gmp_pow(): Argument #1 ($num) must be of type GMP|string|int, array given
Done
