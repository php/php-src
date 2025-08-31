--TEST--
gmp_fact() basic tests
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump(gmp_strval(gmp_fact(0)));
try {
    var_dump(gmp_strval(gmp_fact("")));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(gmp_strval(gmp_fact("0")));
try {
    var_dump(gmp_strval(gmp_fact("-1")));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_strval(gmp_fact(-1)));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(gmp_strval(gmp_fact(20)));
var_dump(gmp_strval(gmp_fact("50")));
var_dump(gmp_strval(gmp_fact("10")));
var_dump(gmp_strval(gmp_fact("0000")));

$n = gmp_init(12);
var_dump(gmp_strval(gmp_fact($n)));
$n = gmp_init(-10);
try {
    var_dump(gmp_strval(gmp_fact($n)));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(gmp_fact(array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
string(1) "1"
gmp_fact(): Argument #1 ($num) is not an integer string
string(1) "1"
gmp_fact(): Argument #1 ($num) must be greater than or equal to 0
gmp_fact(): Argument #1 ($num) must be greater than or equal to 0
string(19) "2432902008176640000"
string(65) "30414093201713378043612608166064768844377641568960512000000000000"
string(7) "3628800"
string(1) "1"
string(9) "479001600"
gmp_fact(): Argument #1 ($num) must be greater than or equal to 0
gmp_fact(): Argument #1 ($num) must be of type GMP|string|int, array given
Done
