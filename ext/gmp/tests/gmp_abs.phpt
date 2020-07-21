--TEST--
gmp_abs() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

try {
    var_dump(gmp_strval(gmp_abs("")));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(gmp_strval(gmp_abs("0")));
var_dump(gmp_strval(gmp_abs(0)));
try {
    var_dump(gmp_strval(gmp_abs(-111111111111111111111))); // This is a float
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(gmp_strval(gmp_abs("111111111111111111111")));
var_dump(gmp_strval(gmp_abs("-111111111111111111111")));
var_dump(gmp_strval(gmp_abs("0000")));

try {
    // Base 8
    var_dump(gmp_strval(gmp_abs("09876543")));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    // Base 8
    var_dump(gmp_strval(gmp_abs("-099987654")));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}


try {
    var_dump(gmp_abs(array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
gmp_abs(): Argument #1 ($a) must be an integer string in base 10
string(1) "0"
string(1) "0"
gmp_abs(): Argument #1 ($a) must be of type bool|int|string|GMP, float given
string(21) "111111111111111111111"
string(21) "111111111111111111111"
string(1) "0"
gmp_abs(): Argument #1 ($a) must be an integer string in base 8
gmp_abs(): Argument #1 ($a) must be an integer string in base 8
gmp_abs(): Argument #1 ($a) must be of type bool|int|string|GMP, array given
Done
