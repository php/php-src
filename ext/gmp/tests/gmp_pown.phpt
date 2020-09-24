--TEST--
gmp_powm() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_strval(gmp_powm(0,1,10)));
var_dump(gmp_strval(gmp_powm(5,1,10)));
var_dump(gmp_strval(gmp_powm(-5,1,-10)));
var_dump(gmp_strval(gmp_powm(-5,1,10)));
var_dump(gmp_strval(gmp_powm(-5,11,10)));
var_dump(gmp_strval(gmp_powm("77",3,1000)));

$n = gmp_init(11);
var_dump(gmp_strval(gmp_powm($n,3,1000)));
$e = gmp_init(7);
var_dump(gmp_strval(gmp_powm($n,$e,1000)));
$m = gmp_init(900);
var_dump(gmp_strval(gmp_powm($n,$e,$m)));

try {
    var_dump(gmp_powm(5, 11, 0));
} catch (\DivisionByZeroError $error) {
    echo $error->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_powm(5, "11", gmp_init(0)));
} catch (\DivisionByZeroError $error) {
    echo $error->getMessage() . \PHP_EOL;
}

try {
    var_dump(gmp_powm(array(),$e,$m));
} catch (\TypeError $error) {
    echo $error->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_powm($n,array(),$m));
} catch (\TypeError $error) {
    echo $error->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_powm($n,$error,array()));
} catch (\TypeError $error) {
    echo $error->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_powm(array(),array(),array()));
} catch (\TypeError $error) {
    echo $error->getMessage() . \PHP_EOL;
}

try {
    $n = gmp_init("-5");
    var_dump(gmp_powm(10, $n, 10));
} catch (\ValueError $error) {
    echo $error->getMessage() . \PHP_EOL;
}

$n = gmp_init("0");
var_dump(gmp_powm(10, $n, 10));

echo "Done\n";
?>
--EXPECT--
string(1) "0"
string(1) "5"
string(1) "5"
string(1) "5"
string(1) "5"
string(3) "533"
string(3) "331"
string(3) "171"
string(3) "371"
Modulo by zero
Modulo by zero
gmp_powm(): Argument #1 ($num) must be of type GMP|string|int, array given
gmp_powm(): Argument #2 ($exponent) must be of type GMP|string|int, array given
gmp_powm(): Argument #2 ($exponent) must be of type GMP|string|int, TypeError given
gmp_powm(): Argument #1 ($num) must be of type GMP|string|int, array given
gmp_powm(): Argument #2 ($exponent) must be greater than or equal to 0
object(GMP)#6 (1) {
  ["num"]=>
  string(1) "1"
}
Done
