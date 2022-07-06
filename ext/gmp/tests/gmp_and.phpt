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

try {
    var_dump(gmp_strval(gmp_and("test", "no test")));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$n = gmp_init("987657876543456");
var_dump(gmp_strval(gmp_and($n, "34332")));
$n1 = gmp_init("987657878765436543456");
var_dump(gmp_strval(gmp_and($n, $n1)));


try {
    var_dump(gmp_and(array(), 1));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_and(1, array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_and(array(), array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
string(6) "106502"
string(5) "40994"
string(3) "515"
string(4) "3333"
string(4) "4544"
gmp_and(): Argument #1 ($num1) is not an integer string
string(4) "1536"
string(15) "424703623692768"
gmp_and(): Argument #1 ($num1) must be of type GMP|string|int, array given
gmp_and(): Argument #2 ($num2) must be of type GMP|string|int, array given
gmp_and(): Argument #1 ($num1) must be of type GMP|string|int, array given
Done
