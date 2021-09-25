--TEST--
gmp_invert() basic tests
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump(gmp_strval(gmp_invert(123123,5467624)));
var_dump(gmp_strval(gmp_invert(123123,"3333334345467624")));
var_dump(gmp_strval(gmp_invert("12312323213123123",7624)));

try {
    var_dump(gmp_strval(gmp_invert(444,0)));
} catch (\DivisionByZeroError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(gmp_strval(gmp_invert(0,28347)));
var_dump(gmp_strval(gmp_invert(-12,456456)));
var_dump(gmp_strval(gmp_invert(234234,-435345)));

$n = gmp_init("349827349623423452345");
$n1 = gmp_init("3498273496234234523451");

var_dump(gmp_strval(gmp_invert($n, $n1)));
var_dump(gmp_strval(gmp_invert($n1, $n)));

try {
    var_dump(gmp_invert(array(), 1));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_invert(1, array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_invert(array(), array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
string(7) "2293131"
string(1) "0"
string(4) "5827"
Division by zero
string(1) "0"
string(1) "0"
string(1) "0"
string(22) "3498273496234234523441"
string(1) "1"
gmp_invert(): Argument #1 ($num1) must be of type GMP|string|int, array given
gmp_invert(): Argument #2 ($num2) must be of type GMP|string|int, array given
gmp_invert(): Argument #1 ($num1) must be of type GMP|string|int, array given
Done
