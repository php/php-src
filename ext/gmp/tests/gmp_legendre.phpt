--TEST--
gmp_legendre() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

for ($i = -1; $i < 10; $i++) {
    var_dump(gmp_strval(gmp_legendre(($i*$i)-1, 3)));
}

var_dump(gmp_strval(gmp_legendre(7, 23)));
var_dump(gmp_strval(gmp_legendre("733535124", "1234123423434535623")));
var_dump(gmp_strval(gmp_legendre(3, "1234123423434535623")));

$n = "123123";
$n1 = "1231231";

var_dump(gmp_strval(gmp_legendre($n, $n1)));
var_dump(gmp_strval(gmp_legendre($n, 3)));
var_dump(gmp_strval(gmp_legendre(3, $n1)));

try {
    var_dump(gmp_legendre(3, array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_legendre(array(), 3));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_legendre(array(), array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
string(1) "0"
string(2) "-1"
string(1) "0"
string(1) "0"
string(2) "-1"
string(1) "0"
string(1) "0"
string(2) "-1"
string(1) "0"
string(1) "0"
string(2) "-1"
string(2) "-1"
string(1) "0"
string(1) "0"
string(2) "-1"
string(1) "0"
string(2) "-1"
gmp_legendre(): Argument #2 ($num2) must be of type GMP|string|int, array given
gmp_legendre(): Argument #1 ($num1) must be of type GMP|string|int, array given
gmp_legendre(): Argument #1 ($num1) must be of type GMP|string|int, array given
Done
