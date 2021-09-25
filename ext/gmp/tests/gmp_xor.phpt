--TEST--
gmp_xor() basic tests
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump(gmp_strval(gmp_xor("111111", "2222222")));
var_dump(gmp_strval(gmp_xor(123123, 435234)));
var_dump(gmp_strval(gmp_xor(555, "2342341123")));
var_dump(gmp_strval(gmp_xor(-1, 3333)));
var_dump(gmp_strval(gmp_xor(4545, -20)));

try {
    var_dump(gmp_strval(gmp_xor("test", "no test")));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$n = gmp_init("987657876543456");
var_dump(gmp_strval(gmp_xor($n, "34332")));
$n1 = gmp_init("987657878765436543456");
var_dump(gmp_strval(gmp_xor($n, $n1)));

try {
    var_dump(gmp_xor(array(), 1));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_xor(1, array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_xor(array(), array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
string(7) "2120329"
string(6) "476369"
string(10) "2342340648"
string(5) "-3334"
string(5) "-4563"
gmp_xor(): Argument #1 ($num1) is not an integer string
string(15) "987657876574716"
string(21) "987658017016065701376"
gmp_xor(): Argument #1 ($num1) must be of type GMP|string|int, array given
gmp_xor(): Argument #2 ($num2) must be of type GMP|string|int, array given
gmp_xor(): Argument #1 ($num1) must be of type GMP|string|int, array given
Done
