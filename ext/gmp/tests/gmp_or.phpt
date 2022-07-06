--TEST--
gmp_or() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_strval(gmp_or("111111", "2222222")));
var_dump(gmp_strval(gmp_or(123123, 435234)));
var_dump(gmp_strval(gmp_or(555, "2342341123")));
var_dump(gmp_strval(gmp_or(-1, 3333)));
var_dump(gmp_strval(gmp_or(4545, -20)));

try {
    var_dump(gmp_strval(gmp_or("test", "no test")));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$n = gmp_init("987657876543456");
var_dump(gmp_strval(gmp_or($n, "34332")));
$n1 = gmp_init("987657878765436543456");
var_dump(gmp_strval(gmp_or($n, $n1)));

try {
    var_dump(gmp_or(array(), 1));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_or(1, array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_or(array(), array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
string(7) "2226831"
string(6) "517363"
string(10) "2342341163"
string(2) "-1"
string(3) "-19"
gmp_or(): Argument #1 ($num1) is not an integer string
string(15) "987657876576252"
string(21) "987658441719689394144"
gmp_or(): Argument #1 ($num1) must be of type GMP|string|int, array given
gmp_or(): Argument #2 ($num2) must be of type GMP|string|int, array given
gmp_or(): Argument #1 ($num1) must be of type GMP|string|int, array given
Done
